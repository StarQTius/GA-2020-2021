#include <catch.hpp>
#include "../trial.hpp"

#include <iostream>
#include <string>

#include <ltl/algos.h>
#include <ltl/Range/Filter.h>

auto get_delta(entt::entity entity, entt::registry& registry, const Position& translation) {
  auto direction = registry.get<physics::angle>(entity);
  auto direction_setpoint = atan(translation);
  return sin(direction_setpoint - direction);
}

Setpoint go_straight(entt::entity, entt::registry&) {
  return {
    .speed = 10_q_cm_per_s,
    .angular_speed = 0_q_rad_per_s
  };
}

Setpoint collide_with_candidate(entt::entity entity, entt::registry& registry) {
  auto candidate = registry.ctx<entt::entity>();
  auto translation = registry.get<Position>(candidate) - registry.get<Position>(entity);
  auto delta = get_delta(entity, registry, translation);
  return {
    .speed = 5_q_m_per_s / (1 + 10 * std::abs(delta)),
    .angular_speed = std::signbit(delta) ?
      -2_q_rad_per_s
    : +2_q_rad_per_s
  };
}

Setpoint collide_with_anyone(entt::entity entity, entt::registry& registry) {
  auto candidate = registry.ctx<entt::entity>();
  auto position = registry.get<Position>(candidate);
  auto translations = registry.group<Position>(entt::get_t<Task>()).each()
                    | ltl::filter([entity](const auto& tuple) { return std::get<0>(tuple) != entity; })
                    | ltl::map([position](const auto& tuple) { return std::get<1>(tuple) - position; });
  auto compare_vector = [](const auto& lhs, const auto& rhs) { return norm2(lhs) < norm2(rhs); };
  auto translation = *ltl::min_element(translations, compare_vector);
  auto delta = get_delta(entity, registry, translation);
  return {
    .speed = 5_q_m_per_s / (1 + 10 * std::abs(delta)),
    .angular_speed = std::signbit(delta) ?
      -2_q_rad_per_s
    : +2_q_rad_per_s
  };
}

Setpoint seek_goal(entt::entity entity, entt::registry& registry) {
  auto task = registry.get<Task>(entity);
  auto position = registry.get<Position>(entity);
  auto goal = registry.get<Position>(task.get_entity());
  auto delta = get_delta(entity, registry, goal - position);
  return {
    .speed = 5_q_m_per_s / (1 + 10 * std::abs(delta)),
    .angular_speed = std::signbit(delta) ?
      -2_q_rad_per_s
    : +2_q_rad_per_s
  };
}

Setpoint flee_candidate(entt::entity entity, entt::registry& registry) {
  auto setpoint = collide_with_candidate(entity, registry);
  return {
    .speed = -setpoint.speed,
    .angular_speed = setpoint.angular_speed
  };
}

Setpoint dont_move(entt::entity, entt::registry&) {
  return { .speed = 0_q_m_per_s, .angular_speed = 0_q_rad_per_s };
}

Setpoint run_circles(entt::entity entity, entt::registry& reigstry) {
  return { .speed = 10_q_m_per_s, .angular_speed = 0.5_q_rad_per_s };
}

/*
  Caractèristiques des robots utilisés dans les test unitaires
*/

RobotFeatures basic_features {
  .hitbox {10.0_q_cm},
  .strategy_ftor = go_straight,
  .shape = sf::CircleShape(physics::cast_for_display(10_q_cm)),
  .goal_mark_shape = sf::CircleShape(5)
};

RobotFeatures shark_features {
  .hitbox {10.0_q_cm},
  .strategy_ftor = collide_with_candidate,
  .shape = sf::CircleShape(physics::cast_for_display(10_q_cm)),
  .goal_mark_shape = sf::CircleShape(5)
};

RobotFeatures collider_features {
  .hitbox {10.0_q_cm},
  .strategy_ftor = collide_with_anyone,
  .shape = sf::CircleShape(physics::cast_for_display(10_q_cm)),
  .goal_mark_shape = sf::CircleShape(5)
};

RobotFeatures sleeper_features {
  .hitbox {10.0_q_cm},
  .strategy_ftor = dont_move,
  .shape = sf::CircleShape(physics::cast_for_display(10_q_cm)),
  .goal_mark_shape = sf::CircleShape(5)
};

