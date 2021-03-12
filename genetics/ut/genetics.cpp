#include <catch.hpp>

#include "../genetics.hpp"

#include <list>

struct AssignementAwareInteger {
  AssignementAwareInteger(int content): copy_counter(0), move_counter(0), content(content) {}

  AssignementAwareInteger(const AssignementAwareInteger& copy_val)
    : copy_counter(1), move_counter(0), content(copy_val.content) {}

  AssignementAwareInteger(AssignementAwareInteger&& move_val)
    : copy_counter(0), move_counter(1), content(move_val.content) {}


  AssignementAwareInteger& operator=(const AssignementAwareInteger& copy_val) {
    content = copy_val.content;
    copy_counter++;
    return *this;
  }

  AssignementAwareInteger& operator=(AssignementAwareInteger&& move_val) {
    content = move_val.content;
    move_counter++;
    return *this;
  }

  int copy_counter;
  int move_counter;
  int content;

};

TEST_CASE("genetics::sort_candidates") {

  std::list<int> int_l {0, 9, 5, 1, 8, 4, 3, 2, 7, 6};

  SECTION("La fonction 'sort_candidates' range un intervalle d'éléments ayant"
          " un score de fitness par ordre décroissant") {

    auto fitness = genetics::sort_candidates(int_l, [](int val){ return val; });
    REQUIRE(int_l == std::list<int> {9, 8, 7, 6, 5, 4, 3, 2, 1, 0});
    REQUIRE(fitness == std::vector<int> {9, 8, 7, 6, 5, 4, 3, 2, 1, 0});

  }

  std::list<AssignementAwareInteger> asg_aware_int_l;
  asg_aware_int_l.emplace_back(0); //pour éviter de faire une copie

  SECTION("La fonction 'sort_candidates' ne fait aucune copie") {

    genetics::sort_candidates(asg_aware_int_l,
      [](const auto& asg_aware_int) {
        return asg_aware_int.content;
      });
    REQUIRE(asg_aware_int_l.front().copy_counter == 0);

  }

}

TEST_CASE("genetics::crossover_uniform") {

  std::list<bool> false_l(1e4, false);
  std::list<bool> true_l(1e4, true);

  SECTION("En moyenne, la moitié des gènes du premier génome sont permuté avec"
          " leurs homologues respectifs dans le second génome (si cette section"
          " du test unitaire échoue, cela peut signifier que la distribution"
          " aléatoire obtenue est atypique : dans ce cas, il faut réexecuter"
          " le test)") {

    genetics::crossover_uniform(false_l, true_l);
    CHECK(abs(ltl::count(false_l, true) - 5e3) <= 2e3);

  }

}

TEST_CASE("genetics::mutate") {

  std::list<bool> bool_l(1e4, false);

  SECTION("En moyenne, une proportion p des gènes sont mutés lorsque p est le"
          " taux de mutation passé en paramètre à 'mutate' (si cette section du"
          " test unitaire échoue, cela peut signifier que la distribution"
          " aléatoire obtenue est atypique : dans ce cas, il faut réexecuter"
          " le test)") {

    genetics::mutate(bool_l, 0.5,
      [](bool& value, auto&& rnd_engine) {
        value = true;
      });
    CHECK(abs(ltl::count(bool_l, true) - 5e3) <= 2e3);

  }

}
