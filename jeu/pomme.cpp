/*
---------------------------------------------------------------------------
Fichier     : pomme.cpp
Nom du labo : Labo8 - Snake
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 10.01.2022
But         : Définition des fonctions membres de la classe pomme.

Remarque(s) :

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#include "pomme.hpp"

Pomme::Pomme(unsigned x,
             unsigned y,
             const unsigned id,
             bool estMangee,
             std::array<short, 4> coul) : id(id), estMangee(estMangee),couleurs(coul){
   coordonnes.x = x;
   coordonnes.y = y;

}

unsigned Pomme::getCoordX() {
   return coordonnes.x;
}

unsigned Pomme::getCoordY() {
   return coordonnes.y;
}

unsigned Pomme::getId() {
   return id;
}

coordonneesXY Pomme::getCoordonnes() {
   return coordonnes;
}

void Pomme::setX(unsigned x) {
   this->coordonnes.x = x;
}

void Pomme::setY(unsigned y) {
   this->coordonnes.y = y;
}
