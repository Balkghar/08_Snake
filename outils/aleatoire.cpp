/*
  ---------------------------------------------------------------------------
  Fichier     : aleatoire.cpp
  Nom du labo : Labo8 - Robots
  Auteur(s)   : Delétraz Alexandre - Germano Hugo
  Date        : 11.01.2023
  But         : Définition de la fonction aleatoireEntreDeuxEntiersPositifs.

  Remarque(s) :

  Compilateur : gcc version 11.2.0
  ---------------------------------------------------------------------------
*/

#include <random>
#include "aleatoire.hpp"

using namespace std;

template <typename T>
T aleatoireEntreDeuxEntiersPositifs(T min, T max) {

   random_device                  rand_dev;
   default_random_engine          generator(rand_dev());
   uniform_int_distribution<int>  distr(min, max);

   return distr(generator);
}