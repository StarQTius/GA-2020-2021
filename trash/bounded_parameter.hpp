#pragma once

#include <exception>
#include <concepts>

#include <units/quantity.h>

/*******************************************************************************
*******************************************************************************/

template<Quantity Q>
class BoundedParameter {
public:
  struct invalid_ratio_argument {
    virtual const char* what() const {
      return "Given argument is not in the range [0.0, 1.0]";
    }
  };

  BoundedParameter(Q max) : max(max) {}

  BoundedParameter& operator=(const BoundedParameter&) = delete;
  BoundedParameter& operator=(BoundedParameter&&) = delete;

  Q get() const { return current; }
  Q max() const { return max; }
  void set(std::floating_point ratio) {
    if (ratio < 0.0 || 1.0 < ratio) throw invalid_ratio_argument();
    current = max * ratio;
  }

private:
  Q current;
  Q max;
};
