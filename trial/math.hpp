#pragma once

#include <SFML/Graphics.hpp>

#include <units/generic/angle.h>
#include <units/math.h>
#include <units/quantity.h>
#include <units/quantity_cast.h>

/*******************************************************************************
*******************************************************************************/

using namespace units;

auto norm2(const sf::Vector2<Quantity auto>& vector) {
  return sqrt(pow<2>(vector.x) + pow<2>(vector.y));
}

auto atan(const sf::Vector2<Quantity auto>& vector) {
  return angle<radian>(std::atan2(vector.y.count(), vector.x.count()));
}

auto cos(Angle auto theta) {
  return std::cos(quantity_cast<radian>(theta).count());
}

auto sin(Angle auto theta) {
  return std::sin(quantity_cast<radian>(theta).count());
}

bool signbit(Quantity auto quantity) {
  return std::signbit(quantity.count());
}

template<Quantity Q>
auto fmod(Q lhs, Quantity auto rhs) {
  return Q(std::fmod(lhs.count(), quantity_cast<Q>(rhs).count()));
}

template<Quantity Q>
auto min(Q lhs, Q rhs) {
  return lhs < rhs ? lhs : rhs;
}

template<Quantity Q>
auto max(Q lhs, Q rhs) {
  return lhs > rhs ? lhs : rhs;
}
