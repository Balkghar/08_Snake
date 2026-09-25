/*
  ---------------------------------------------------------------------------
  Fichier     : aleatoire.cpp
  Nom du labo : Labo8 - Snake
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

namespace {

mt19937 &generateur() {
  // Générateur initialisé une seule fois : random_device est lent (appel
  // système) et n'est pas fait pour être tiré à chaque nombre
  static mt19937 generator(random_device{}());
  return generator;
}

}  // namespace

int aleatoireEntreDeuxValeurs(int min, int max) {
  uniform_int_distribution<int> distr(min, max);
  return distr(generateur());
}

uint64_t graineAleatoire() {
  return (uint64_t(generateur()()) << 32) | generateur()();
}