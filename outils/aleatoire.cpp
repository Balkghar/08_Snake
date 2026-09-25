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

uint64_t &graine() {
  static uint64_t valeur = (uint64_t(random_device{}()) << 32) | random_device{}();
  return valeur;
}

mt19937 &generateur() {
  // Générateur initialisé une seule fois : random_device est lent (appel
  // système) et n'est pas fait pour être tiré à chaque nombre
  static mt19937 generator = [] {
    seed_seq suite{uint32_t(graine()), uint32_t(graine() >> 32)};
    return mt19937(suite);
  }();
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

void fixerGraine(uint64_t valeur) {
  graine() = valeur;
  seed_seq suite{uint32_t(valeur), uint32_t(valeur >> 32)};
  generateur().seed(suite);
}

uint64_t graineDePartie() {
  return graine();
}
