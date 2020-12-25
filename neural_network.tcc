/**
    Constructeur public de gate::NeuralNetwork
    Construit une instance de NeuralNetwork selon la forme du réseau. On
    donnera le nombre de noeuds pour chacune des couches. Les poids et les biais
    sont initialisés aléatoirement selon la distribution fournié :
      -layer_sz_l : liste du nombre de noeuds pour chaque couche (dans l'ordre
      de proximité avec la couche d'entrée)
      -rnd_distr : distribution aléatoire utilisée
      -act_ftor : fonction d'activation (qui peut être foncteur)
**/

template<
  typename RandomDistribution,
  typename ActivationFunctor,
  typename LastActivationFunctor
>
gate::NeuralNetwork::NeuralNetwork(
  const std::list<gate::layer_size>& layer_sz_l,
  RandomDistribution&&               rnd_distr,
  ActivationFunctor&&                act_ftor,
  LastActivationFunctor&&            last_act_ftor):
  act_ftor{std::forward<ActivationFunctor>(act_ftor)},
  last_act_ftor{std::forward<LastActivationFunctor>(last_act_ftor)}
{
  if(layer_sz_l.empty())
    throw empty_layer_size_list{};

  neural_map.reserve(layer_sz_l.size() - 1);

  auto it_fst = layer_sz_l.begin();
  auto it_snd = it_fst; it_snd++;
  while(it_snd != layer_sz_l.end()){
    neural_map.push_back({
      create_random_weight_matrix(*it_snd, *it_fst, rnd_distr),
      create_random_bias_vector(*it_snd, rnd_distr)
    });
    it_fst++; it_snd++;
  }
}

/**
  Fonction membre de gate::NeuralNetwork
  Connaissant les valeurs des noeuds sur la couche d'entrée, calcule les valeurs
  des noeuds sur la couche de sortie :
    -input_vect : représente la valeur des noeuds sur la couche d'entrée
  +retourne le vecteur représentant la couche de sortie
**/

gate::Vector gate::NeuralNetwork
::operator()(const Vector& input_vect) const
{
  Vector output_vect = input_vect;
  for(auto it = neural_map.begin(); it != neural_map.end(); it++){
    output_vect = prod(it->first, output_vect) + it->second;
    std::transform(
      output_vect.begin(),
      output_vect.end(),
      output_vect.begin(),
      it == neural_map.end() - 1 ? last_act_ftor : act_ftor
    );
  }

  return output_vect;
}

/**
  Fonction de gate
  Crée une matrice de composantes aléatoires suivants une distribution
  aléatoire :
    -wmat_sz_x, wmat_sz_y : dimensions de la matrice
    -rnd_distr : distribution aléatoire utilisée
  +retourne la matrice attendue
**/

template<typename RandomDistribution>
gate::WeightMatrix gate::create_random_weight_matrix(
  layer_size           wmat_sz_x,
  layer_size           wmat_sz_y,
  RandomDistribution&& rnd_distr)
{
  auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine rnd_engine{seed};

  WeightMatrix wmat{wmat_sz_x, wmat_sz_y};
  for(auto it1 = wmat.begin1(); it1 != wmat.end1(); it1++)
    for(auto it = it1.begin(); it != it1.end(); it++)
      *it = rnd_distr(rnd_engine);

  return wmat;
}

/**

**/

template<typename RandomDistribution>
gate::Vector gate::create_random_bias_vector(
  layer_size           vect_sz,
  RandomDistribution&& rnd_distr)
{
  auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine rnd_engine{seed};

  Vector ret_vect{vect_sz};
  for(auto it = ret_vect.begin(); it != ret_vect.end(); it++)
    *it = rnd_distr(rnd_engine);

  return ret_vect;
}
