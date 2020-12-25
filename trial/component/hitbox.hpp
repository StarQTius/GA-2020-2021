#pragma once

#include <SFML/Graphics.hpp>

#include <units/physical/si/base/length.h>

#include "../math.hpp"
#include "../physics.hpp"

/*******************************************************************************
  Hitbox représente la forme physique des robots. Comme le nom l'indique,
  la structure permet de gérer les collisions entre les robots. Chaque Hitbox
  est un disque centré sur un robot, qui représente le périmètre que les autres
  robots ne doivent pas franchir.
*******************************************************************************/

struct Hitbox {
  //
  bool is_in(const sf::Rect<Length auto>& rect,
             const sf::Vector2<Length auto>& position) const {
    return rect.left + radius <= position.x
        && rect.top + radius <= position.y
        && position.x <= rect.left + rect.width - radius
        && position.y <= rect.top + rect.height - radius;
  }

  //
  bool contains(const sf::Vector2<Length auto>& relative_position) const {
    return norm2(relative_position) < radius;
  }

  //
  bool is_intersecting(const Hitbox& hitbox,
                       const sf::Vector2<Length auto>& relative_position) const {
    return norm2(relative_position) < radius + hitbox.radius;
  }

  physics::length radius;

};
