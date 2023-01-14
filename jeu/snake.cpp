/*
  ---------------------------------------------------------------------------
  Fichier     : snake.cpp
  Nom du labo : Labo8 - Snake
  Auteur(s)   : Delétraz Alexandre - Germano Hugo
  Date        : 10.01.2022
  But         : Définition des fonctions membres de la classe snake.

  Remarque(s) : 

  Compilateur : gcc version 11.2.0
  ---------------------------------------------------------------------------
*/
#include <vector>

#include "snake.hpp"

using namespace std;

Snake::Snake(unsigned x,
             unsigned y,
             const unsigned id,
             bool estEnVie) : id(id), estEnVie(estEnVie) {
   coordonnee.at(0).at(0) = x;
   coordonnee.at(0).at(1) = y;
   longueurAAjouter = longueur;

}


void Snake::setCoordX(unsigned x) {
   coordonnee.at(0).at(0) = x;

}

void Snake::setCoordY(unsigned y) {
   coordonnee.at(0).at(1) = y;
}

unsigned Snake::getCoordX() {
   return this->coordonnee.at(getId()).at(0);
}

unsigned Snake::getCoordY() {
   return this->coordonnee.at(getId()).at(1);

}

unsigned Snake::getId() {
   return id;
}

// TODO
void Snake::deplacerVersPomme(unsigned int xPomme, unsigned int yPomme,
                              const unsigned idPomme,
                              unsigned largeur,
                              unsigned hauteur,
                              vector<Snake>& serpents) {
   for(Snake& monSerpent : serpents) {
      if(monSerpent.id == idPomme) {
         do {
            if(monSerpent.getCoordX() < xPomme){
               monSerpent.coordonnee.at(getId()).at(0) += 1;
            }
            else if(monSerpent.getCoordX() > xPomme) {
               monSerpent.coordonnee.at(getId()).at(0) -= 1;
            }
            if(monSerpent.getCoordY() < yPomme) {
               monSerpent.coordonnee.at(getId()).at(1) += 1;
            }
            else if(monSerpent.getCoordY() > yPomme) {
               monSerpent.coordonnee.at(getId()).at(1) -= 1;
            }
         }while(peutSeDeplacer(monSerpent.coordonnee.at(getId()).at(0),
                               monSerpent.coordonnee.at(getId()).at(1),
                               largeur, hauteur));

      }

   }

}

bool Snake::peutSeDeplacer(unsigned int x, unsigned int y, unsigned largeur,
                           unsigned hauteur) const {
   if(x > largeur or y > hauteur or x == 0 or y == 0) {
      return true;
   }

   return false;
}