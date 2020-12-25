#pragma once

#include <concepts>

#include <boost/callable_traits/return_type.hpp>

#include <ltl/algos.h>

#include "linear.hpp"

/*******************************************************************************
*******************************************************************************/

namespace neural {

template<typename F>
struct Perceptron {
  using scalar = boost::callable_traits::return_type_t<F>;

  Perceptron(const Matrix<scalar>& weights, const F& activation_function)
    : weights(weights),
      activation_function(activation_function)
  {}

  Perceptron(const Matrix<scalar>& weights, F&& activation_function)
    : weights(weights),
      activation_function(std::move(activation_function))
  {}

  Perceptron(Eigen::Index input_size, Eigen::Index output_size)
    : weights(output_size, input_size)
  {}

  //
  Vector<scalar> operator<<(const Vector<scalar>& input) const {
    static_assert(Vectorizable<F, scalar> || Modifying<F, Vector<scalar>>
                  || Returning<F, Vector<scalar>>,
                  "Activation function must satisfy one of the following : "
                  "Vectorizable<scalar>, Modifying<Vector<scalar>> or "
                  "Returning<Vector<scalar>>");
    if constexpr (Vectorizable<F, scalar>) {
      return (weights * input).unaryExpr(activation_function);
    } else if constexpr (Modifying<F, Vector<scalar>>) {
      Vector<scalar> output(weights * input);
      activation_function(output);
      return output;
    } else {
      return activation_function(Vector<scalar>(weights * input));
    }
  }

  //
  Matrix<scalar> operator<<(const Matrix<scalar>& input) const {
    static_assert(Vectorizable<F, scalar> || Modifying<F, Matrix<scalar>>
               || Returning<F, Matrix<scalar>>,
                  "Activation function must satisfy one of the following : "
                  "Vectorizable<scalar>, Modifying<Matrix<scalar>> or "
                  "Returning<Matrix<scalar>>");
    if constexpr (Vectorizable<F, scalar>) {
      return (weights * input).unaryExpr(activation_function);
    } else if constexpr (Modifying<F, Matrix<scalar>>) {
      Matrix<scalar> output(weights * input);
      activation_function(output);
      return output;
    } else {
      return activation_function(Matrix<scalar>(weights * input));
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
  F activation_function;
};

} // namespace neural
