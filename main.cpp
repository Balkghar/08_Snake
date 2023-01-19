/*
  ---------------------------------------------------------------------------
  Fichier     : main.cpp
  Nom du labo : Labo8 - Snake
  Auteur(s)   : Delétraz Alexandre - Germano Hugo
  Date        : 10.01.2022
  But         : 

  Remarque(s) :

  Compilateur : gcc version 11.2.0
  ---------------------------------------------------------------------------
*/

#include <iostream>
#include <cstdlib>
#include <limits>

#include "jeu/combatSnakes.hpp"

using namespace std;

//-----------------------------------------------------------------------------
int main(int argv, char **args) {
// int argc, char* argv[] are important for SDL2 on Windows
  (void) argv;       // use these arguments
  (void) args;       // .. to avoid a warning

  Combat combat(100, 100, 20);

  combat.commencerCombat();
  return EXIT_SUCCESS;
}
