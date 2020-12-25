#ifndef GATE_PLAYGROUND_ONSCREEN_SCALE_HPP
#define GATE_PLAYGROUND_ONSCREEN_SCALE_HPP

#include <units/derived_dimension.h>
#include <units/exponent.h>
#include <units/physical/si/base/length.h>
#include <units/quantity.h>
#include <units/unit.h>

#include "onscreen_coordinate.hpp"

/*******************************************************************************
  onscreen_scale.hpp : définit une dimension pour les échelles coordonnées sur
  l'écran / longueur et les littéraux associés. Cette dimension permet de fixer
  une relation entre les longueurs physiques simulées (ie, les distances, la
  taille des robots) et leurs représentation sur l'écran. Par exemple, on peut
  décider de représenter la simualtion avec une échelle de 100 px/m : dans ce
  cas, un robot de 30cm de diamètre sera représenté sur l'écran par un disque
  d'un diamètre de 30px.
*******************************************************************************/

using namespace units;
using namespace units::physical;

struct pixel_per_meter : unit<pixel_per_meter> {};
struct dim_onscreen_scale : derived_dimension<
  dim_onscreen_scale, pixel_per_meter, exponent<dim_onscreen_coordinate, 1>,
  exponent<si::dim_length, -1>> {};

template<typename T>
concept OnscreenScale = QuantityOf<T, dim_onscreen_scale>;

template<UnitOf<dim_onscreen_scale> U, QuantityValue Rep = double>
using onscreen_scale = quantity<dim_onscreen_scale, U, Rep>;

constexpr auto operator "" _q_px_per_m(unsigned long long l) {
  return onscreen_scale<pixel_per_meter, int64_t>(l);
};
constexpr auto operator "" _q_px_per_m(long double l) {
  return onscreen_scale<pixel_per_meter, long double>(l);
}

#endif
