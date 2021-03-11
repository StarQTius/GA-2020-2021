#pragma once

#include <entt/entt.hpp>

#include "component.hpp"
#include "math.hpp"
#include "outcome.hpp"
#include "physics.hpp"

//TODO : remove
#include <iostream>

/*******************************************************************************

*******************************************************************************/

//
auto create_playground_sprite(entt::registry& registry) {
  auto entity = registry.create();
  const auto& playground = registry.ctx<TrialParameters>().playground;
  Position position{playground.left, playground.top};
  Position size{playground.width, playground.height};

  sf::RectangleShape playground_sprite(physics::cast_for_display(size));
  playground_sprite.setPosition(physics::cast_for_display(position));
  playground_sprite.setFillColor({0, 0, 0, 0});
  playground_sprite.setOutlineColor(sf::Color::White);
  playground_sprite.setOutlineThickness(5);
  registry.emplace<ShapePtr>(entity, new sf::RectangleShape(playground_sprite));

  return physics::cast_for_display(position) + physics::cast_for_display(size) / 2.f;
}

//
void set_setpoints(entt::registry& registry) {
  auto robots = registry.view<Strategy, physics::speed, physics::angular_speed>();
  for (auto&& [entity, strategy, speed, angular_speed] : robots.each()) {
    auto setpoint = strategy(entity, registry);
    speed = setpoint.speed;
    angular_speed = setpoint.angular_speed;
  }
}

//
void update_positions(entt::registry& registry) {
  auto bodies = registry.view<Position, Hitbox, physics::angle, physics::speed, physics::angular_speed>();
  const auto& playground = registry.ctx<TrialParameters>().playground;
  auto dt = registry.ctx<TrialParameters>().dt;
  for (auto&& [entity, position, hitbox, direction, speed, angular_speed] : bodies.each()) {
    position.x += speed * dt * cos(direction);
    position.x = std::max(playground.left + hitbox.radius, position.x);
    position.x = std::min(playground.left + playground.width - hitbox.radius, position.x);

    position.y += speed * dt * sin(direction);
    position.y = std::max(playground.top + hitbox.radius, position.y);
    position.y = std::min(playground.top + playground.height - hitbox.radius, position.y);

    direction = fmod(direction + angular_speed * dt, 2 * physics::pi);
  }
}

//
void position_sprites(entt::registry& registry) {
  auto positionables = registry.view<Position, ShapePtr>();
  for (auto&& [entity, position, shape_ptr] : positionables.each()) {
    const auto& local_bounds = shape_ptr->getLocalBounds();
    sf::Vector2f recentring_translation(-local_bounds.width / 2, -local_bounds.height / 2);
    shape_ptr->setOrigin(-physics::cast_for_display(position));
    shape_ptr->setPosition(recentring_translation);
  }
}

#pragma message "Réactiver 'rotate_sprites'"
//
void rotate_sprites(entt::registry& registry) {
  /*auto rotatables = registry.view<physics::angle, ShapePtr>();
  for (auto&& [entity, direction, shape_ptr] : rotatables.each())
    shape_ptr->setRotation(physics::cast_for_display(direction));*/
}

//
void update_tasks(entt::registry& registry) {
  auto workers = registry.view<Position, Task>();
  auto candidate = registry.ctx<entt::entity>();
  for (auto&& [entity, position, task] : workers.each())
    if (task.update(entity, registry) && entity == candidate)
      registry.set<Outcome>(Outcome::goal_reached);
}

//
void detect_collisions(entt::registry& registry) {
  auto   collidables = registry.view<Position, Hitbox>();
  auto   candidate = registry.ctx<entt::entity>();
  auto&& [candidate_position, candidate_speed, candidate_hitbox] =
    registry.get<Position, physics::speed, Hitbox>(candidate);
  auto& outcome = registry.ctx<Outcome>();

  for (auto&& [entity, position, hitbox] : collidables.each()) {
    if (entity != candidate
    && hitbox.is_intersecting(candidate_hitbox, position - candidate_position)
    && outcome != Outcome::candidate_collided) {
      outcome = (candidate_speed != 0_q_m_per_s) ?
        Outcome::candidate_collided
      : Outcome::foe_collided;
    }
  }
}

//
physics::length get_distance_to_goal(entt::entity entity, entt::registry& registry) {
  auto task_entity = registry.get<Task>(entity).get_entity();
  return norm2(registry.get<Position>(entity) - registry.get<Position>(task_entity));
}
