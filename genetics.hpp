#ifndef GATE_GENETICS_HPP
  #define GATE_GENETICS_HPP

  #include <algorithm> //sort, for_each
  #include <chrono> //system_clock
  #include <cmath> //abs
  #include <iterator> //distance, make_move_iterator
  #include <random> //default_random_engine, uniform_real_distribution
  #include <type_traits> //decay
  #include <utility> //pair
  #include <vector>

  namespace gate{

    template<typename FitnessFunctor, typename Iterator>
    void select(Iterator, Iterator, FitnessFunctor&&);

    template<typename Iterator>
    constexpr auto& cross_over = std::swap_ranges<Iterator, Iterator>;

    template<
      typename Iterator,
      typename floating_point,
      typename RandomModifier
    >
    void mutate(Iterator, Iterator, floating_point, RandomModifier&&);

  }

  #include "genetics.tcc"

#endif
