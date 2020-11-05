#include <catch.hpp>

#include "../genetics.hpp"

#include <list>

struct MoveAwareInteger{
  MoveAwareInteger(int content): move_counter(0), content(content) {}

  MoveAwareInteger(const MoveAwareInteger& cp_val){
    content = cp_val.content;
  }

  MoveAwareInteger(MoveAwareInteger&& mv_val){
    content = mv_val.content;
    move_counter++;
  }

  MoveAwareInteger& operator=(const MoveAwareInteger& cp_val){
    content = cp_val.content;
    return *this;
  }

  MoveAwareInteger& operator=(MoveAwareInteger&& mv_val){
    content = mv_val.content;
    move_counter++;
    return *this;
  }

  int move_counter;
  int content;

};

TEST_CASE("gate::select"){

  std::list<int> int_l = {0, 9, 5, 1, 8, 4, 3, 2, 7, 6};

  SECTION("La fonction 'select' range un intervalle d'éléments ayant un score"
          " de fitness par ordre décroissant")
  {

    gate::select(int_l.begin(), int_l.end(), [](int val){ return val; });
    REQUIRE(int_l == std::list<int>{{9, 8, 7, 6, 5, 4, 3, 2, 1, 0}});

  }

  std::list<MoveAwareInteger> mv_awa_int_l = {0};

  SECTION("La fonction 'select' transfert les valeurs des éléments via"
          " assignement move vers un conteneur auxilliaire puis réassigne les"
          " valeurs par ordre décroissant de fitness par assignement move")
  {

    gate::select(mv_awa_int_l.begin(), mv_awa_int_l.end(),
      [](const auto& mv_awa_int){
        return mv_awa_int.content;
      }
    );
    REQUIRE(mv_awa_int_l.front().move_counter == 6);

  }

}

TEST_CASE("gate::mutate"){

  std::list<bool> bool_l(1e4, false);

  SECTION("En moyenne, une proportion p des gènes sont mutés lorsque p est le"
          " taux de mutation passé en paramètre à 'mutate' (si cette section du"
          " test unitaire échoue, cela peut signifier que la distribution"
          " aléatoire obtenue est atypique : dans ce cas, il faut réexecuter"
          " le test)")
  {

    gate::mutate(bool_l.begin(), bool_l.end(), 0.5,
      [](auto&& rnd_gtor){
        return true;
      }
    );
    CHECK(abs(std::count(bool_l.begin(), bool_l.end(), true) - 5e3) <= 2e3);

  }

}
