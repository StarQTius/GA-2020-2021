#include <catch.hpp>
#include "../net.hpp"

#include <list>

using namespace neural;

struct MockLayer {
  MockLayer(int input_size, int output_size)
    : _input_size(input_size),
      _output_size(output_size) {}

  auto operator<<(int value) { return value + _input_size - _output_size; }

  int input_size() const { return _input_size; };
  int output_size() const { return _output_size; };

  int _input_size;
  int _output_size;

};

struct MovableOnlyLayer : MockLayer {
  using MockLayer::MockLayer;
  MovableOnlyLayer(MovableOnlyLayer&&) = default;
  MovableOnlyLayer& operator=(MovableOnlyLayer&&) = default;
};

struct ResizableLayer : MockLayer {
  using MockLayer::MockLayer;
  void resize(int, int) { has_been_resized = true; }
  bool has_been_resized = false;
};

struct MoveAwareLayer : MockLayer {
  using MockLayer::MockLayer;
  MoveAwareLayer(MoveAwareLayer&& rhs)
    : MockLayer(rhs), has_been_moved(true), moves_nb(rhs.moves_nb + 1)
  {}

  MoveAwareLayer& operator=(MoveAwareLayer&& rhs) {
    MockLayer::operator=(rhs);
    has_been_moved = true;
    moves_nb += rhs.moves_nb + 1;
    return *this;
  }

  bool has_been_moved = false;
  int  moves_nb = 0;
};


TEST_CASE("Net : construire un Net en passant les couches directement") {
  auto net = make_net(
    MockLayer(3, 5),
    MockLayer(5, 2),
    MockLayer(2, 8)
  );

  SECTION("L'opérateur >> propage la valeur d'entrée, en appelant succesivement"
          "les opérateurs >> de chaque couche et en passant la sortie d'une"
          "couche à l'entrée de la couche suivante") {
    REQUIRE(net << 8 == 8 + 3 - 8);
    REQUIRE(net << 16 == 16 + 3 - 8);
    REQUIRE(net << 0 == 0 + 3 - 8);
  }
}

TEST_CASE("Net : construire un Net en donnant la forme du réseau") {
  auto net = Net<MockLayer, MockLayer, MockLayer> {3, 5, 2, 8};

  SECTION("L'opérateur >> propage la valeur d'entrée, en appelant succesivement"
          "les opérateurs >> de chaque couche et en passant la sortie d'une"
          "couche à l'entrée de la couche suivante") {
    REQUIRE(net << 8 == 8 + 3 - 8);
    REQUIRE(net << 16 == 16 + 3 - 8);
    REQUIRE(net << 0 == 0 + 3 - 8);
  }
}

TEST_CASE("Net : construire un Net en donnant la forme du réseau à partir d'un "
          "objet satisfaisant std::ranges::range") {
  auto net = Net<MockLayer, MockLayer, MockLayer>(std::list {3, 5, 2, 8});

  SECTION("L'opérateur >> propage la valeur d'entrée, en appelant succesivement"
          "les opérateurs >> de chaque couche et en passant la sortie d'une"
          "couche à l'entrée de la couche suivante") {
    REQUIRE(net << 8 == 8 + 3 - 8);
    REQUIRE(net << 16 == 16 + 3 - 8);
    REQUIRE(net << 0 == 0 + 3 - 8);
  }
}

TEST_CASE("Net : redimensionner le réseau") {
  auto net = Net<MovableOnlyLayer, MockLayer> {1, 2, 3};

  SECTION("La fonction membre 'resize' permet de changer la taille d'une couche") {
    REQUIRE(net[0_n].output_size() == 2);
    REQUIRE(net[1_n].input_size() == 2);

    net.resize(1_n, 5);

    REQUIRE(net[0_n].output_size() == 5);
    REQUIRE(net[1_n].input_size() == 5);
  }

  SECTION("'resize' permet de changer la taille de la couche d'entrée du réseau") {
    REQUIRE(net[0_n].input_size() == 1);

    net.resize(0_n, 4);

    REQUIRE(net[0_n].input_size() == 4);
  }

  SECTION("'resize' permet de changer la taille de la couche de sortie du réseau") {
    REQUIRE(net[1_n].output_size() == 3);

    net.resize(2_n, 6);

    REQUIRE(net[1_n].output_size() == 6);
  }

  auto net2 = Net<MoveAwareLayer, ResizableLayer> {1, 1, 1};

  SECTION("Si elle est définie, 'resize' appelle la fonction membre 'resize' de "
          "de la couche redimensionnée; Sinon, 'resize' construit une nouvelle "
          "couche et l'assigne par 'move-assignement'") {
    // La couche est 'moved' lors de l'appel au constructeur de Net
    REQUIRE(!net2[1_n].has_been_resized);

    net2[0_n].has_been_moved = false;
    net2.resize(1_n, 2);

    REQUIRE(net2[0_n].has_been_moved);
    REQUIRE(net2[1_n].has_been_resized);
  }

  auto net3 = Net<MoveAwareLayer, MoveAwareLayer, MoveAwareLayer, MoveAwareLayer, MoveAwareLayer> {1, 1, 1, 1, 1, 1};

  SECTION("On peut redimensionner plusieurs couches adjacentes avec un nombre "
          "minimal d'opération") {
    REQUIRE(net3[0_n].moves_nb == 2);
    REQUIRE(net3[1_n].moves_nb == 2);
    REQUIRE(net3[2_n].moves_nb == 2);
    REQUIRE(net3[3_n].moves_nb == 2);
    REQUIRE(net3[4_n].moves_nb == 2);

    net3.resize(2_n, 4_n, {10, 11});

    REQUIRE(net3[0_n].input_size() == 1);
    REQUIRE(net3[1_n].input_size() == 1);
    REQUIRE(net3[2_n].input_size() == 10);
    REQUIRE(net3[3_n].input_size() == 11);
    REQUIRE(net3[4_n].input_size() == 1);

    REQUIRE(net3[0_n].output_size() == 1);
    REQUIRE(net3[1_n].output_size() == 10);
    REQUIRE(net3[2_n].output_size() == 11);
    REQUIRE(net3[3_n].output_size() == 1);
    REQUIRE(net3[4_n].output_size() == 1);

    REQUIRE(net3[0_n].moves_nb == 2);
    REQUIRE(net3[1_n].moves_nb == 3);
    REQUIRE(net3[2_n].moves_nb == 3);
    REQUIRE(net3[3_n].moves_nb == 3);
    REQUIRE(net3[4_n].moves_nb == 2);
  }
}
