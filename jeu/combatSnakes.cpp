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

using namespace std;

AreneDeCombat::AreneDeCombat(unsigned int largeur, unsigned int longueur, unsigned int nbSerpent)
   : largeur(largeur), longueur(longueur), nbSerpent(nbSerpent) {
   initialiserSerpent();
   initialiserPomme();

}

void AreneDeCombat::initialiserPomme() {

   pommes.reserve(nbSerpent);

   const unsigned min = 1;

   unsigned x;
   unsigned y;

   for (unsigned i = 1; i <= AreneDeCombat::nbSerpent; ++i ) {
      do {

         x = aleatoireEntreDeuxEntiersPositifs(min, largeur);
         y = aleatoireEntreDeuxEntiersPositifs(min, longueur);

      }while( placeEstLibre(x, y) );
      pommes.emplace_back(x, y, i, false);

   }
}

void AreneDeCombat::initialiserSerpent() {

   serpents.reserve(nbSerpent);

   const unsigned min = 1;

   unsigned x;
   unsigned y;

   for (unsigned i = 1; i <= AreneDeCombat::nbSerpent; ++i ) {
      do {

         x = aleatoireEntreDeuxEntiersPositifs(min, largeur);
         y = aleatoireEntreDeuxEntiersPositifs(min, longueur);

      }while( placeEstLibre(x, y) );
      serpents.emplace_back(x, y, i, true);
   }

}

bool AreneDeCombat::placeEstLibre(unsigned int x, unsigned int y) {

//   return !serpentPresent(x, y) and !pommePresente(x, y);
   return (serpentPresent(x, y) or pommePresente(x, y)); /// pas d'accord avec ça
}

bool AreneDeCombat::serpentPresent(unsigned int x, unsigned int y) {
   for (Snake monSerpent: serpents) {
      if (monSerpent.getCoordX() == x and monSerpent.getCoordY() == y) {
         return true;
      }
   }

   return false;
}

bool AreneDeCombat::pommePresente(unsigned int x, unsigned int y) {
   for(Pomme mesPommes : pommes) {
      if(mesPommes.getCoordX() == x and mesPommes.getCoordY() == y) {
         return true;
      }
   }

   return false;
}

void AreneDeCombat::commencerCombat(){

   Affichage2d affichage(this->largeur, this->longueur, 50, 6);

   affichage.initalisationAffichage();

   affichage.nettoyerAffichage(Couleur::blanc);

   ajouterSerpentAffichage(affichage);
   ajouterPommeAffichage(affichage);


   affichage.mettreAjourAffichage();

}
void AreneDeCombat::ajouterSerpentAffichage(Affichage2d& affichage){

   for(Snake serpent : serpents){
      affichage.ajouterElementAffichage(serpent.getCoordX(), serpent.getCoordY(), Couleur::noir);
   }

}

void AreneDeCombat::ajouterPommeAffichage(Affichage2d& affichage){
   for(Pomme pomme : pommes){

      affichage.ajouterElementAffichage(pomme.getCoordX(), pomme.getCoordY(), Couleur::rouge);

   }
}