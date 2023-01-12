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

Snake::Snake(unsigned x, unsigned y,
             const int id, bool estEnVie) :id(id), estEnVie(estEnVie) {
   setCoordX(x);
   setCoordY(y);
   longueurAAjouter = longueur;


}

void Snake::deplacerSerpent() {

}

void Snake::setCoordX(unsigned x) {
   coordonnee.at(0).at(0) = x;

}

void Snake::setCoordY(unsigned y) {
   coordonnee.at(0).at(1) = y;
}

unsigned Snake::getCoordX() {
   return this->coordonnee.at(0).at(0);
}

unsigned Snake::getCoordY() {
   return this->coordonnee.at(0).at(1);

}
