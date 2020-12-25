#ifndef GATE_PLAYGROUND_CANDIDATE_HPP
#define GATE_PLAYGROUND_CANDIDATE_HPP

#include <memory>
#include <ranges>
#include <stdexcept>

#include <opennn/Tensor.h>
#include <opennn/NeuralNetwork.h>

#include <SFML/Graphics.hpp>

#include <units/math.h>
#include <units/physical/si/derived/speed.h>
#include <units/physical/si/derived/angular_velocity.h>
#include <units/generic/angle.h>
#include <units/physical/si/base/length.h>

#include "onscreen_coordinate.hpp"
#include "physics.hpp"
#include "robot.hpp"

using units::abs;

/*******************************************************************************
  gate::Candidate : représente le robot de l'équipe INTech. La classe implémente
  l'interface de gate::Entity.
*******************************************************************************/

namespace gate {

class Candidate{
public:
  /*
    Identité esthétique
  */
  static sf::Color                           fill_color;
  static onscreen_coordinate<pixel, numeric> goal_mark_radius;
  static sf::Color                           goal_mark_fill_color;

  /**
  **/
  Candidate(sf::Vector2<Length auto> position, Radian auto direction, Length auto radius,
            sf::Vector2<Length auto> goal,
            const std::shared_ptr<openNN::NeuralNetwork>& neural_network_ptr)
      : neural_network_shptr(neural_network),
        hitbox(position, radius),
        direction(direction),
        is_moving(false),
        goal(goal),
        sprite(display_scale * radius),
        goal_mark(goal_mark_radius),
        neural_network_ptr(neural_network_ptr) {
    sprite.setFillColor(fill_color);
    goal_mark.setFillColor(goal_mark_fill_color);
  }

  /**

  **/
  auto update(const sf::Rect<Length auto>& space, const RobotRange& foes) {
    for (const auto& foe : foes) {
      if (hitbox.is_intersecting(foe.shape()))
        return is_moving ?
          Outcome::candidate_collided
        : Outcome::foe_collided;
    }

    auto output = neural_network_ptr->calculate_outputs(make_network_input(foes));
    update_position(space, output);
    update_sprites();

    return hitbox.contains(goal) ?
      Outcome::goal_reached
    : Outcome::none;
  }

  /**

  **/
  auto shape() const { return hitbox; }

  /**

  **/
  auto tag() const { return Kind::candidate; }

  /**

  **/
  void draw(sf::RenderTarget& rd_target, sf::RenderStates rd_states) const {
    render_target.draw(sprite);
    render_target.draw(goal_mark);
  }

  /**

  **/
  auto make_network_input(const RobotRange& foes) {
    if (neural_network_ptr->get_inputs_number() != 5 + 2 * std::range::distance(foes))
      throw std::out_of_bound("openNN::Tensor<>");

    openNN::Tensor<double> input(neural_network_ptr->get_inputs_number(), 1);

    /*Remplir avec les données du candidat*/
    input(0) = hitbox.position.x / space.width;
    input(1) = hitbox.position.y / space.height;
    input(2) = goal.x / space.width;
    input(3) = goal.y / space.height;
    input(4) = cos(direction);
    input(5) = sin(direction);

    /*Remplir avec les données des ennemis*/
    size_t i = 5;
    for (const auto& foe : foes) {
      input(++i) = foe.shape().pos_m.x / space.width;
      input(++i) = foe.shape().pos_m.y / space.height;
    }

    return input;
  }

  /**

  **/
  void update_position(const sf::Rect<Length auto>& space,
                       openNN::Tensor<double> output) {
    is_moving = is_moving_threshold < output(0);
    if (is_moving) {
      hitbox.position.x += speed * dt * cos(direction);
      hitbox.position.y += speed * dt * sin(direction);
      constrain_to(hitbox, space);
    } else {
      sf::Vector2<auto> subgoal(output(1), output(2));
      sf::Vector2<auto> translation(subgoal - hitbox.position);
      auto setpoint = atan(translation);
      auto delta = direction - setpoint;

      /*Faire pivoter de l'angle espilon; Si la différence avec la consigne est
        plus petite qu'epsilon, alors l'angle réel prend la valeur de la
        consigne*/
      auto epsilon = angular_speed * dt;
      if (abs(delta) > epsilon) {
        direction += (signbit(delta) ? -1 : 1) * epsilon;
      } else {
        direction = setpoint;
      }
    }
  }

  /**

  **/
  void update_sprites() {
    /*Les coordonnées d'un cercle en SFML étant celle du coin supérieur gauche
      de l'image, on doit soustraire l'écart de la position réelle avant
      d'afficher*/
    auto sprite_position = hitbox.position
                         - hitbox.radius * (ux + uy);
    sprite.setPosition(display_scale * sprite_position);

    /*Idem...*/
    auto goal_onscreen_position = display_scale * goal
                                - goal_mark_radius * (ux + uy);
    goal_mark.setPosition(goal_position);
  }

private:
  Hitbox                                               hitbox;
  sf::Vector2<si::length<si::meter, numeric>>          goal;
  si::speed<si::metre_per_second, numeric>             speed;
  si::angle<si::radian, numeric>                       direction;
  si::angular_velocity<si::radian_per_second, numeric> angular_speed;

  std::shared_ptr<const NeuralNetwork> neural_network_shptr;
  bool                                 is_moving;

  sf::CircleShape sprite;
  sf::CircleShape goal_mark;

};

} //namespace gate

#endif