RobotFeatures worker_features {
  .hitbox {10.0_q_cm},
  .strategy_ftor = seek_goal,
  .shape = sf::CircleShape(physics::cast_for_display(10_q_cm)),
  .goal_mark_shape = sf::CircleShape(5)
};

RobotFeatures coward_features {
  .hitbox {10.0_q_cm},
  .strategy_ftor = flee_candidate,
  .shape = sf::CircleShape(physics::cast_for_display(10_q_cm)),
  .goal_mark_shape = sf::CircleShape(5)
};

RobotFeatures racers_features {
  .hitbox {10.0_q_cm},
  .strategy_ftor = run_circles,
  .shape = sf::CircleShape(physics::cast_for_display(10_q_cm)),
  .goal_mark_shape = sf::CircleShape(5)
};

std::wstring answer, yes(L"o"), no(L"n");

TEST_CASE("perform_trial : mener à terme une épreuve en donnant un retour visuel", "[visual]") {
  TrialParameters trial_parameters {
    .playground {
      .left = 0_q_m,
      .right = 0_q_m,
      .width = 5_q_m,
      .height = 4_q_m },
    .foe_nb = 2,
    .seed = 0,
    .dt = 1_q_ms,
    .time_limit = 10_q_s
  };

  SECTION("Un appel à 'perform_trial' affiche une fenêtre représentant les robots "
          "sur la table de jeu en temps réel, distinguant le candidat des adversaires "
          "avec un code visuel bien spécifique") {
    trial_parameters.seed = generate_seed();
    perform_trial(trial_parameters, basic_features, basic_features, true);

    std::wcout << "Est-ce que que 3 gros cercles sont apparus ainsi que 3 petits "
                  "cercle ? (o/n)" << std::endl;
    std::wcin >> answer;
    REQUIRE(answer == yes);

    std::wcout << "Est-ce que les gros cercles suivent des trajectoires rectilignes "
                  "à la même vitesse dans la même direction ? (o/n)" << std::endl;
    std::wcin >> answer;
    REQUIRE(answer == yes);
  }
}

TEST_CASE("perform_trial : garantir que les contraintes physiques sont respectées", "[physics]") {
  TrialParameters trial_parameters {
    .playground {
      .left = 0_q_m,
      .right = 0_q_m,
      .width = 5_q_m,
      .height = 4_q_m },
    .foe_nb = 0,
    .seed = 0,
    .dt = 1_q_ms,
    .time_limit = 10_q_s
  };

  SECTION("Les entités ne peuvent pas franchir la zone de jeu délimité par un "
          "rectangle blanc") {
    trial_parameters.seed = generate_seed();
    perform_trial(trial_parameters, racers_features, racers_features, true);

    std::wcout << "Est-ce que les robots ont franchis les limites de la zone de jeu ?" << std::endl;
    std::wcin >> answer;
    REQUIRE(answer == no);
  }
}

TEST_CASE("perform_trial : réccupérer l'issue de la simulation", "[outcome]") {
  TrialParameters trial_parameters {
    .playground {
      .left = 0_q_m,
      .right = 0_q_m,
      .width = 5_q_m,
      .height = 4_q_m },
    .foe_nb = 2,
    .seed = 0,
    .dt = 1_q_ms,
    .time_limit = 10_q_s
  };

  SECTION("'perform_trial' indique si l'adversaire a heurté le candidat") {
    trial_parameters.seed = generate_seed();
    auto outcome = perform_trial(trial_parameters, sleeper_features, shark_features, true).outcome;
    REQUIRE(outcome == Outcome::foe_collided);
  }

  SECTION("'perform_trial' indique si le candidat a heurté l'adversaire") {
    trial_parameters.seed = generate_seed();
    auto outcome = perform_trial(trial_parameters, collider_features, sleeper_features, true).outcome;
    REQUIRE(outcome == Outcome::candidate_collided);
  }

  SECTION("'perform_trial' indique si le candidat a atteint son objectif") {
    trial_parameters.seed = generate_seed();
    auto outcome = perform_trial(trial_parameters, worker_features, coward_features, true).outcome;
    REQUIRE(outcome == Outcome::goal_reached);
  }
}
