#pragma once

/*******************************************************************************
  gate::Outcome : énumère les issues possibles d'une simulation.
*******************************************************************************/

enum class Outcome {
  none,
  candidate_collided,
  foe_collided,
  goal_reached
};
