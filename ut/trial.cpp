#include <catch.hpp>
#include "../trial/trial.hpp"

TEST_CASE("perform_trial : tester une execution sans affichage") {

  TrialParameters trial_parameters {
    .playground = Playground { .left = 0_q_m, .top = 0_q_m, .width = 1_q_m, .height = 1_q_m },
    .foe_nb = 2,
    .seed = 19937,
    .dt = 1_q_ms,
    .time_limit = 100_q_s,
  };

  RobotFeatures robot_features {
    .hitbox = Hitbox { .radius = 30_q_cm },
    .speed_parameters = physics::speed(1_q_m_per_s),
    .angular_speed_parameters = physics::angular_speed(1_q_rad_per_s),
    .shape = sf::CircleShape(30),
    .goal_mark_shape = sf::CircleShape(5)
  };

  SECTION("") {

  }
}

/*
template<typename T>
concept Vectorial = requires(T t, std::floating_point auto floating_point, Vectorial auto vectorial) {
  { floating_point * t; } -> Vectorial;  // Multiplication par un scalaire à gauche
  { t * floating_point; } -> Vectorial;  // Multiplication par un scalaire à droite
  { t *= floating_point; } -> Vectorial; //
  { t / floating_point; } -> Vectorial;  // Division par un scalaire à droite
  { t /= floating_point; } -> Vectorial; //
};

template<typename T>
concept Matricial = Vectorial<T> && requires(T t, Matricial matricial) {
  { matricial * t; } -> Matricial; // Produit matriciel à droite
  { t * matricial; } -> Matricial; // Produit matriciel à gauche
}

template<typename T>
concept Normed = requires(T t) {
  { norm(t); } -> std::scalar
};

concept Hilbertian = requires(T t, std::)

template<typename T>
concept Layer = requires(T t, Vectorial auto vectorial) { {  vectorial >> t; } -> Vectorial };
*/
