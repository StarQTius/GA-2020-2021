#pragma once

#include <concepts>
#include <functional>

#include <boost/callable_traits/return_type.hpp>

#include <ltl/algos.h>

#include "linear.hpp"

/*******************************************************************************
*******************************************************************************/

namespace neural {

//TODO : enlever la rustine
template<typename Scalar, auto& ActivationFunction>
struct StaticPerceptron {
  using scalar = Scalar;

  StaticPerceptron(const Matrix<scalar>& weights)
    : weights(weights)
  {}

  StaticPerceptron(Eigen::Index input_size, Eigen::Index output_size)
    : weights(output_size, input_size)
  {}

  //
  Vector<scalar> operator<<(const Vector<scalar>& input) const {
    using F = decltype(ActivationFunction);
    static_assert(Vectorizable<F, scalar> || Modifying<F, Vector<scalar>>
                  || Returning<F, Vector<scalar>>,
                  "Activation function must satisfy one of the following : "
                  "Vectorizable<scalar>, Modifying<Vector<scalar>> or "
                  "Returning<Vector<scalar>>");
    if constexpr (Vectorizable<F, scalar>) {
      return (weights * input).unaryExpr(std::ref(ActivationFunction));
    } else if constexpr (Modifying<F, Vector<scalar>>) {
      Vector<scalar> output(weights * input);
      ActivationFunction(output);
      return output;
    } else {
      return ActivationFunction(Vector<scalar>(weights * input));
    }
  }

  //
  Matrix<scalar> operator<<(const Matrix<scalar>& input) const {
    using F = decltype(ActivationFunction);
    static_assert(Vectorizable<F, scalar> || Modifying<F, Matrix<scalar>>
               || Returning<F, Matrix<scalar>>,
                  "Activation function must satisfy one of the following : "
                  "Vectorizable<scalar>, Modifying<Matrix<scalar>> or "
                  "Returning<Matrix<scalar>>");
    if constexpr (Vectorizable<F, scalar>) {
      return (weights * input).unaryExpr(std::ref(ActivationFunction));
    } else if constexpr (Modifying<F, Matrix<scalar>>) {
      Matrix<scalar> output(weights * input);
      ActivationFunction(output);
      return output;
    } else {
      return ActivationFunction(Matrix<scalar>(weights * input));
    }
  }

  //
  auto input_size() const {
    return weights.cols();
  }

  //
  auto output_size() const {
    return weights.rows();
  }

  //
  void resize(Eigen::Index rows_nb, Eigen::Index columns_nb) {
    weights.resize(rows_nb, columns_nb);
  }

  Matrix<scalar> weights;
};

} // namespace neural
