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
#include "snake.hpp"
#include <vector>

Snake::Snake(unsigned x,unsigned y, const unsigned id, bool estEnVie) : id(id), estEnVie(estEnVie) {

   coordonnee.resize(longueur);


   coordonnee.at(0).x = x;
   coordonnee.at(0).y = y;


   longueurAAjouter = longueur;


}

void Snake::deplacerSerpent() {

}

void Snake::setCoordX(unsigned x) {
   coordonnee.at(0).x = x;

}

void Snake::setCoordY(unsigned y) {
   coordonnee.at(0).y = y;
}

unsigned Snake::getCoordX() {
   return this->coordonnee.at(0).x;
}

unsigned Snake::getCoordY() {
   return this->coordonnee.at(0).y;

}

// TODO
void Snake::deplacerVersPomme(unsigned int x, unsigned int y) {

}

// TODO
bool Snake::peutSeDeplacer(unsigned int x, unsigned int y) const {
   return false;
}

unsigned Snake::getId() {
   return id;
}
