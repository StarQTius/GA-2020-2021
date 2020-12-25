#pragma once

#include <eigen3/Eigen/Dense>

namespace neural {

  template<typename Scalar>
  using Matrix = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>;

  template<typename Scalar>
  using Vector = Eigen::Matrix<Scalar, Eigen::Dynamic, 1>;

  template<typename T, typename Scalar>
  concept Vectorizable = requires(T t, Scalar scalar) { { t(scalar) } -> std::convertible_to<Scalar>; };

  template<typename T, typename Vector>
  concept Modifying = requires(T t, Vector vector) { { t(vector) } -> std::same_as<void>; };

  template<typename T, typename Vector>
  concept Returning = requires(T t, Vector vector) { { t(vector) } -> std::convertible_to<Vector>; };

} // namespace neural
