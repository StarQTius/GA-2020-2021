#pragma once

#include "outcome.hpp"
#include "physics.hpp"

struct TrialResults {
  Outcome         outcome;
  physics::time   time;
  physics::length best_distance;
};
