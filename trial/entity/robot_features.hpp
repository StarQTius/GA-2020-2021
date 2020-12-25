#pragma once

#include <concepts>

#include <boost/callable_traits/return_type.hpp>

#include <entt/entt.hpp>

#include <SFML/Graphics.hpp>

#include <units/random.h>

#include "../component.hpp"
#include "../physics.hpp"
#include "../position_picker.hpp"
#include "../trial_parameters.hpp"

template<
  std::invocable<entt::entity, entt::registry&> StrategyFtor,
  std::derived_from<sf::Shape> Shape,
  std::derived_from<sf::Shape> GoalMarkShape
> requires std::same_as<boost::callable_traits::return_type_t<StrategyFtor>, Setpoint>
struct RobotFeatures {
  Hitbox       hitbox;
  StrategyFtor strategy_ftor;

  Shape         shape;
  GoalMarkShape goal_mark_shape;
};

//
entt::entity create_robot(entt::registry& registry,
                          const RobotFeatures<auto, auto, auto>& robot_features,
                          bool shall_display) {
  using units::uniform_real_distribution;

  const auto& playground = registry.ctx<TrialParameters>().playground;
        auto& rnd_engine = registry.ctx<std::mt19937>();
  const auto& [hitbox, strategy_ftor, shape, goal_mark_shape] = robot_features;
  PositionPicker position_picker(playground);
  uniform_real_distribution<physics::angle> angle_picker(0_q_rad, 2 * physics::pi);

  // Créer les plus simples composants du robot
  auto entity = registry.create();
  registry.emplace<Position>(entity, 0_q_m, 0_q_m);
  registry.emplace<physics::speed>(entity, 0);
  registry.emplace<physics::angular_speed>(entity, 0);
  registry.emplace<Hitbox>(entity, hitbox);
  registry.emplace<Strategy>(entity, strategy_ftor);

  // Créer une tâche pour le robot
  auto task_entity = registry.create();
  registry.emplace<Position>(task_entity, 3_q_m, 3_q_m );
  registry.emplace<Task>(entity, task_entity, registry);

  // Créer des composants graphiques si besoin
  if (shall_display) {
    registry.emplace<ShapePtr>(task_entity, new auto(goal_mark_shape));
    registry.emplace<ShapePtr>(entity, new auto(shape));
  }

  registry.emplace<physics::angle>(entity, atan(registry.get<Position>(task_entity) - registry.get<Position>(entity)));
  normal_distribution make_noise(0.0_q_m, 1.0_q_m);
  registry.get<Position>(task_entity) += { .x = make_noise(rnd_engine), .y = make_noise(rnd_engine) };

  return entity;
}
