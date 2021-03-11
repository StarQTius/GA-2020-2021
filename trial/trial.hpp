#include <concepts>
#include <random>
#include <tuple>

#include <entt/entt.hpp>

#include "ltl/Range/Map.h"

#include <SFML/Graphics.hpp>

#include <units/random.h>

#include "../seed.hpp"
#include "component/hitbox.hpp"
#include "entity/robot_features.hpp"
#include "io/event.hpp"
#include "io/render.hpp"
#include "mechanic.hpp"
#include "outcome.hpp"
#include "physics.hpp"
#include "trial_results.hpp"

TrialResults perform_trial(const TrialParameters& trial_parameters,
                           const RobotFeatures<auto, auto, auto>& candidate_features,
                           const RobotFeatures<auto, auto, auto>& foe_features,
                           bool shall_display = false) {
  entt::registry registry;
  registry.set<TrialParameters>(trial_parameters);
  registry.set<Outcome>(Outcome::none);
  registry.set<std::mt19937>(trial_parameters.seed);

  // Créer le candidat et les adversaires en tant qu'entité. La variable de contexte
  // associée au type 'entt::entity' dans le registre vaut l'identifiant du candidat.
  auto candidate = create_robot(registry, candidate_features, shall_display);
  for (auto i = 0; i < trial_parameters.foe_nb; i++)
    create_robot(registry, foe_features, shall_display);
  registry.set<entt::entity>(candidate);

  // Créer un fenêtre de rendu et le sprite de la zone de jeu si nécessaire.
  sf::RenderWindow render_window;
  if (shall_display) {
    auto center = create_playground_sprite(registry);
    render_window.create(sf::VideoMode::getDesktopMode(), "Unit test");

    auto view = render_window.getView();
    view.setCenter(center);
    render_window.setView(view);

  } else {
    create_playground_sprite(registry);
  }

  physics::time   simulation_time = 0_q_ms;
  physics::length best_distance = get_distance_to_goal(candidate, registry);
  sf::Clock       clock;
  while (registry.ctx<Outcome>() == Outcome::none
     && (!shall_display || render_window.isOpen())
     && (simulation_time < trial_parameters.time_limit)) {
    set_setpoints(registry);
    update_positions(registry);
    position_sprites(registry);
    rotate_sprites(registry);
    update_tasks(registry);
    detect_collisions(registry);

    if (shall_display) {
      display(render_window, registry);
      handle_events(render_window);
    }

    auto distance = get_distance_to_goal(candidate, registry);
    if (distance < best_distance) best_distance = distance;

    simulation_time += trial_parameters.dt;
    while (shall_display && clock.getElapsedTime().asMilliseconds() < quantity_cast<si::millisecond>(trial_parameters.dt).count());
    clock.restart();
  }

  return {
    .outcome = registry.ctx<Outcome>(),
    .time = simulation_time,
    .best_distance = best_distance
  };
}
