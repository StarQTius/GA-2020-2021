#include <iostream>
#include <fstream>
#include <random>
#include <vector>

#include "ltl/algos.h"
#include "ltl/algos.h"

#include "../genetics/genetics.hpp"
#include "metrics/measure_accuracy.hpp"
#include "strategy/neural_engine.hpp"
#include "math.hpp"
#include "physics.hpp"

double sin_delta(entt::entity entity, entt::registry& registry) {
  auto task = registry.get<Task>(entity);
  auto goal = registry.get<Position>(task.get_entity());
  auto direction = registry.get<physics::angle>(entity);
  return sin(atan(registry.get<Position>(entity) - goal) - direction);
}

double cos_delta(entt::entity entity, entt::registry& registry) {
  auto task = registry.get<Task>(entity);
  auto goal = registry.get<Position>(task.get_entity());
  auto direction = registry.get<physics::angle>(entity);
  return cos(atan(registry.get<Position>(entity) - goal) - direction);
}

double speed(entt::entity entity, entt::registry& registry) {
  return registry.get<physics::speed>(entity).count();
}

double distance_to_goal(entt::entity entity, entt::registry& registry) {
  auto task = registry.get<Task>(entity);
  auto goal = registry.get<Position>(task.get_entity());
  return norm2(registry.get<Position>(entity) - goal).count();
}

double x_position(entt::entity entity, entt::registry& registry) {
  return registry.get<Position>(entity).x.count();
}

double y_position(entt::entity entity, entt::registry& registry) {
  return registry.get<Position>(entity).y.count();
}


double bias(entt::entity, entt::registry&) {
  return 1.0;
}

template<typename... Args>
auto make_batch(size_t nb, Args&&... args) {
  std::vector<NeuralEngine> batch;
  for (size_t i = 0; i < nb; i++)
    batch.emplace_back(std::forward<Args>(args)...);
  return batch;
}

int main() {
  using namespace genetics;
  using std::ranges::begin;

  //
  auto batch = make_batch(50, 15,
    sin_delta,
    cos_delta,
    //speed,
    distance_to_goal,
    //x_position,
    //y_position,
    bias);
  RobotFeatures neural_features {
    .hitbox {10_q_cm},
    .strategy_ftor = NeuralEngine(0),
    .shape = sf::CircleShape(physics::cast_for_display(10_q_cm)),
    .goal_mark_shape = sf::CircleShape(physics::cast_for_display(5_q_cm))
  };
  TrialParameters parameters {
    .playground {0_q_m, 0_q_m, 5_q_m, 5_q_m },
    .foe_nb = 0,
    .seed = 1337,
    .dt = 0.1_q_s,
    .time_limit = 10_q_s
  };

  //
  int64_t seeds[10];
  auto fitness_ftor = [&](auto& candidate) {
    auto features = neural_features;
    features.strategy_ftor = candidate;
    double fitness = 0;
    size_t nb_sucess = 0;

    for (size_t i = 0; i < 10; i++) {
      parameters.seed = seeds[i];
      auto results = perform_trial(parameters, features, features);
      fitness -= results.best_distance.count() + results.time.count();
      nb_sucess += results.outcome == Outcome::goal_reached;
    }

    if (nb_sucess < 10){
      std::wcout << nb_sucess << std::flush;
    } else {
      std::wcout << "@" << std::flush;
    }

    return fitness;
  };

  //
  std::mt19937             rnd_engine(generate_seed());
  std::normal_distribution make_noise(0.0, 1.0);
  int                      elitism = 15;

  auto best_features = neural_features;
  for (auto& candidate : batch) {
    mutate(candidate.view(), 1.0, [&](auto& value, auto& rnd_engine) { value = make_noise(rnd_engine); });
  }

  for (int i = 0; i < 100; i++) {
    ltl::for_each(seeds, [](auto& x) { x = generate_seed(); });

    std::wcout << std::wstring(elitism, 'v') << std::endl;
    auto fitnesses = sort_candidates(batch, fitness_ftor);
    auto worst_fitness = *ltl::min_element(fitnesses);
    auto positive_fitnesses = fitnesses | ltl::take_n(elitism)
                                        | ltl::map([&](const auto& x) { return x - worst_fitness; });
    std::discrete_distribution pick_index(positive_fitnesses);

    best_features.strategy_ftor = batch.front();
    perform_trial(parameters, best_features, neural_features, true);
    std::wcout << std::endl;
    std::wcout << "BATCH " << i << " --- Best : " << fitnesses[0] << ", "
                                                  << fitnesses[1] << ", "
                                                  << fitnesses[2] << std::endl;

    for (auto& candidate : batch | ltl::drop_n(elitism)) {
      candidate = batch[pick_index(rnd_engine)];
      auto copy = batch[pick_index(rnd_engine)];
      crossover_uniform(candidate.view(), copy.view());
      mutate(candidate.view(), 0.001, [&](auto& value, auto& rnd_engine) { value = make_noise(rnd_engine); });
    }
  }

  std::ofstream log("log.txt");
  neural_features.strategy_ftor = batch.front();
  measure_accuracy(1e4, parameters, neural_features, neural_features, log);

  std::wstring answer, yes(L"o"), no(L"n");
  std::wcout << "Faire une démonstration ?" << std::endl;
  std::wcin >> answer;
  while (answer == yes) {
    parameters.seed = generate_seed();
    perform_trial(parameters, best_features, neural_features, true);
    std::wcout << "Recommencer ?" << std::endl;
    std::wcin >> answer;
  }

  return 0;
}
