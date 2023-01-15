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
#include "../outils/aleatoire.hpp"
#include <iostream>

using namespace std;


Combat::Combat(unsigned int largeur, unsigned int longueur, unsigned int nbSerpent)
   : largeur(largeur), longueur(longueur), nbSerpent(nbSerpent) {

   initialiserSerpent();
   initialiserPomme();
}

void Combat::initialiserPomme() {

   pommes.reserve(nbSerpent);

   const unsigned min = 1;

   unsigned x;
   unsigned y;

   for ( unsigned i = 1; i <= Combat::nbSerpent; ++i ) {
      do {

         x = aleatoireEntreDeuxEntiersPositifs(min, largeur);
         y = aleatoireEntreDeuxEntiersPositifs(min, longueur);

      }while( placeEstOccupee(x, y) );
      pommes.emplace_back(x, y, i, false);

   }
}

void Combat::initialiserSerpent() {

   serpents.reserve(nbSerpent);

   const unsigned min = 1;


   unsigned x;
   unsigned y;

   for ( unsigned i = 1; i <= Combat::nbSerpent; ++i ) {
      do {

         x = aleatoireEntreDeuxEntiersPositifs(min, largeur);
         y = aleatoireEntreDeuxEntiersPositifs(min, longueur);

      }while( placeEstOccupee(x, y) );
      serpents.emplace_back(x, y, i, true);
   }

}

bool Combat::placeEstOccupee(unsigned int x, unsigned int y) {

   return (serpentPresent(x, y) or pommePresente(x, y));
}

bool Combat::serpentPresent(unsigned int x, unsigned int y) {
   for (Snake& monSerpent : serpents) {
      if (monSerpent.getCoordX() == x and monSerpent.getCoordY() == y) {
         return true;
      }
   }

   return false;
}

bool Combat::pommePresente(unsigned int x, unsigned int y) {
   for(Pomme& mesPommes : pommes) {
      if(mesPommes.getCoordX() == x and mesPommes.getCoordY() == y) {
         return true;
      }
   }

   return false;
}


void Combat::commencerCombat(){

   Affichage2d affichage(this->largeur, this->longueur, 50, 6);

   affichage.initalisationAffichage();
   
   affichage.nettoyerAffichage(Couleur::blanc);

   ajouterSerpentAffichage(affichage);
   
   ajouterPommeAffichage(affichage);


   affichage.mettreAjourAffichage();

}
void Combat::ajouterSerpentAffichage(Affichage2d& affichage){

   for(Snake& serpent : serpents){
      for(coordonneesXY& coord : serpent.getCoord()){

         affichage.ajouterElementAffichage(coord.x, coord.y, Couleur::noir);
         
      }
   }
}

void Combat::ajouterPommeAffichage(Affichage2d& affichage){
   for(Pomme& pomme : pommes){

      affichage.ajouterElementAffichage(pomme.getCoordX(), pomme.getCoordY(), Couleur::rouge);

   }
}