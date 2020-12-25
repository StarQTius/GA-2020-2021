#ifndef GATE_PLAYGROUND_ANGULAR_SPEED_HPP
#define GATE_PLAYGROUND_ANGULAR_SPEED_HPP

#include <units/derived_dimension.h>
#include <units/exponent.h>
#include <units/generic/angle.h>
#include <units/physical/si/base/time.h>
#include <units/quantity.h>
#include <units/unit.h>

/*******************************************************************************
  angular_speed.hpp : définit une dimension pour la vitesse angulaire et les
  littéraux associés.
*******************************************************************************/

using namespace units;
using namespace units::physical;

namespace gate {

struct radian_per_second : unit<radian_per_second> {};
struct dim_angular_speed : derived_dimension<dim_angular_speed,
                                             radian_per_second,
                                             exponent<dim_angle<>, 1>,
                                             exponent<si::dim_time, -1>> {};

template<typename T>
concept AngularSpeed = QuantityOf<T, dim_angular_speed>;

template<UnitOf<dim_angular_speed> U, QuantityValue Rep = double>
using angular_speed = quantity<dim_angular_speed, U, Rep>;

constexpr auto operator"" _q_rad_per_s(unsigned long long l) {
  return angular_speed<radian_per_second, int64_t>(l);
}
constexpr auto operator"" _q_rad_per_s(long double l) {
  return angular_speed<radian_per_second, long double>(l);
}

}

#endif
