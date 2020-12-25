#ifndef GATE_NEURAL_NETWORK_HPP
  #define GATE_NEURAL_NETWORK_HPP

  #include <chrono>
  #include <exception>
  #include <functional>
  #include <list>
  #include <utility> //pair
  #include <vector>

  #include <boost/numeric/ublas/vector.hpp>
  #include <boost/numeric/ublas/matrix.hpp>
  using namespace boost::numeric;

  /**
    Ce fichier défini une structure NeuralNetwork qui va servir de base pour
    l'algorithme génétique. Afin de rendre le code facilement intégrable, les
    matrices de poids sont accessibles et modifiables. Il suffira juste
    d'encapsuler une instance de NeuralNetwork dans une classe pour avoir du
    code "OOP-friendly".
  **/

  namespace gate{

    /*
      Exceptions
    */

    struct empty_layer_size_list : std::exception {

      virtual const char* what() const noexcept {
        return "Provided list of layer sizes cannot be empty";
      }

    };

    /*
      Définitions
    */

    typedef size_t layer_size;
    typedef float  scalar;

    typedef ublas::vector<scalar>                          Vector;
    typedef ublas::matrix<scalar>                          WeightMatrix;
    typedef std::vector< std::pair<WeightMatrix, Vector> > NeuralMap;

    /*
      NeuralNetwork : structure principale du fichier
    */

    struct NeuralNetwork{

      template<
        typename RandomDistribution,
        typename ActivationFunctor,
        typename LastActivationFunctor
      >
      NeuralNetwork(
        const std::list<layer_size>&,
        RandomDistribution&&,
        ActivationFunctor&&,
        LastActivationFunctor&&);

      Vector operator()(const Vector&) const;

      NeuralMap                     neural_map;
      std::function<scalar(scalar)> act_ftor;
      std::function<scalar(scalar)> last_act_ftor;

    };

    /*
      Fonctions auxiliaires
    */

    template<typename RandomDistribution>
    WeightMatrix create_random_weight_matrix(
      layer_size,
      layer_size,
      RandomDistribution&&
    );

    template<typename RandomDistribution>
    Vector create_random_bias_vector(layer_size, RandomDistribution&&);

  }

  #include "neural_network.tcc"

#endif
