#pragma once

#include <concepts>
#include <iterator>
#include <ranges>
#include <type_traits>
#include <utility>
#include <vector>

#include "ltl/operator.h"
#include "ltl/Tuple.h"

/*******************************************************************************
*******************************************************************************/

namespace neural {

template<int... Is>
using int_seq = std::integer_sequence<int, Is...>;

//
template<int B, int E, int... Is>
struct int_range : int_range<B, E - 1, E - 1, Is...> {};

template<int B, int... Is>
struct int_range<B, B, Is...> : int_seq<Is...> {};

//
template<typename T>
concept Layer = requires(T t, int8_t n) {
  T(n, n);
  { t.output_size() } -> std::integral;
  { t.input_size() } -> std::integral;
};

template<typename T>
concept Resizable = requires(T t, int8_t n) { t.resize(n, n); };

template<Layer... Layers>
class Net {
  template<typename... Args> friend auto make_net(Args&&...);

public:
  // Construire le réseau en donnant les tailles des intercouches
  template<std::integral... Ints> requires (sizeof...(Ints) - 1 == sizeof...(Layers))
  Net(Ints... layer_sizes)
    : Net(std::vector<int64_t> {layer_sizes...}, layer_tuple.make_indexer()) {}
  Net(const std::ranges::range auto& layer_sizes)
    : Net(std::vector(layer_sizes.begin(), layer_sizes.end()), layer_tuple.make_indexer()) {}

  // Accéder à une couche
  auto&       operator[](auto i) { return layer_tuple[i]; };
  const auto& operator[](auto i) const { return layer_tuple[i]; }

  // Calculer le résultat d'une entrée par le réseau
  template<typename T>
  decltype(auto) operator<<(T&& input) {
    return propagate_forward(std::forward<T>(input), layer_tuple.make_indexer());
  }

  // Redimensionner une intercouche
  template<int I>
  void resize(ltl::number_t<I> index, std::integral auto new_size) {
    if constexpr (I > 0) {
      auto previous = ltl::number_t<I - 1>();
      resize(previous, layer_tuple[previous].input_size(), new_size);
    }
    if constexpr (I < sizeof...(Layers))
      resize(index, new_size, layer_tuple[index].output_size());
  }

  // Redimensionner une série d'intercouches consécutives
  template<int B, int E>
  void resize(ltl::number_t<B> begin, ltl::number_t<E> end,
              const std::ranges::range auto& new_sizes) {
    resize(begin, end, {new_sizes.begin(), new_sizes.end()}, int_range<B, E - 1>());
  }
  template<int B, int E>
  void resize(ltl::number_t<B> begin, ltl::number_t<E> end, const std::vector<size_t>& new_sizes) {
    resize(begin, end, new_sizes, int_range<B, E - 1>());
  }
  template<int B, int E>
  void resize(ltl::number_t<B> begin, ltl::number_t<E> end, std::vector<size_t>&& new_sizes) {
    resize(begin, end, std::move(new_sizes), int_range<B, E - 1>());
  }

  // Appliquer un foncteur à chaque couche du réseau
  template<typename F>
  void for_each(F&& ftor) {
    ltl::for_each(layer_tuple, ftor);
  }

private:
  // Construire un réseau en passant les couches via perfect forwarding
  // Le CTAD étant impossible, ce constructeur est en privé et est utilisé par
  // 'make_net'
  template<Layer... Args>
  Net(Args&&... args)
    : layer_tuple(std::forward<Args>(args)...) {}

  // Construire un réseau en donnant les tailles des intercouches
  // Ce constructeur est appellé par les deux autres constructeurs qui prennent
  // aussi en argument les tailles des intercouches. La variable supplémentaire
  // sert juste à tenir le pattern à étendre.
  template<int... Is>
  Net(const std::vector<auto>& layer_sizes, ltl::number_list_t<Is...>)
    : Net(Layers(layer_sizes.at(Is), layer_sizes.at(Is + 1))...)
  {}

  // Calculer le résultat d'une entrée par le réseau
  // Elle permet d'invoquer succesivement chaque couche, de la première à la
  // dernière.
  template<typename T, int... Is>
  decltype(auto) propagate_forward(T&& input, ltl::number_list_t<Is...>) {
    return (layer_tuple[ltl::number_t<sizeof...(Layers) - 1 - Is>()] << ... << std::forward<T>(input));
  }

  // Redimensionner
  template<int B, int E, int... Is>
  void resize(ltl::number_t<B>, ltl::number_t<E>, const std::vector<auto>& new_sizes, int_seq<Is...>) {
    // Si le redimensionnement n'affecte pas la taille de l'entrée du réseau alors
    // changer seulement la taille de la sortie de la première couche (designée
    // par 'begin').
    if constexpr (B > 0)
      resize(ltl::number_t<B - 1>(), layer_tuple[ltl::number_t<B - 1>()].input_size(), new_sizes.front());

    // Changer la taille de l'entrée et de la sortie les couches autres que la
    // première et dernière couche, s'il y en a.
    if constexpr (B < E - 1) (resize(ltl::number_t<Is>(), new_sizes[Is - B], new_sizes[Is - B + 1]), ...);

    // Si le redimensionnement n'affecte pas la taille de la sortie du réseau alors
    // changer seulement la taille de l'entrée de la dernière couche (désignée par
    // 'end').
    if constexpr (E <= sizeof...(Layers))
      resize(ltl::number_t<E - 1>(), new_sizes.back(), layer_tuple[ltl::number_t<E - 1>()].output_size());
  }

  //
  template<int I>
  void resize(ltl::number_t<I> layer_index, std::integral auto new_input_size,
              std::integral auto new_output_size) {
    using L = std::decay_t<decltype(layer_tuple[layer_index])>;
    if constexpr (Resizable<L>)
      layer_tuple[layer_index].resize(new_input_size, new_output_size);
    else
      layer_tuple[layer_index] = L(new_input_size, new_output_size);
  }

  ltl::tuple_t<Layers...> layer_tuple;

};

//
template<typename... Args>
auto make_net(Args&&... args) {
  return Net<std::decay_t<Args>...>(std::forward<Args>(args)...);
}

} // namespace neural
