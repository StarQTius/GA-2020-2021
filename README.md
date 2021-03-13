# GA-2020-2021

Le but de ce projet est de créer un système d'évitement d'obstacle mobile grâce à un algorithme génétique.
Pour l'instant, la version proposée sur ce dépôt est encore basique et certaines parties du code doivent être repensées.

Le projet est destiné au club INTech afin de préparer la coupe de France de robotique. Le but est de permettre au robot d'éviter les robots adversaires sans avoir à implémenter un algorithme de _path finding_.

Malheureusement, le programme ne peut pour l'instant qu'entraîner un robot pour qu'il atteigne un point sur la table de jeu sans robots adverses. Mais bien qu'il s'agisse d'un prototype, implémenter les adversaires dans la simulation sera une tâche relativement rapide à effectuer.

Pour l'instant, la préparation de la coupe de France ne me permet pas de continuer ce projet. Si vous voulez vous en servir ou vous en inspirer, je serais heureux de savoir ce que vous allez en faire, donc n'hésitez pas à me tenir courant. ;)

## Installation

Le code est écrit en C++20, et a été testé uniquement avec GCC.

Les dépendances suivantes sont requises :
- [entt](https://github.com/skypjack/entt) : Gaming meets modern C++ - a fast and reliable entity component system (ECS) and much more
- [SFML](https://www.sfml-dev.org/) : Simple and Fast Multimedia Library
- [Units](https://github.com/mpusz/units) : A compile-time enabled Modern C++ library that provides compile-time dimensional analysis and unit/quantity manipulation
- [Eigen 3.4](https://gitlab.com/libeigen/eigen/-/tree/3.4) : Eigen is a C++ template library for linear algebra: matrices, vectors, numerical solvers, and related algorithms

La bilbiothèque [Little-Type-Library](https://github.com/qnope/Little-Type-Library) est également utilisée, mais elle est incluse dans ce dépôt. Cette bibliothèque propose énormément de fonctionnalités utiles pour la métaprogrammation et la programmation fonctionnelle en C++17. Si vous voulez utiliser des ranges en C++17 ou que l'implémentation des ranges en C++20 ne vous convient pas, je vous encourage vivement à jeter un coup d’œil à cette bibliothèque.

Une fois les dépendances installées, clonez le dépôt, entrez dans le dossier GA-2020-2021 et construisez le projet avec le _makefile_ :
```    
git clone https://github.com/Stercutius/GA-2020-2021
cd GA-2020-2021
make release
```
...puis entrez la commande `./release` pour exécuter le programme.

## Présentation du programme
Lorsque vous lancez le programme, une population de 50 individus est créée. Dans le problème auquel on s'intéresse, un individu peut être assimilé à un robot. Chaque robot va se comporter différent une fois soumis à une épreuve sur la table de jeu.

### Les épreuves

Une épreuve se déroule comme ceci :
- Le robot est positionné de manière aléatoire sur la table de jeu de 5m x 5m, avec une orientation aléatoire
- Une position aléatoire sur la table de jeu est déterminée
- Le robot doit rejoindre la position avant le temps imparti
- A l'issue de l'épreuve, un score est attribué au robot :
  > S = -(T + Dmin)
  
  ...avec **T** le temps qui a fallu au robot pour rejoindre la position et **Dmin** la distance minimale à l'objectif atteinte par le robot.
- L'épreuve est répétée 10 fois, avec des positions et une orientation de départ différentes à chaque fois. Le score finale est égal à la somme des scores de chaque épreuve.

### L'évolution de la population

Les algorithmes génétiques s'inspire de l'évolution des espèces.

Au départ, chaque individu de la population est soumis à une suite de 10 épreuves. Ces 10 épreuves sont les mêmes pour chaque individu.
Une fois que chaque individu a obtenu un score, l'algorithme génétique suivant est appliqué :
- Les individus sont classés par ordre décroissant de score
- Les 15 premiers individus sont gardés en tant que parents de la génération suivante
- Via une méthode de _crossover uniform_, 35 individus supplémentaires sont créés
- Les caractéristiques de ces nouveaux individus sont mutées de manière aléatoire
- Les parents et les nouveaux individus sont réunis afin de constituer la nouvelle population
  
Ces étapes sont répétées avec la nouvelle population, et ainsi de suite jusqu'à la génération 100. Voici ce que la console devrait vous afficher :

```
vvvvvvvvvvvvvvv
00000000000070000000000000000000000050000000000000
BATCH 0 --- Best : -67.4164, -87.2682, -104.739
vvvvvvvvvvvvvvv
76100001000000100000000000070000000100000070101071
BATCH 1 --- Best : -57.4163, -57.4163, -57.4163

...
```

Chaque chiffre correspond au nombre d'épreuves réussies par chaque individu. Le terme "BATCH" désigne la génération. Les flèches vers le bas indiquent les parents de la génération actuelle (ces flèches n'ont cependant pas de sens pour la première génération et désigne des individus complètement quelque conque). Les meilleurs scores sont affichés pour chaque génération.

Entre chaque génération, la simulation d'une épreuve réalisée par le meilleur individu de la génération est affichée.

### Données statistiques

A la fin de l'exécution du programme, un fichier `log.txt` est créé dans le répertoire du programme. Ce fichier correspond à la réalisation de 10 000 épreuves par le meilleur individu de la dernière génération.

A la fin de ce fichier, vous pourrez voir la synthèse des résultats de ces épreuves :

```
...

RESULTATS
Succès : 96%
Temps moyen : 2.80075
Proximité moyenne : 0.0752873
Pire proximité : 0.120537
```

Temps moyen correspond au temps nécessaire qu'il faut au robot pour atteindre l'objectif, en secondes. Les échecs sont comptés dans ce temps, et dans ce cas, le temps considéré est le temps de l'épreuve. Cette donnée n'est pas très intéressante en soit, et permet simplement de détecter un problème majeur dans l'entraînement.

En revanche, proximité moyenne est pire proximité est beaucoup plus intéressant. Ces indicateurs permettent d'évaluer la fiabilité de l'algorithme.

La proximité moyenne est égale à la distance minimale à l'objectif atteinte pas le robot, en mètre. La pire proximité est égale à la plus grande des distances minimales à l'objectif atteint par le robot, en mètre également.

Avec ces valeurs, il est possible de savoir avec quelle tolérance on peut considérer que l'objectif est atteint. Dans cette exemple, il est judicieux de laisser le robot rejoindre l'objectif jusqu'à ce qu'il soit à une distance de l'objectif de moins de 12cm. A ce moment, on reprend la main afin de se positionner plus précisément.

## Une session d'entraînement en image

Quelques images de simulations, afin que vous puissiez vous faire une idée un peu plus claire du projet :
![Génération 1](https://github.com/Stercutius/GA-2020-2021/blob/main/doc/first_gen.gif)

_Génération 1 : le comportement des individus est très loin de ce qu'on cherche à obtenir_

![Génération 59](https://github.com/Stercutius/GA-2020-2021/blob/main/doc/gen_59.gif)

_Génération 59 : le comportement global des individus répond assez bien aux exigences_

Le comportement des individus continue de s'améliorer dans les générations, mais plus lentement. Les individus vont bloquer sur des cas particuliers.

Même si cela semble encourageant, les statistiques mettent en évidence que les individus ne sont pas aussi performants que l'on peut croire :
```
...
vvvvvvvvvvvvvvv
@@@@@@@@@@@@@@@@@@@@@@7@@@@@@@@@@@@@@@@@@@@@@@@@@@
BATCH 90 --- Best : -27.2412, -27.3768, -27.3768
vvvvvvvvvvvvvvv
99999999999999999999999999999999999999999999999999
BATCH 91 --- Best : -34.5357, -34.5429, -34.5429
vvvvvvvvvvvvvvv
99999999999999999999999999999999999999999999999999
BATCH 92 --- Best : -30.456, -30.456, -30.4853
vvvvvvvvvvvvvvv
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
BATCH 93 --- Best : -29.7929, -29.7929, -29.7929
vvvvvvvvvvvvvvv
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
BATCH 94 --- Best : -25.8496, -25.8496, -25.8496
vvvvvvvvvvvvvvv
99999999999999999999999999999999999999999999999999
BATCH 95 --- Best : -27.8747, -27.8747, -27.8747
vvvvvvvvvvvvvvv
99999999999999999099999999999199999999999999999999
BATCH 96 --- Best : -33.9623, -33.9664, -33.9664
vvvvvvvvvvvvvvv
99999999999999999999999999999999999999999999999999
BATCH 97 --- Best : -41.1321, -41.1321, -41.1377
vvvvvvvvvvvvvvv
99999999999999999999999999999999999999999999299999
BATCH 98 --- Best : -33.3411, -33.3411, -33.3411
vvvvvvvvvvvvvvv
88888888888888888888888888888888888888888888888888
BATCH 99 --- Best : -38.1944, -38.1944, -38.1944
```
Il y a des cas limites que les individus ne semblent pas pouvoir traiter. Il serait intéressant de pouvoir visualiser ces cas. Je compte implémenter une telle fonctionnalité si je trouve le temps de remettre les mains dans le projet.
