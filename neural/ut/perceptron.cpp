#include <catch.hpp>
#include "../perceptron.hpp"

#include "../net.hpp"

using namespace neural;

TEST_CASE("Perceptron : calculer l'image d'un vecteur par un perceptron") {
  uint8_t calls_nb;
  Vector<double> input(3);
  input << 1, 2, 3;

  Matrix<double> identity_matrix(3, 3);
  identity_matrix << 1, 0, 0,
                     0, 1, 0,
                     0, 0, 1;

  SECTION("Lorsque la fonction d'activation est vectorisable, elle est appellée "
          "pour chaque coefficient du vecteur de sortie") {
    auto doubler_function = [&](double scalar) { calls_nb++; return 2 * scalar; };
    Perceptron perceptron {
      .weights = identity_matrix,
      .activation_function = doubler_function
    };
    calls_nb = 0;

    REQUIRE(perceptron << input == 2 * input);
    REQUIRE(calls_nb == 3);
  }

  SECTION("Lorsque la fonction d'activation est vectorisée et qu'elle modifie "
          "le vecteur passée en argument, il n'y a qu'un seul appel") {
    auto doubler_modifying_application = [&](Vector<auto>& input) {
      calls_nb++;
      input *= 2;
    };
    Perceptron perceptron {
      .weights = identity_matrix,
      .activation_function = doubler_modifying_application
    };
    calls_nb = 0;

    REQUIRE(perceptron << input == 2 * input);
    REQUIRE(input != 2 * input);
    REQUIRE(calls_nb == 1);
  }

  SECTION("Lorsque la fonction d'activation est vectorisée et qu'elle retourne "
          "le vecteur passée en argument, il n'y a qu'un seul appel") {
    auto doubler_nonmodifying_application = [&](const Vector<auto>& input) {
      calls_nb++;
      return 2 * input;
    };
    Perceptron perceptron {
      .weights = identity_matrix,
      .activation_function = doubler_nonmodifying_application
    };
    calls_nb = 0;

    REQUIRE(perceptron << input == 2 * input);
    REQUIRE(calls_nb == 1);
  }

  SECTION("Le vecteur passé en entrée du perceptron est multiplié par la gauche "
          "par la matrice de poids, puis le résultat est passé à la fonction "
          "d'activation") {
    auto identity_function = [](auto value) { return value; };
    Matrix<double> matrix(3, 3);
    matrix << 1, 0, 0,
              1, 1, 1,
              1, 0, 1;
    Perceptron perceptron {
      .weights = matrix,
      .activation_function = identity_function
    };

    Vector<double> output(3);
    output << 1, 6, 4;

    REQUIRE(perceptron << input == output);
  }
}

TEST_CASE("Perceptron : calculer l'image d'une matrice par un perceptron") {
  uint8_t calls_nb;
  Matrix<double> input(3, 3);
  input << 1, 2, 3, 4, 5, 6, 7, 8, 9;

  Matrix<double> identity_matrix(3, 3);
  identity_matrix << 1, 0, 0,
                     0, 1, 0,
                     0, 0, 1;

  SECTION("Lorsque la fonction d'activation est vectorisable, elle est appellée "
          "pour chaque coefficient de la matrice sortie") {
    auto doubler_function = [&](double scalar) { calls_nb++; return 2 * scalar; };
    Perceptron perceptron {
      .weights = identity_matrix,
      .activation_function = doubler_function
    };
    calls_nb = 0;

    REQUIRE(perceptron << input == 2 * input);
    REQUIRE(calls_nb == 9);
  }

  SECTION("Lorsque la fonction d'activation est vectorisée et qu'elle modifie la "
          "matrice passée en argument, il n'y a qu'un seul appel") {
    auto doubler_modifying_application = [&](Matrix<auto>& input) {
      calls_nb++;
      input *= 2;
    };
    Perceptron perceptron {
      .weights = identity_matrix,
      .activation_function = doubler_modifying_application
    };
    calls_nb = 0;

    REQUIRE(perceptron << input == 2 * input);
    REQUIRE(input != 2 * input);
    REQUIRE(calls_nb == 1);
  }

  SECTION("Lorsque la fonction d'activation est vectorisée et qu'elle modifie la "
          "matrice passée en argument, il n'y a qu'un seul appel") {
    auto doubler_modifying_application = [&](const Matrix<auto>& input) {
      calls_nb++;
      return input * 2;
    };
    Perceptron perceptron {
      .weights = identity_matrix,
      .activation_function = doubler_modifying_application
    };
    calls_nb = 0;

    REQUIRE(perceptron << input == 2 * input);
    REQUIRE(calls_nb == 1);
  }

  SECTION("La matrice passée en entrée du perceptron est multiplié par la gauche "
          "par la matrice de poids, puis le résultat est passé à la fonction "
          "d'activation") {
    auto identity_function = [](auto value) { return value; };
    Matrix<double> matrix(3, 3);
    matrix << 1, 0, 0,
              1, 1, 1,
              1, 0, 1;
    Perceptron perceptron {
      .weights = matrix,
      .activation_function = identity_function
    };

    Matrix<double> output(3, 3);
    output << 1, 2, 3,
              12, 15, 18,
              8, 10, 12;

    REQUIRE(perceptron << input == output);
  }
}
