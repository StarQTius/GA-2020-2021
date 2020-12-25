#pragma once

#include <memory>
#include <functional>

#include <entt/entt.hpp>

#include <SFML/Graphics.hpp>

#include "../physics.hpp"

/*******************************************************************************
*******************************************************************************/

using Playground = sf::Rect<physics::length>;
using Position = sf::Vector2<physics::length>;

struct Setpoint { physics::speed speed; physics::angular_speed angular_speed; };
using StrategyFunction = Setpoint(entt::entity, entt::registry&);
using Strategy = std::function<StrategyFunction>;

using ShapePtr = std::unique_ptr<sf::Shape>;
