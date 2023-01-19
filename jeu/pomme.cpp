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
  coordonnees.x = x;
  coordonnees.y = y;
  valeur = 1;

}

int Pomme::getCoordX() {
  return coordonnees.x;
}

int Pomme::getCoordY() {
  return coordonnees.y;
}

unsigned Pomme::getValeur() {
  return valeur;
}

void Pomme::nouvellePomme(int x, int y) {
  coordonnees.x = x;
  coordonnees.y = y;

}

void Pomme::pommeEstMangee() {
  this->estMangee = false;
}

unsigned Pomme::getId() {
  return id;
}
bool Pomme::estIntacte() {
  return this->estMangee;
}
