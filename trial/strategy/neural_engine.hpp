#pragma once

#include <cmath>
#include <functional>
#include <utility>

#include <entt/entt.hpp>

#include "ltl/functional.h"
#include "ltl/Range/enumerate.h"
#include "ltl/Tuple.h"

#include "../../neural/net.hpp"
#include "../../neural/static_perceptron.hpp"
#include "../component.hpp"
#include "../math.hpp"
#include "../trial_parameters.hpp"

double relu(double x) { return std::max(x, 0.0); }
double sigmoid(double x) { return 1 / (1 + std::exp(-x)); }

using FetcherType = double(entt::entity, entt::registry&);

class NeuralEngine {
public:
  std::ranges::range auto view() {
    using Matrix = decltype(net[0_n].weights);
    return std::array {net[0_n].weights.reshaped(), net[1_n].weights.reshaped()}
        | ltl::join;
  }

  static constexpr auto output_size = 3;

  template<typename... Args>
  NeuralEngine(int hidden_layer_size, Args&&... fetchers)
    : net {(int) sizeof...(Args), hidden_layer_size, output_size},
      fetchers {std::forward<Args>(fetchers)...} {}

  Setpoint operator()(entt::entity entity, entt::registry& registry) {
    using Vector = neural::Vector<double>;
    Vector input(fetchers.size());
    for (auto&& [i, fetcher] : ltl::enumerate(fetchers))
      input[i] = fetcher(entity, registry);

    auto output = net << input;
    auto move = output[0];
    auto turn_left = output[1];
    auto turn_right = output[2];

    return Setpoint {
      .speed = 2.0_q_m_per_s * std::min(move, 0.5),
      .angular_speed = 6.0_q_rad_per_s * (turn_right - turn_left)
    };
  };

private:
  neural::Net<
    neural::StaticPerceptron<double, relu>,
    neural::StaticPerceptron<double, sigmoid>
  > net;
  std::list<std::function<FetcherType>> fetchers;
};
