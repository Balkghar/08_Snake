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

//--------------------------- Constructeur --------------------------------

AreneDeCombat::AreneDeCombat(unsigned int largeur,
                             unsigned int longueur,
                             unsigned int nbSerpent)
   : largeur(largeur), hauteur(longueur), nbSerpent(nbSerpent) {
   initialiserSerpent();
   initialiserPommes();

}

//--------------------------- Méthodes d'initialisation --------------------

void AreneDeCombat::initialiserPommes() {

   pommes.reserve(nbSerpent);

   const unsigned min = 1;

   unsigned x;
   unsigned y;

   for (unsigned i = 1; i <= AreneDeCombat::nbSerpent; ++i ) {
      do {

         x = aleatoireEntreDeuxValeurs(min, largeur);
         y = aleatoireEntreDeuxValeurs(min, hauteur);

      }while(placeEstOccupee(x, y) );
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

         x = aleatoireEntreDeuxValeurs(min, largeur);
         y = aleatoireEntreDeuxValeurs(min, hauteur);

      }while(placeEstOccupee(x, y) );
      serpents.emplace_back(x, y, i, true);
   }

}

bool AreneDeCombat::placeEstOccupee(unsigned int x, unsigned int y) {

   return (serpentPresent(x, y) or pommePresente(x, y));
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

void AreneDeCombat::nouvellePomme(Pomme pommeMangee) {
   const unsigned min = 1;
   do {

      pommeMangee.setX(aleatoireEntreDeuxValeurs(min, largeur));
      pommeMangee.setY(aleatoireEntreDeuxValeurs(min, hauteur));

   }while(placeEstOccupee(pommeMangee.getCoordX(), pommeMangee.getCoordY()) );

}

//----------------------- Méthodes pour le déplacement --------------------

coordonneesXY AreneDeCombat::bonnePomme(const std::vector<Snake>& serpents,
                                        const std::vector<Pomme>& pommes) {
   for(const Snake& monSerpent : serpents) {
      for(Pomme maPommes : pommes) {
         if(monSerpent.getId() == maPommes.getId()) {
            return maPommes.getCoordonnes();
         }
      }
   }

}

void AreneDeCombat::tourDeJeu() {

   for(Snake& monSerpent : serpents) {
      monSerpent.deplacerVersObjet(bonnePomme(serpents, pommes),
                                   largeur,
                                   hauteur);
      if(serpentPresent(monSerpent.getCoordX(),monSerpent.getCoordY())) {
         //utilisé monSerpent.tuer();

      }
      if(pommePresente(monSerpent.getCoordX(),monSerpent.getCoordY())) {
         for(Pomme maPomme : pommes) {
            if(maPomme.getId() == monSerpent.getId()) {
               monSerpent.mangerPomme();
               nouvellePomme(maPomme);
            }
         }
      }
   }
}



//--------------------------- Méthode pour l'affichage --------------------


void AreneDeCombat::commencerCombat() {

   Affichage2d affichage(this->largeur, this->hauteur, 50, 6);

   affichage.initalisationAffichage();

   affichage.nettoyerAffichage(Couleur::blanc);

   ajouterSerpentAffichage(affichage);
   ajouterPommeAffichage(affichage);




   affichage.mettreAjourAffichage();

}
void AreneDeCombat::ajouterSerpentAffichage(Affichage2d& affichage) {

   for(Snake serpent : serpents) {
      affichage.ajouterElementAffichage(serpent.getCoordX(), serpent.getCoordY(), Couleur::noir);
   }

}

void AreneDeCombat::ajouterPommeAffichage(Affichage2d& affichage) {
   for(Pomme pomme : pommes) {

      affichage.ajouterElementAffichage(pomme.getCoordX(), pomme.getCoordY(), Couleur::rouge);

   }
}


