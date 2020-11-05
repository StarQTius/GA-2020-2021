/**
**/

template<typename FitnessFunctor, typename Iterator>
void gate::select(
  Iterator         it_begin,
  Iterator         it_end,
  FitnessFunctor&& fit_ftor)
{
  typedef typename std::decay<decltype(*it_begin)>::type           value;
  typedef typename std::decay<decltype(fit_ftor(*it_begin))>::type fitness;

  std::vector< std::pair<fitness, value> > fit_val_v;
  fit_val_v.reserve(std::distance(it_begin, it_end));

  /* Remplir fit_it_v avec les scores de fitness et les itérateurs
     correspondants */
  std::for_each(
    std::make_move_iterator(it_begin),
    std::make_move_iterator(it_end),
    [&fit_val_v, &fit_ftor](value&& val){
      fit_val_v.emplace_back(fit_ftor(val), std::move(val));
    }
  );

  /* Trier fit_it_v par ordre décroissant de score de fitness */
  std::sort(fit_val_v.begin(), fit_val_v.end(),
    [](const auto& lhs, const auto& rhs){
      return lhs.first > rhs.first;
    }
  );

  /* Remettre les valeurs dans l'intervalle délimité par les itérateurs passés
     en paramètre, dans l'ordre établie */
  auto it = it_begin;
  std::for_each(
    std::make_move_iterator(fit_val_v.begin()),
    std::make_move_iterator(fit_val_v.end()),
    [&it](auto&& fit_val){
      *it++ = std::move(fit_val.second);
    }
  );
}

/**
**/

template<
  typename Iterator,
  typename floating_point,
  typename RandomDistribution
>
void gate::mutate(
  Iterator             it_begin,
  Iterator             it_end,
  floating_point       rate,
  RandomDistribution&& rnd_mod)
{
  typedef typename std::decay<decltype(*it_begin)>::type value;

  auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::default_random_engine             rnd_engine{seed};
  std::uniform_real_distribution<double> uni_real_distr;

  /* Pour chaque élement de l'intervalle délimité par les itérateurs, avec une
     probabilité 'rate' de se produire, assigner une valeur aléatoire à
     l'élement */
  std::for_each(it_begin, it_end,
    [&uni_real_distr, &rnd_engine, rate, &rnd_mod](value& val){
      if(uni_real_distr(rnd_engine) < rate){
        val = rnd_mod(rnd_engine);
      }
    }
  );
}
