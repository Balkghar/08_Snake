/*
---------------------------------------------------------------------------
Fichier     : combatSnakes.cpp
Nom du labo : Labo8 - Snake
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 10.01.2022
But         : Définition des fonctions membres de la classe
              combat.

Remarque(s) :

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#include "combatSnakes.hpp"
#include "snake.hpp"
#include "pomme.hpp"
#include "outils/aleatoire.hpp"
#include "outils/affichage2d.hpp"


Combat::Combat(unsigned int largeur, unsigned int longueur, unsigned int nbSerpent)
   : largeur(largeur), longueur(longueur), nbSerpent(nbSerpent) {
   initialiserSerpent();
   initialiserPomme();

}

void Combat::initialiserPomme() {
   const unsigned min = 1;

   unsigned x;
   unsigned y;

   for ( unsigned i = 1; i <= Combat::nbSerpent; ++i ) {
      do {

         x = aleatoireEntreDeuxEntiersPositifs(min, largeur);
         y = aleatoireEntreDeuxEntiersPositifs(min, longueur);

      }while( placeEstLibre(x, y) );
      pommes.emplace_back(x, y, i, false);

   }
}

void Combat::initialiserSerpent() {
   const unsigned min = 1;

   unsigned x;
   unsigned y;

   for ( unsigned i = 1; i <= Combat::nbSerpent; ++i ) {
      do {

         x = aleatoireEntreDeuxEntiersPositifs(min, largeur);
         y = aleatoireEntreDeuxEntiersPositifs(min, longueur);

      }while( placeEstLibre(x, y) );
      serpents.emplace_back(x, y, i, true);
   }

}

bool Combat::placeEstLibre(unsigned int x, unsigned int y) {

   return !serpentPresent(x, y) and !pommePresente(x, y);
}

bool Combat::serpentPresent(unsigned int x, unsigned int y) {
   for (Snake monSerpent: serpents) {
      if (monSerpent.getCoordX() == x and monSerpent.getCoordY() == y) {
         return true;
      }
   }

   return false;
}

bool Combat::pommePresente(unsigned int x, unsigned int y) {
   for(Pomme mesPommes : pommes) {
      if(mesPommes.getCoordX() == x and mesPommes.getCoordY() == y) {
         return true;
      }
   }

   return false;
}