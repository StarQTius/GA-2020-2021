#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

void handle_events(sf::Window& window) {
  sf::Event event;
  while (window.pollEvent(event)) {
    if (event.type == sf::Event::Closed) window.close();
  }
}
