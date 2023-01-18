/*
  ---------------------------------------------------------------------------
  Fichier     : aleatoire.cpp
  Nom du labo : Labo8 - Robots
  Auteur(s)   : Delétraz Alexandre - Germano Hugo
  Date        : 11.01.2023
  But         : Définition de la fonction aleatoireEntreDeuxValeurs.

  Remarque(s) :

  Compilateur : gcc version 11.2.0
  ---------------------------------------------------------------------------
*/

#include <random>
#include "aleatoire.hpp"

using namespace std;

int aleatoireEntreDeuxValeurs(int min, int max) {

  random_device rand_dev;
  default_random_engine generator(rand_dev());
  uniform_int_distribution<int> distr(min, max);

  return distr(generator);
}