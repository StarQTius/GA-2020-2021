#pragma once

#include "../trial.hpp" //TODO : restructurer
#include "ltl/Range/Value.h"

/*******************************************************************************
  Lorsque l'entraînement est terminé, il est nécessaire de mesurer les performances
  du réseau de neurone. La fonction 'measure_accuracy' soumet le candidat déterminé
  par 'candidate_features' à une série d'épreuves. La fonction s'occupe de fournir
  une seed pour chaque épreuve. La fonction envoie à 'out_stream' les résultats
  de la mesure.
*******************************************************************************/

template<typename C>
void measure_accuracy(int trials_nb, TrialParameters& parameters,
                      const RobotFeatures<auto, auto, auto>& candidate_features,
                      const RobotFeatures<auto, auto, auto>& foe_features,
                      std::basic_ostream<C>& out_stream) {
  physics::time average_time = 0_q_s;
  physics::length average_closeness = 0_q_m;
  physics::length worst_closeness = 0_q_m;
  size_t success_nb = 0;

  out_stream << "Série de " << trials_nb << " épreuves" << std::endl;
  out_stream << std::endl;

  for (auto i : ltl::valueRange(0, trials_nb)) {
    parameters.seed = generate_seed();
    out_stream << "Epreuve " << i << " / " << trials_nb << " --- ";

    auto results = perform_trial(parameters, candidate_features, foe_features);
    out_stream << "Issue : " << (results.outcome == Outcome::goal_reached ? "SUCCES" : "ECHEC") << " - ";
    if (results.outcome == Outcome::goal_reached) {
      success_nb++;
      average_time += (results.time - average_time) / success_nb;

      out_stream << "Temps : " << results.time.count();
    } else {
      average_closeness += (results.best_distance - average_closeness) / (i + 1 - success_nb);
      worst_closeness = max(results.best_distance, worst_closeness);

      out_stream << "Proximité : " << results.best_distance.count();
    }

    out_stream << std::endl;
  }

  out_stream << std::endl
             << "RESULTATS" << std::endl
             << "Succès : " << success_nb * 100 / trials_nb << "%" << std::endl
             << "Temps moyen : " << average_time.count() << std::endl
             << "Proximité moyenne : " << average_closeness.count() << std::endl
             << "Pire proximité : " << worst_closeness.count() << std::endl;
}
