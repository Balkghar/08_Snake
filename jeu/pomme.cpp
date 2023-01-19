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
#include "../outils/aleatoire.hpp"

//=========================== Partie public ===============================

//--------------------------- Constructeur --------------------------------
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

//--------------------------- getter et setter ----------------------------
int Pomme::getCoordX() const {
  return coordonnees.x;
}

int Pomme::getCoordY() const {
  return coordonnees.y;
}

unsigned Pomme::getValeur() const {
  return valeur;
}

bool Pomme::estIntacte() const {
  return estMangee;
}

void Pomme::pommeEstMangee() {
  estMangee = false;
}

void Pomme::setCoordPomme(int x, int y) {
  coordonnees.x = x;
  coordonnees.y = y;
}

void Pomme::setValPomme() {
  valeur = (unsigned) aleatoireEntreDeuxValeurs(1, 10);
}
