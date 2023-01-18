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


Combat::Combat(unsigned largeur, unsigned longueur, unsigned nbSerpent)
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
      pommes.emplace_back(x, y, i, true);

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
      serpents.emplace_back(x, y, i, true,10);
   }

}

bool Combat::placeEstOccupee(unsigned x, unsigned y) {
   return (serpentPresent(x, y) or pommePresente(x, y));
}

bool Combat::serpentPresent(unsigned int x, unsigned int y) {
   for (Snake& serpent : serpents) {
      for(coordonneesXY& coord : serpent.getCoord()){
         if (coord.x == x and coord.y == y) {
            return true;
         }
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
   
   combatSerpents(affichage);

   affichage.fermerAffichage();

}
void Combat::combatSerpents(Affichage2d& affichage){

   do{

      afficher(affichage);

      for(size_t d = 0; d < serpents.size(); ++d){
         if(serpents.at(d).getEstEnVie()){
            serpents.at(d).deplacerVersXY(pommes.at(d).getCoordX(), pommes.at(d).getCoordY());
            mangerPomme(serpents.at(d), pommes.at(d));
            combatSerpent(serpents.at(d));
         }
         else{
            if(pommes.at(d).getEstMangee()){
               pommes.at(d).pommeEstMangee();
            }
         }
      }
   }while(nbSerpent > 1);
}

void Combat::combatSerpent(Snake& serpent){
   for(int i = 0; i < serpents.size(); ++i){
      if(serpents.at(i).getId() != serpent.getId() && serpent.getEstEnVie() && serpents.at(i).getEstEnVie()){
         if(serpent.combattreSerpent(serpents.at(i))){
            --nbSerpent;
         }
      }
   }
}

void Combat::afficher(Affichage2d& affichage){

   affichage.nettoyerAffichage(Couleur::blanc);

   ajouterSerpentAffichage(affichage);

   ajouterPommeAffichage(affichage);

   affichage.mettreAjourAffichage();

}
void Combat::mangerPomme(Snake& serpent, Pomme& pomme){

   const unsigned min = 1;

   unsigned x,y;

   if(serpent.getCoordX() == pomme.getCoordX() && serpent.getCoordY() == pomme.getCoordY()){

      do {

         x = aleatoireEntreDeuxEntiersPositifs(min, largeur);
         y = aleatoireEntreDeuxEntiersPositifs(min, longueur);

      }while( placeEstOccupee(x, y) );
      serpent.longueurAAjouterSupl(pomme.getValeur());
      pomme.nouvellePomme(x,y);
   }
}

void Combat::ajouterSerpentAffichage(Affichage2d& affichage){

   for(Snake& serpent : serpents){
      for(coordonneesXY& coord : serpent.getCoord()){
         if(serpent.getEstEnVie()){
            affichage.ajouterElementAffichage(coord.x, coord.y, Couleur::noir);
         }
      }
   }
}

void Combat::ajouterPommeAffichage(Affichage2d& affichage){
   for(Pomme& pomme : pommes){
      if(pomme.getEstMangee()){
         affichage.ajouterElementAffichage(pomme.getCoordX(), pomme.getCoordY(), Couleur::rouge);
      }
   }
}