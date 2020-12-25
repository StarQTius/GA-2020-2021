#pragma once

#include <chrono>

/*******************************************************************************
  seed.hpp : definit la fonction 'generate_seed' qui permet de générer une
  graine pour les générateurs de nombre aléatoire. Il s'agit simplement d'un
  alias de 'std::chrono::system_clock::now().time_since_epoch().count', ce qui
  permet d'améliorer la lisibilité du code.
*******************************************************************************/

inline auto generate_seed() {
  return std::chrono::system_clock::now().time_since_epoch().count();
}

/*
template<typename... Args>
using Procedure = void(Args&...);

template<typename... Args>
using Predicate = bool(const Args&...);

template<class Manager, class Data>
class OrderWrapper {
public:
  constexpr OrderWrapper(Procedure<Manager, Data>& procedure,
                         Predicate<Manager, Data>& should_execute)
    : procedure(procedure),
      should_execute(should_execute) {}

   auto operator()(Manager& manager, Data& data) const {
     bool has_executed = false;
     if (has_executed = should_execute(manager, data)) procedure(manager, data);

     return has_executed
   }

private:
  Procedure<Manager, Data>& procedure;
  Predicate<Manager, Data>& should_execute;

};

enum class Order {
  cx,
  cy
};

constexpr auto order_to_int(Order order) {
  return static_cast<std::underlying_type<Order>::type>(order);
}

constexpr OrderWrapper<OrderManager, OrderData> order_procedures[] = {
  [order_to_int(cx)] = {translate_horizontaly, got_at_least_N_args<1>},
  [order_to_int(cy)] = {translate_verticaly, got_at_least_N_args<1>}
};

auto execute_order(Order order, OrderManager& manager, OrderData& data) {
  return order_procedures[order_to_int(order)](manager, data);
}
*/
