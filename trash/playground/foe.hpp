#ifndef GATE_PLAYGROUND_FOE_HPP
#define GATE_PLAYGROUND_FOE_HPP

/*******************************************************************************
  gate::Foe : représente les robots ennemis. Ils se déplacent en translation
  vers un point sur la table, qui est tiré aléatoirement au début et à chaque
  fois le point est atteint. La classe comprend un générateur de nombre
  aléatoire initialisé par l'utilisateur avec une graine.
*******************************************************************************/

namespace gate {

class Foe{
public:
  typedef std::default_random_engine::result_type seed;

  /*
    Apparence
  */

  static sf::Color fill_color;
  static sf::Color outline_color;

  static pixel     goal_mark_radius_px;
  static sf::Color goal_mark_fill_color;
  static sf::Color goal_mark_outline_color;

  /**

  **/

  Foe(sf::Vector2<meter> pos_m, meter radius_m, seed& val)
    : hitbox_m{pos_m, radius_m},
      goal_m{pos_m},
      dir_cos{1.0},
      dir_sin{0.0},
      rnd_engine{val},
      sprite{scale_px_per_m * radius_m},
      goal_mark{goal_mark_radius_px} {
    sprite.setFillColor(fill_color);
    sprite.setOutlineColor(outline_color);

    goal_mark.setFillColor(goal_mark_fill_color);
    goal_mark.setOutlineColor(goal_mark_outline_color);
  }

  /**

  **/

  Outcome update(const sf::Rect<meter>& space,
                 const std::list<Robot>& candidate_l) {
    if (update_position(space)) set_new_goal(space);
    update_sprites();

    return Outcome::none;
  }

  /**

  **/

  Hitbox shape() const { return hitbox; }

  /**

  **/

  Kind tag() const { return Kind::foe; }

  /**

  **/

  void draw(sf::RenderTarget& rd_target, sf::RenderStates rd_states) const {
    rd_target.draw(sprite);
    rd_target.draw(goal_mark);
  }

  /**

  **/

  bool update_position(const sf::Rect<meter>& space){
    hitbox.pos.x += speed_m_per_s * dt_s * dir_cos;
    hitbox.pos.y += speed_m_per_s * dt_s * dir_sin;
    constrain_to(hitbox, space);

    return norm2_m(goal - hitbox.pos) < speed_m_per_s * dt_s;
  }

  /**

  **/

  void set_new_goal(const sf::Rect<meter>& space) {
    /*Tirer aléatoirement la position du nouveau but*/
    std::uniform_real_distribution<distance_m> x_distr(
        space.left + hitbox.radius_m,
        space.left + space.width - hitbox.radius_m
    );
    std::uniform_real_distribution<distance_m> y_distr(
        space.top + hitbox.radius_m,
        space.top + space.height - hitbox.radius_m
    );
    goal_m.x = x_distr(rnd_engine);
    goal_m.y = y_distr(rnd_engine);

    /*Orienter l'ennemie dans la direction du but*/
    sf::Vector2<meter> translation_m = goal_m - hitbox.pos_m;
    meter distance_m = norm2_m(translation_m);
    dir_cos = -translation_m.x / distance_m;
    dir_sin = -translation_m.y / distance_m;
  }

  /**

  **/

  void update_sprites() {
    Vector2 sprite_pos_m = hitbox.pos_m - hitbox.radius_m * (ux + uy);
    sprite.setPosition(scale_px_per_m * sprite_pos_m);

    Vector2 goal_pos_px = scale_px_per_m * goal_m
                        - goal_mark_radius_px * (ux + uy);
    goal_mark.setPosition(goal_pos_px);
  }

 private:
  Hitbox hitbox;
  sf::Vector2<meter> goal;
  angle_rad dir_cos;
  angle_rad dir_sin;
  std::default_random_engine rnd_engine;

  sf::CircleShape sprite;
  sf::CircleShape goal_mark;
};

}

#endif
