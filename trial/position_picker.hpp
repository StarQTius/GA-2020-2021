#pragma once

#include <units/random.h>

#include "component.hpp"
#include "physics.hpp"

struct PositionPicker {
  PositionPicker(const Playground& playground)
    : pick_x(playground.left, playground.left + playground.width),
      pick_y(playground.top, playground.top + playground.height)
  {}

  template<typename T>
  auto operator()(T&& rnd_engine) {
    return Position(pick_x(rnd_engine), pick_y(rnd_engine));
  }

  uniform_real_distribution<physics::length> pick_x;
  uniform_real_distribution<physics::length> pick_y;
};
