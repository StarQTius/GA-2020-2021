#include <catch.hpp>
#include "../neural_engine.hpp"

#include <iterator>
#include <random>

#include <SFML/Graphics.hpp>

#include "../../../genetics/genetics.hpp"
#include "../../../seed.hpp"
#include "../../entity/robot_features.hpp"

Setpoint dont_move(entt::entity, entt::registry&) {
  return { .speed = 0_q_m_per_s, .angular_speed = 0_q_rad_per_s };
}

double fetch_nothing(entt::entity, entt::registry&) {
  return 0.0;
}

TEST_CASE("NeuralEngine : donner une consigne en fonction de l'environement") {
  RobotFeatures robot_features {
    .hitbox { 5_q_cm },
    .strategy_ftor = dont_move,
    .shape = sf::CircleShape(0),
    .goal_mark_shape = sf::CircleShape(0)
  };

  SECTION("La consigne donnée en retour doit vérifier au moins l'un des deux : "
          "soit la consigne de vitesse est à 1 et celle de vitesse angulaire est "
          "à 0, soit la consigne de vitesse est à 0 et celle de vitesse angulaire "
          "à 1 ou -1") {
    entt::registry registry;
    auto seed = generate_seed();
    registry.set<std::mt19937>(seed);
    registry.set<TrialParameters>(
      TrialParameters {
        .playground {
          .left = 0_q_m,
          .top = 0_q_m,
          . width = 100_q_m,
          .height = 100_q_m },
        .foe_nb = 2,
        .seed = seed,
        .dt = 1e-3_q_s,
        .time_limit = 100_q_s
      });

    for (int i = 0; i < 100; i++) {
      auto candidate = create_robot(registry, robot_features, false);
      for (int j = 0; j < 2; j++) create_robot(registry, robot_features, false);

      NeuralEngine neural_engine(20, fetch_nothing);
      auto setpoint = neural_engine(candidate, registry);
      bool just_moving = setpoint.speed == 1_q_m_per_s
                      && setpoint.angular_speed == 0_q_rad_per_s;
      bool just_turning = setpoint.speed == 0_q_m_per_s
                       && (setpoint.angular_speed == 1_q_rad_per_s
                       || setpoint.angular_speed == -1_q_rad_per_s);
      bool is_valid = just_moving || just_turning;
      REQUIRE(is_valid);

      registry.clear();
    }
  }
}

TEST_CASE("NeuralEngine : itérer les poids") {
  SECTION("La méthode 'view' retourne un container satisfaisant 'std::ranges::range' "
          "qui permet d'accéder aux poids lorsqu'il est itéré") {
    NeuralEngine neural_engine(10, fetch_nothing);
    auto weights = neural_engine.view();

    REQUIRE(std::distance(weights.begin(), weights.end()) == 1 * 10 + 10 * 3);
  }
}

TEST_CASE("NeuralEngine : entraîner le réseau interne") {
  SECTION("NeuralEngine est compatible avec 'genetics::mutate', ce qui permet de "
          "modifier aléatoirement l'état du réseau interne") {
    NeuralEngine neural_engine(10, fetch_nothing);
    genetics::mutate(neural_engine.view(), 0.5, [&](auto& x, const auto&) { x = 0; });

    CHECK(std::abs(ltl::count(neural_engine.view(), 0) - 40) <= 15);
  }

  SECTION("NeuralEngine est compatible avec genetics::crossover_uniform, ce qui "
          "permet de croiser les poids de deux instances de NeuralEngine") {
    NeuralEngine neural_engine1(10, fetch_nothing), neural_engine2(10, fetch_nothing);
    for (auto& weight : neural_engine1.view()) weight = 0;
    for (auto& weight : neural_engine2.view()) weight = 1;
    genetics::crossover_uniform(neural_engine1.view(), neural_engine2.view());

    REQUIRE(ltl::count(neural_engine1.view(), 0) + ltl::count(neural_engine2.view(), 0) == 40);
    REQUIRE(ltl::count(neural_engine1.view(), 1) + ltl::count(neural_engine2.view(), 1) == 40);
  }
}
