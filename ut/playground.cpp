#include "../playground.hpp"

sf::Color         gate::Candidate::fill_color{sf::Color::Blue};
sf::Color         gate::Candidate::outline_color{sf::Color::Blue};
gate::distance_px gate::Candidate::goal_mark_radius_px{3};
sf::Color         gate::Candidate::goal_mark_fill_color{sf::Color::Blue};
sf::Color         gate::Candidate::goal_mark_outline_color{sf::Color::Blue};

sf::Color         gate::Foe::fill_color{sf::Color::Red};
sf::Color         gate::Foe::outline_color{sf::Color::Red};
gate::distance_px gate::Foe::goal_mark_radius_px{3};
sf::Color         gate::Foe::goal_mark_fill_color{sf::Color::Red};
sf::Color         gate::Foe::goal_mark_outline_color{sf::Color::Red};

#include <chrono>
#include <thread>

using namespace std::this_thread;
using namespace std::chrono_literals;
using namespace std::chrono;

int main(){

  std::default_random_engine rnd_engine{200};

  sf::RenderWindow window{sf::VideoMode{2000, 1500}, "Test"};

  gate::ReLUNN relunn{
    {gate::input_vect_sz, 5, gate::output_vect_sz},
    std::uniform_real_distribution<gate::scalar>{},
    gate::relu
  };

  std::list<gate::Robot> candidate_l{
    gate::Candidate{ {0.6, 0.6}, 0, gate::robot_radius_m, {500, 500}, relunn }
  };
  std::list<gate::Robot> foe_l{
    gate::Foe{ {1.0, 1.0}, gate::robot_radius_m, rnd_engine }
  };

  gate::Rect space{0, 0, 3.0, 2.0};
  sf::RectangleShape field{
    {gate::px_per_m * space.width,
     gate::px_per_m * space.height}
  };
  field.setPosition(0.0, 0.0);

  while(window.isOpen()){

    sf::Event event;
    while(window.pollEvent(event)){

      if(event.type == sf::Event::Closed)
        window.close();

    }

    for(auto& foe : foe_l)
      foe.update(space, candidate_l);
    /*for(auto& candidate : candidate_l)
      candidate.update(space, foe_l);*/

    window.clear();

    window.draw(field);
    for(auto& foe : foe_l)
      window.draw(foe);
    /*for(auto& candidate : candidate_l)
      window.draw(candidate);*/

    sleep_for(microseconds((int) (gate::dt_s * 1000000)));

    window.display();

  }

  return 0;
}
