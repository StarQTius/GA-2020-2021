#pragma once

#include <concepts>
#include <random>

#include "component.hpp"
#include "physics.hpp"

/*******************************************************************************
*******************************************************************************/

struct TrialParameters {
  Playground    playground;
  size_t        foe_nb;
  int64_t       seed;
  physics::time dt;
  physics::time time_limit;
};
