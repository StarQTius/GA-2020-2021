#ifndef GATE_PLAYGROUND_HPP
  #define GATE_PLAYGROUND_HPP

  #include <chrono> //system_clock
  #include <functional> //ref, reference_wrapper
  #include <list>
  #include <random> //default_random_engine, uniform_real_distribution

  #include <cmath> //M_PI, cos, sin, atan2, signbit, abs, max
  #define _USE_MATH_DEFINES

  #include "entity.hpp"
  #include "neural_network.hpp"

  #include <SFML/Graphics.hpp>

  namespace gate{
    /**
      TODO RM
    **/
    void print_nn(const NeuralNetwork& relunn) {
      for(auto wmat_bvect : relunn.neural_map) {
        std::cout << "WMAT :" << std::endl;
        const auto& wmat = wmat_bvect.first;
        for(auto it1 = wmat.begin1(); it1 != wmat.end1(); it1++) {
          for(auto val : it1)
            std::cout << val << ' ';
          std::cout << std::endl;
        }

        std::cout << "BVECT :" << std::endl;
        for(auto val : wmat_bvect.second) std::cout << val << ' ';
        std::cout << std::endl;
      }
    }

    /**
      TODO RM
    **/
    void print_vect(const Vector& vect) {
      for(auto val : vect) std::cout << val << ' ';
      std::cout << std::endl;
    }

    /*
      Trial : permet de simuler une épreuve
    */

    class Trial{

    public:

      /**

      **/

      Trial(std::default_random_engine::result_type seed):
        seed{seed}
      {}

      /**
        TODO : factoriser ça ptn
      **/

      duration_s operator()(const NeuralNetwork& neural_network){

        std::default_random_engine rnd_engine{seed};
        std::uniform_real_distribution<distance_m> pos_x_distr{
          robot_radius_m,
          space_width - robot_radius_m
        };
        std::uniform_real_distribution<distance_m> pos_y_distr{
          robot_radius_m,
          space_height - robot_radius_m
        };
        std::uniform_real_distribution<angle_rad> dir_distr{0, 2 * M_PI};

        Rect space{0, 0, space_width, space_height};

        Candidate candidate{
          {pos_x_distr(rnd_engine), pos_y_distr(rnd_engine)},
          dir_distr(rnd_engine),
          robot_radius_m,
          {pos_x_distr(rnd_engine), pos_y_distr(rnd_engine)},
          neural_network
        };

        std::list<Robot> foe_l;
        for (auto i = 0; i < foe_nb; i++)
          foe_l.emplace_back(
            Foe(
              {pos_x_distr(rnd_engine), pos_y_distr(rnd_engine)},
              robot_radius_m,
              rnd_engine));

        duration_s time_s = 0;
        Outcome outcome;
        while ((outcome = candidate.update(space, foe_l)) == Outcome::none && time_s < time_limit_s) {
          for (auto& foe : foe_l)
            foe.update(space, std::list<Robot>{candidate});
          time_s += dt_s;
        }

        switch(outcome){
          case Outcome::none :
          case Outcome::goal_reached :
            return time_s;
          case Outcome::candidate_collided :
            return time_limit_s;
          case Outcome::foe_collided :
            return (*this)(neural_network); //On recommence avec une autre épreuve
        }

      }

    private:

      /*
        Variables membres
      */

      const std::default_random_engine::result_type seed;

    };
  }
#endif
