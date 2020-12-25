#ifndef GATE_PLAYGROUND_ONSCREEN_COORDINATE_HPP
#define GATE_PLAYGROUND_ONSCREEN_COORDINATE_HPP

#include <units/base_dimension.h>
#include <units/quantity.h>
#include <units/unit.h>

/*******************************************************************************
  onscreen_coordinate.hpp : définit une dimension pour les coordonnées sur
  l'écran et les littéraux associés. Les coordonnées sont utilisées pour faire
  afficher les sprites par SFML. L'emploi d'une dimension à part est justifier
  par la nécessité de séparer la représentation physique des objets (en unité de
  longueur) et de leur représentation sur l'écran. Ainsi, on ne peut tout
  simplement pas indiquer les coordonnées sur l'écran en unité de longueur (ça
  n'a pas de sens !) : il faut les exprimer avec une autre dimension. L'unité de
  base définie dans ce fichier pour les coordonnées est le pixel (px).
*******************************************************************************/

using namespace units;

struct pixel : named_unit<pixel, "pixel", no_prefix> {};
struct dim_onscreen_coordinate : base_dimension<"onscreen coordinate", pixel> {};

template<typename T>
concept OnscreenCoordinate = QuantityOf<T, dim_onscreen_coordinate>;

template<UnitOf<dim_onscreen_coordinate> U, QuantityValue Rep = double>
using onscreen_coordinate = quantity<dim_onscreen_coordinate, U, Rep>;

constexpr auto operator"" _q_px(unsigned long long l) {
  return onscreen_coordinate<pixel, int64_t>(l);
}
constexpr auto operator"" _q_px(long double l) {
  return onscreen_coordinate<pixel, long double>(l);
}

#endif
