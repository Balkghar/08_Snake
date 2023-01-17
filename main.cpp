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
int main() {
  
  Combat combat(100, 100, 200);

  combat.commencerCombat();
  return EXIT_SUCCESS;
}
