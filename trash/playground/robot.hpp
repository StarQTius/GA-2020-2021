#ifndef GATE_PLAYGROUND_ROBOT_HPP
#define GATE_PLAYGROUND_ROBOT_HPP

#include <ranges>

#include "physics.hpp"

/*******************************************************************************
  gate::Robot : instanciation de gate::Entity<> qui convient au besoin de la
  simulation.
*******************************************************************************/

namespace gate {

typedef Robot : Entity<si::length<si::meter>, Hitbox, Outcome, Kind>;

template<typename T>
concept RobotRange =
     requires(T t) { *ltl::begin(t); } -> std::is_convertible<Robot>
  && std::ranges::range<T>;


}

#endif
