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

Combat::Combat(unsigned int largeur,
               unsigned int longueur,
               unsigned int nbSerpent
) : largeur(largeur), longueur(longueur), nbSerpent(nbSerpent) {

  initialiserSerpent();
  initialiserPomme();
}

void Combat::initialiserPomme() {

  pommes.reserve(nbSerpent);

  const unsigned min = 1;

  int x;
  int y;

  for (unsigned i = 1; i <= Combat::nbSerpent; ++i) {
    do {

      x = aleatoireEntreDeuxEntiersPositifs(min, largeur);
      y = aleatoireEntreDeuxEntiersPositifs(min, longueur);

    } while (placeEstOccupee(x, y));
    pommes.emplace_back(x, y, i, false);

  }
}

void Combat::initialiserSerpent() {

  serpents.reserve(nbSerpent);

  const unsigned min = 1;

  int x;
  int y;

  for (unsigned i = 1; i <= Combat::nbSerpent; ++i) {
    do {

      x = aleatoireEntreDeuxEntiersPositifs(min, largeur);
      y = aleatoireEntreDeuxEntiersPositifs(min, longueur);

    } while (placeEstOccupee(x, y));
    serpents.emplace_back(x, y, i, true, 10);
  }

}

bool Combat::placeEstOccupee(int x,
                             int y
) {

  return (serpentPresent(x, y) or pommePresente(x, y));
}

bool Combat::serpentPresent(int x,
                            int y
) {
  for (Snake &serpent : serpents) {
    for (coordonneesXY &coord : serpent.getCoord()) {
      if (coord.x == x and coord.y == y) {
        return true;
      }
    }

  }

  return false;
}

bool Combat::pommePresente(int x,
                           int y
) {
  for (Pomme &mesPommes : pommes) {
    if (mesPommes.getCoordX() == x and mesPommes.getCoordY() == y) {
      return true;
    }
  }

  return false;
}

void Combat::commencerCombat() {

  Affichage2d affichage(this->largeur, this->longueur, 50, 6);

  affichage.initalisationAffichage();

  combatSerpents(affichage);

  affichage.fermerAffichage();

}
void Combat::combatSerpents(Affichage2d &affichage
) {

  for (int i = 0; i < 500; ++i) {

    afficher(affichage);

    for (size_t i = 0; i < serpents.size(); ++i) {
      serpents.at(i).deplacerVersXY(pommes.at(i).getCoordX(), pommes.at(i).getCoordY());
      mangerPomme(serpents.at(i), pommes.at(i));
      combatSerpent(serpents.at(i));
    }
  }
}

void Combat::combatSerpent(Snake &serpent
) {
  for (size_t i = 0; i < serpents.size(); ++i) {
    if (serpents.at(i).getId() != serpent.getId()) {

      if (unsigned d = serpent.combattreSerpent(serpents.at(i))) {
        if (d == 1) {

        } else {

        }
      }
    }
  }
}

void Combat::eliminerSerpent(Snake &serpent
) {
  for (std::vector<Snake>::iterator it = serpents.begin(); it != serpents.end(); ++it) {
    if (serpent.getId() == (*it).getId()) {
      effacerPomme(serpent.getId());
      serpents.erase(it);
    }
  }

}

void Combat::effacerPomme(unsigned id
) {
  for (std::vector<Pomme>::iterator it = pommes.begin(); it != pommes.end(); ++it) {
    if (id == (*it).getId()) {
      pommes.erase(it);
    }
  }
}

void Combat::afficher(Affichage2d &affichage
) {

  affichage.nettoyerAffichage(Couleur::blanc);

  ajouterSerpentAffichage(affichage);

  ajouterPommeAffichage(affichage);

  affichage.mettreAjourAffichage();

}
void Combat::mangerPomme(Snake &serpent,
                         Pomme &pomme
) {

  const unsigned min = 1;

  int x, y;

  if (serpent.getCoordX() == pomme.getCoordX() && serpent.getCoordY() == pomme.getCoordY()) {

    do {

      x = aleatoireEntreDeuxEntiersPositifs(min, largeur);
      y = aleatoireEntreDeuxEntiersPositifs(min, longueur);

    } while (placeEstOccupee(x, y));
    serpent.longueurAAjouterSupl(pomme.getValeur());
    pomme.nouvellePomme(x, y);
  }
}

void Combat::ajouterSerpentAffichage(Affichage2d &affichage
) {

  for (Snake &serpent : serpents) {
    for (coordonneesXY &coord : serpent.getCoord()) {
      affichage.ajouterElementAffichage(coord.x, coord.y, Couleur::noir);
    }
  }
}

void Combat::ajouterPommeAffichage(Affichage2d &affichage
) {
  for (Pomme &pomme : pommes) {

    affichage.ajouterElementAffichage(pomme.getCoordX(), pomme.getCoordY(), Couleur::rouge);

  }
}