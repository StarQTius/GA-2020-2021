#include <catch.hpp>

#include <random> //uniform_real_distribution

#include <cmath>

#include "../neural_network.hpp"

TEST_CASE("gate::NeuralNetwork"){

  gate::NeuralNetwork neural_network{
    {8, 25, 2},
    std::uniform_real_distribution{-1.0, 1.0},
    [](float x){
      return 1 / (1 + std::exp(-x));
    }
  };

  SECTION("Le ctor de NeuralNetwork donne la bonne forme à sa carte neuronale"){

    REQUIRE(neural_network.neural_map.size() == 2);
    REQUIRE(neural_network.neural_map[0].first.size1() == 25);
    REQUIRE(neural_network.neural_map[0].first.size2() == 8);
    REQUIRE(neural_network.neural_map[0].second.size() == 25);
    REQUIRE(neural_network.neural_map[1].first.size1() == 2);
    REQUIRE(neural_network.neural_map[1].first.size2() == 25);
    REQUIRE(neural_network.neural_map[1].second.size() == 2);


  }

  SECTION("Le résulat du produit d'un objet Vector et d'un objet NeuralNetwork"
          " est un vecteur dont les composantes sont dans [0,1]")
  {

    gate::Vector input_vector{8, 0.125f};
    gate::Vector output_vector{neural_network(input_vector)};
    REQUIRE(output_vector.size() == 2);
    REQUIRE(0 <= output_vector[0]); REQUIRE(output_vector[0] <= 1);
    REQUIRE(0 <= output_vector[1]); REQUIRE(output_vector[1] <= 1);

  }

  gate::NeuralNetwork def_network{
    {3, 3, 3},
    std::uniform_real_distribution{-1.0, 1.0},
    [](float x){
      return x;
    }
  };
  def_network.neural_map[0].first = ublas::identity_matrix{3};
  def_network.neural_map[0].first(2, 2) = 0.0;
  def_network.neural_map[0].second[0] = 0.0;
  def_network.neural_map[0].second[1] = 0.0;
  def_network.neural_map[0].second[2] = 0.0;


  def_network.neural_map[1].first = gate::WeightMatrix{3, 3, 0};
  def_network.neural_map[1].first(0, 0) = 3.0;
  def_network.neural_map[1].first(0, 1) = 4.0;
  def_network.neural_map[1].first(0, 2) = 5.0;
  def_network.neural_map[1].first(1, 0) = 8.0;
  def_network.neural_map[1].first(1, 1) = 1.0;
  def_network.neural_map[1].first(1, 2) = 3.0;
  def_network.neural_map[1].first(2, 0) = 5.0;
  def_network.neural_map[1].first(2, 1) = 4.0;
  def_network.neural_map[1].first(2, 2) = 0.0;
  def_network.neural_map[1].second[0] = -4.0;
  def_network.neural_map[1].second[1] = 1.0;
  def_network.neural_map[1].second[2] = -5.0;


  SECTION("Le résulat du produit d'un objet Vector et d'un objet NeuralNetwork"
          " est le bon résultat"){

    auto output_vector = def_network(gate::Vector{3, 1.0});
    gate::Vector expct_output_vector{3};
    expct_output_vector[0] = 3.0;
    expct_output_vector[1] = 10.0;
    expct_output_vector[2] = 4.0;
    REQUIRE(ublas::norm_inf(output_vector - expct_output_vector) < 0.0000001f);

  }

}
