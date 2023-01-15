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
  
  Combat combat(200, 150, 40);

  combat.commencerCombat();
  

  cin.ignore(numeric_limits<streamsize>::max(), '\n');


  return EXIT_SUCCESS;
}
