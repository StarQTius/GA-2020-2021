#include <catch.hpp>
#include "../entity.hpp"

#include <SFML/System.hpp>
#include <SFML/Window.hpp>

struct Hitbox : sf::IntRect {

  using sf::IntRect::IntRect;

  bool is_in(sf::IntRect space){

    return space.contains(left, top)
        && space.contains(left + width, top + height);

  }

};

typedef gate::Entity<int, Hitbox, void, void> Entity_ut;
struct EntityContent{

  static constexpr int hitbox_width = 50;
  static constexpr int hitbox_height = 50;

  static sf::Color fill_color;
  static sf::Color outline_color;

  EntityContent():
    hitbox{hitbox_width, 300, hitbox_width, hitbox_height}
  {}

  void update(sf::IntRect space, std::list<Entity_ut> entity_l){

    for(const auto& entity_ut : entity_l){

      if(entity_ut.shape().intersects(hitbox))
        throw 0;

    }

    hitbox.left += 1;
    hitbox.left %= space.left + space.width - hitbox.width;

  }

  Hitbox shape() const {

    return hitbox;

  }

  void tag() const {}

  void draw(sf::RenderTarget& rd_target, sf::RenderStates rd_states) const {

    sf::RectangleShape sprite{ sf::Vector2f{hitbox_width, hitbox_height} };
    sprite.setPosition(hitbox.left, hitbox.top);
    sprite.setFillColor(fill_color);
    sprite.setOutlineColor(outline_color);

    rd_target.draw(sprite, rd_states);

  }

  Hitbox hitbox;

};

sf::Color EntityContent::fill_color{50, 100, 200, 255};
sf::Color EntityContent::outline_color{30, 75, 170, 255};

TEST_CASE("gate::entity"){

  sf::RenderWindow window(sf::VideoMode{800, 600}, "Entity unit test");
  sf::Font font;
  font.loadFromFile("../font.ttf");

  Entity_ut entity_ut{ EntityContent{} };
  sf::IntRect space{0, 0, 700, 600};

  SECTION("Une entité s'affiche correctement à l'écran avec la fonction"
          "'draw'")
  {

    sf::Text question{
      L"Est-ce que l'entité est bien affichée à l'écran ? (carré bleu)",
      font
    };
    question.setPosition(0, 450);
    question.setFillColor(sf::Color::White);
    sf::Text yes{L"Oui", font}; sf::Text no{L"Non", font};
    yes.setPosition(0, 500); no.setPosition(150, 500);
    yes.setFillColor(sf::Color::White); no.setFillColor(sf::Color::White);
    sf::IntRect yes_box{yes.getGlobalBounds()};
    sf::IntRect no_box{no.getGlobalBounds()};

    window.clear();
    window.draw(entity_ut);
    window.draw(question);
    window.draw(yes);
    window.draw(no);
    window.display();

    bool success = false;
    while(window.isOpen()){

      sf::Event event;
      window.pollEvent(event);

      if(event.type == sf::Event::Closed){

        window.close();

      }

      if(event.type == sf::Event::MouseButtonPressed){

        sf::Vector2i mouse_pos{ sf::Mouse::getPosition(window) };

        if(yes_box.contains(mouse_pos)){

          success = true;
          window.close();

        }

        if(no_box.contains(mouse_pos))
          window.close();

      }

    }

    REQUIRE(success);

  }

  SECTION("Une entité peut être mis à jour avec la fonction 'update'"){

    sf::Text question{
      L"Est-ce que l'entité se déplace bien vers la droite ?",
      font
    };
    question.setPosition(0, 450);
    question.setFillColor(sf::Color::White);
    sf::Text yes{L"Oui", font}; sf::Text no{L"Non", font};
    yes.setPosition(0, 500); no.setPosition(150, 500);
    yes.setFillColor(sf::Color::White); no.setFillColor(sf::Color::White);
    sf::IntRect yes_box{yes.getGlobalBounds()};
    sf::IntRect no_box{no.getGlobalBounds()};

    sf::Clock clock;

    bool success = false;
    while(window.isOpen()){

      window.clear();
      window.draw(entity_ut);
      window.draw(question);
      window.draw(yes);
      window.draw(no);
      window.display();

      sf::Event event;
      window.pollEvent(event);

      if(event.type == sf::Event::Closed){

        window.close();

      }

      if(event.type == sf::Event::MouseButtonPressed){

        sf::Vector2i mouse_pos{ sf::Mouse::getPosition(window) };

        if(yes_box.contains(mouse_pos)){

          success = true;
          window.close();

        }

        if(no_box.contains(mouse_pos))
          window.close();

      }

      sf::Time begin = clock.getElapsedTime();
      while(clock.getElapsedTime() < begin + sf::milliseconds(10)) {}

      entity_ut.update(space, std::list<Entity_ut>{});

    }

    REQUIRE(success);

  }

  SECTION("Les entités ont une hitbox qui sert de référence pour les"
          " collisions")
  {

    Entity_ut entity_1{ EntityContent{} };
    Entity_ut entity_2{ EntityContent{} };

    REQUIRE_THROWS(entity_1.update(space, std::list<Entity_ut>{entity_2}));

  }

}
