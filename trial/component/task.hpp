#pragma once

#include <concepts>
#include <random>

#include <entt/entt.hpp>

#include "../math.hpp"
#include "../physics.hpp"
#include "../position_picker.hpp"
#include "../trial_parameters.hpp"
#include "component.hpp"
#include "hitbox.hpp"

/*******************************************************************************
*******************************************************************************/

class Task {
public:
  Task(entt::entity entity, entt::registry& registry) : entity(entity) {}

  //
  bool update(entt::entity worker, entt::registry& registry) {
    const auto& task_position = registry.get<Position>(entity);
    auto&&      [position, speed] = registry.get<Position, physics::speed>(worker);
    auto        dt = registry.ctx<TrialParameters>().dt;

    if (norm2(position - task_position) < 5_q_cm) { //TODO : change
      const auto& hitbox = registry.get<Hitbox>(worker);
      const auto& playground = registry.ctx<TrialParameters>().playground;
      auto&       rnd_engine = registry.ctx<std::mt19937>();

      PositionPicker position_picker(playground);
      registry.replace<Position>(entity, position_picker(rnd_engine));

      return true;
    }

    return false;
  }

  //
  auto get_entity() const { return entity; }

private:
  entt::entity entity;
  std::mt19937 rnd_engine;

};
