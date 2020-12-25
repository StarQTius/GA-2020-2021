#ifndef GATE_ENTITY_HPP
  #define GATE_ENTITY_HPP

  #include <exception>
  #include <list>
  #include <memory>
  #include <utility>

  #include <SFML/Graphics.hpp>

  #include "forward_to_member.hpp"

  /*****************************************************************************

  *****************************************************************************/

  namespace gate{

    /*
      Exception
    */

    struct not_in_space : std::exception {

      virtual const char* what() const noexcept {

        return "Given shape is not contained in space";

      }

    };

    /*
      Entity : classe d'emballage pour l'effacement de type
      La classe emballée doit être de la forme suivante :
        class {
          Event update(sf::Rect, std::list<Entity>)
          Shape shape() const
          Tag tag() const
          void draw(sf::RenderTarget&, sf::RenderStates) const
        }
    */

    template<
      typename length,
      class    Shape,
      class    Event = void,
      class    Tag   = void
    >
    class Entity : public sf::Drawable {

    public:

      /*
        Interface avec l'objet emballé
      */

      template<class T>
      Entity(T&& content):
        concept_uptr( new Model<T>( std::forward<T>(content) ) )
      {}

      FORWARD_TO_MEMBER(*concept_uptr, update, )
      FORWARD_TO_MEMBER(*concept_uptr, shape, const)
      FORWARD_TO_MEMBER(*concept_uptr, tag, const)

      /* Membres virtuels */

      virtual void draw(
        sf::RenderTarget& render_target,
        sf::RenderStates  render_states) const
      {

        concept_uptr->draw(render_target, render_states);

      }

    private:

      /*
        Concept : classe interface
      */

      struct Concept{

        virtual Event update(
          const sf::Rect<length>&,
          const std::list<Entity>&) = 0;
        virtual Shape shape() const = 0;
        virtual Tag tag() const = 0;
        virtual void draw(sf::RenderTarget&, sf::RenderStates) const = 0;

        virtual ~Concept() = default;

      };

      /*
        Model : classe d'implementation
      */

      template<class T>
      struct Model : Concept {

        template<typename S>
        Model(S&& content): content( std::forward<S>(content) ) {}

        /**
          Met à jour l'état de l'entité :
            -space : espace dans lequel l'entité doit être contenu
            -entity_l : liste des autres entités à considérer
          +retourne un Event donné par l'objet emballé
        **/

        virtual Event update(
          const sf::Rect<length>&  space,
          const std::list<Entity>& entity_l)
        {

          content.update(space, entity_l);
          /*if(!content.shape().is_in(space))
            throw not_in_space*/

        }

        /**
          Donne accès à la forme de l'entité :
          +retourne le Shape de l'objet emballé
        **/

        virtual Shape shape() const {

          return content.shape();

        }

        /**
          Donne accès à l'étiquette de l'entité :
          +retourne le Tag de l'objet emballé
        **/

        virtual Tag tag() const {

          return content.tag();

        }

        /**

        **/

        virtual void draw(
          sf::RenderTarget& render_target,
          sf::RenderStates  render_states) const
        {

          content.draw(render_target, render_states);

        }

        T content;

      };

      /*
        Variables membres
      */

      std::shared_ptr<Concept> concept_uptr;

    };

  }

#endif
