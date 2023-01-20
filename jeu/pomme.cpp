/*
---------------------------------------------------------------------------
Fichier     : pomme.cpp
Nom du labo : Labo8 - Snake
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 10.01.2023
But         : Définition des fonctions membres de la classe pomme.

Remarque(s) : La valeur des pommes est volontairement de 1 au départ pour toutes les
              pommes.

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#include "pomme.hpp"
#include "../outils/aleatoire.hpp"

//=========================== Partie public ===============================

//--------------------------- Constructeur --------------------------------
Pomme::Pomme(CoordonneesXY coord,
             const unsigned id,
             bool estIntacte
) : id(id), entiere(estIntacte) {
   coordonnees = coord;
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
   return entiere;
}

void Pomme::pommeEstMangee() {
   entiere = false;
}

void Pomme::setCoordPomme(CoordonneesXY coord) {

   coordonnees.x = coord.x;
   coordonnees.y = coord.y;
}

void Pomme::setValPomme() {
   valeur = (unsigned) aleatoireEntreDeuxValeurs(1, 10);
}

CoordonneesXY Pomme::getCoord() const {
   return coordonnees;
}

unsigned Pomme::getId() const {
   return id;
}
