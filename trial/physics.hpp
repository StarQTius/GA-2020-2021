#pragma once

#include <concepts>

#define _USE_MATH_DEFINES
#include <cmath>

#include <SFML/Graphics.hpp>

#include <units/generic/angle.h>
#include <units/physical/si/cgs/base/length.h>
#include <units/physical/si/cgs/base/time.h>
#include <units/physical/si/derived/speed.h>
#include <units/physical/si/cgs/derived/speed.h>
#include <units/physical/si/derived/angular_velocity.h>
#include <units/physical/si/cgs/derived/speed.h>
#include <units/quantity.h>
#include <units/quantity_cast.h>

#include "physics/onscreen_scale.hpp"

/*******************************************************************************
  La simulation se base sur un modèle physique. Il est donc nécessaire de
  définir des dimensions, grandeurs et fonctions qui permettent de mettre en
  oeuvre ce modèle.
*******************************************************************************/

//Doit être en dehors du namespace pour les UDLs
using namespace units::physical::si::literals;
using namespace units::physical::si::cgs::literals;

namespace physics {

using namespace units;
using namespace units::physical;

typedef float numeric; //Précision standard pour les calculs

/*
  Type standard pour les dimensions
*/

typedef si::length<si::metre, numeric>                       length;
typedef si::time<si::second, numeric>                        time;
typedef si::speed<si::metre_per_second, numeric>             speed;
typedef angle<radian, numeric>                               angle;
typedef si::angular_velocity<si::radian_per_second, numeric> angular_speed;
typedef onscreen_coordinate<pixel, numeric>                  onscreen_coordinate;

/*
  Grandeurs physiques
*/

constexpr auto pi = M_PI * 1.0_q_rad;
constexpr auto display_scale = 100.0_q_px_per_m;

/*
  Fonctions de conversion pour l'affichage
*/

constexpr auto cast_for_display(Length auto distance) {
  return quantity_cast<onscreen_coordinate>(display_scale * distance).count();
}

constexpr auto cast_for_display(Angle auto theta) {
  return quantity_cast<angle>(theta).count();
}

auto cast_for_display(const sf::Vector2<Quantity auto>& vector) {
  return sf::Vector2 {
    .x = cast_for_display(vector.x),
    .y = cast_for_display(vector.y)
  };
}

} // namespace physics
