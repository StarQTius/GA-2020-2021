#pragma once

#include <concepts>
#include <ranges>

#include <SFML/Graphics.hpp>

// TODO : trouver une solution

#include "../component.hpp"

/*template<typename T>
concept Drawables =
   std::ranges::range<T>
&& requires(T t) { { *t.begin() } -> std::derived_from<sf::Drawable>; };*/

//
void display(sf::RenderWindow& render_window, entt::registry& registry) {
  render_window.clear();
  auto drawables = registry.view<ShapePtr>().each();
  for (auto&& [entity, shape_ptr] : drawables) render_window.draw(*shape_ptr);
  // TODO : remove
  auto entity  = registry.ctx<entt::entity>();
  auto task_entity = registry.get<Task>(entity).get_entity();
  auto speed = registry.get<physics::speed>(entity);
  auto angular_speed = registry.get<physics::angular_speed>(entity);
  auto distance = norm2(registry.get<Position>(entity) - registry.get<Position>(task_entity));
  sf::Text speed_text;
  sf::Text angular_speed_text;
  sf::Text distance_text;
  sf::Font font;
  font.loadFromFile("../font.ttf");
  speed_text.setFont(font);
  angular_speed_text.setFont(font);
  distance_text.setFont(font);
  speed_text.setString(std::string("Vitesse : ") + std::to_string(speed.count()));
  angular_speed_text.setString(std::string("Vitesse angulaire : ") + std::to_string(angular_speed.count()));
  distance_text.setString(std::string("Distance : ") + std::to_string(distance.count()));
  speed_text.move(-250, -100);
  angular_speed_text.move(-250, -50);
  distance_text.move(-250, 0);
  render_window.draw(speed_text);
  render_window.draw(angular_speed_text);
  render_window.draw(distance_text);

  render_window.display();
}
