#ifndef GATE_PLAYGROUND_NEURAL_HPP
#define GATE_PLAYGROUND_NEURAL_HPP

/*******************************************************************************
  neural.hpp : définit les constantes et les fonctions utiles pour utiliser le
  réseau de neurone pour la simulation.
*******************************************************************************/

namespace gate {

/*
  Constantes
*/

const size_t input_vect_sz = 2 * foe_nb + 7;
const size_t output_vect_sz = 3;
const scalar is_moving_threshold = 0.5;

/*
  Fonctions d'activations
*/

scalar relu(scalar x) { return std::max((scalar) 0, x); }
scalar sigmoid(scalar x) { return 1.0 / (1.0 + std::exp(-x)); }

}

#endif
