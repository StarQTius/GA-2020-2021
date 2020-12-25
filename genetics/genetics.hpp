#pragma once

#include <algorithm>
#include <concepts>
#include <iterator>
#include <random>
#include <ranges>
#include <tuple>
#include <type_traits>
#include <utility>

#include <ltl/Range/Map.h>
#include <ltl/Range/Zip.h>
#include <ltl/Range/Filter.h>
#include <ltl/Range/actions.h>
#include <ltl/operator.h>

#include "../seed.hpp"

namespace genetics {

template<typename T, typename Candidate>
concept FitnessFunctor = requires(T t, Candidate candidate) {
  { t(candidate) } -> std::convertible_to<long double>;
};

template<typename T, typename Value, typename RndEngine>
concept RandomModifier = requires(T t, Value value, RndEngine rnd_engine) {
  { t(value, rnd_engine) } -> std::same_as<void>;
};

/**
  Trier les candidats selon leurs scores respectifs :
  Les scores sont données par le foncteur 'fitness_ftor'. La fonction range sur
  place la liste des candidats et retourne leurs scores dans l'ordre
  correspondant à l'ordre final de 'candidates' (ie, le premier score est celui
  du premier candidat dans 'candidates', le deuxième est celui du deuxième
  candidat, ect...). Les scores sont retournés dans un 'std::vector'.
**/

auto sort_candidates(std::ranges::range auto&& candidates,
                     FitnessFunctor<decltype(*std::ranges::begin(candidates))> auto&& fitness_ftor) {
  auto make_rated_candidate = [&](auto& candidate) {
    auto fitness = fitness_ftor(candidate);
    return std::make_pair(std::move(candidate), std::move(fitness));
  };
  auto extract_candidate = [](auto&& rated_candidate) { return std::move(rated_candidate.first); };
  auto extract_fitness = [](auto&& rated_candidate) { return std::move(rated_candidate.second); };
  auto compare_rated_candidate = [](const auto& lhs, const auto& rhs){ return lhs.second > rhs.second; };

  auto rated_candidates = candidates
                        | ltl::map(make_rated_candidate)
                        | ltl::to_vector
                        | ltl::actions::sort_by(compare_rated_candidate);
  ltl::copy(rated_candidates | ltl::map(extract_candidate), ltl::begin(candidates));

  return rated_candidates | ltl::map(extract_fitness) | ltl::to_vector;
}

/**
  Croiser deux génomes de manière uniforme :
  Pour deux génomes de même taille, chaque gene dans le premier génome à une
  1 chance sur 2 d'être permuter avec son homologue dans le second génome (ie,
  même position dans les génomes respectifs).
  TODO : rajouter la contrainte taille de genes1 = taille de genes2
**/
void crossover_uniform(std::ranges::range auto&& genes1, std::ranges::range auto&& genes2) {
  std::mt19937                rnd_engine(generate_seed());
  std::bernoulli_distribution should_swap_genes(0.5);

  auto zipped_genes = ltl::zip(genes1, genes2)
                    | ltl::filter([&](auto&&) { return should_swap_genes(rnd_engine); });
  ltl::for_each(zipped_genes, ltl::unzip(lift(std::swap)));
}

/**
  Faire muter un génome :
  Chaque gene à une chance 'rate' d'être modifié par le foncteur 'rnd_modifier'.
**/
void mutate(std::ranges::range auto&& genes, std::floating_point auto rate,
            RandomModifier<decltype(*std::ranges::begin(genes)), std::mt19937> auto&& rnd_modifier) {
  std::mt19937                rnd_engine(generate_seed());
  std::bernoulli_distribution should_mutate(rate);

  auto target_genes = genes | ltl::filter([&](auto&&) { return should_mutate(rnd_engine); });
  for (auto& gene : target_genes) rnd_modifier(gene, rnd_engine);
}

} // namespace genetics
