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

Pomme::Pomme(int x,
             int y,
             const unsigned id,
             bool estMangee,
             std::array<short, 4> coul
) : id(id), estMangee(estMangee), couleurs(coul) {
  coordonnes.at(0) = x;
  coordonnes.at(1) = y;
  valeur = 1;

}

int Pomme::getCoordX() {
  return coordonnes.at(0);
}

int Pomme::getCoordY() {
  return coordonnes.at(1);
}

unsigned Pomme::getValeur() const {
  return valeur;
}

// TODO
void Pomme::nouvellePomme(int x,
                          int y
) {
  coordonnes.at(0) = x;
  coordonnes.at(1) = y;

}

unsigned Pomme::getId() const {
  return id;
}
