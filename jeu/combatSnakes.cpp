/*
---------------------------------------------------------------------------
Fichier     : combatSnakes.cpp
Nom du labo : Labo8 - Snake
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 10.01.2023
But         : Définition des fonctions membres de la classe
              Combat.

Remarque(s) :

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#include <iostream>
#include "combatSnakes.hpp"
#include "../outils/aleatoire.hpp"

using namespace std;
//=========================== Partie public ===============================

//------------------------- Constructeur --------------------------------
Combat::Combat(unsigned int largeur,
               unsigned int longueur,
               unsigned int nbSerpent
) : largeur(largeur), longueur(longueur), nbSerpent(nbSerpent) {

  // L'affichage SDL partant de 0, pour que l'affichage soit correct à l'écran,
  // il faut faire moins 1 à la valeur entrée par l'utilisateur.
  longueurAffichage = longueur - 1;
  largeurAffichage = largeur - 1;

  initialiserSerpent();
  initialiserPomme();
}
//------------------------- lancement du combat -------------------------
void Combat::commencerCombat() {

  Affichage2d affichage(largeur, longueur, SDL_DELAY, AUGMENT_PIXEL);

  affichage.initalisationAffichage();

  affichercombatSerpents(affichage);

  affichage.fermerAffichage();

}

//=========================== Partie privée ===============================

//------------------------- méthodes d'initialisation -------------------
void Combat::initialiserPomme() {

  pommes.reserve(nbSerpent);

  for (unsigned i = 1; i <= Combat::nbSerpent; ++i) {

    CoordonneesXY nouvelleCoord = generateurDeCoord();

    pommes.emplace_back(nouvelleCoord.x, nouvelleCoord.y, i, true);

  }
}

void Combat::initialiserSerpent() {

  serpents.reserve(nbSerpent);

  for (unsigned i = 1; i <= Combat::nbSerpent; ++i) {
    CoordonneesXY nouvelleCoord = generateurDeCoord();

    serpents.emplace_back(nouvelleCoord.x, nouvelleCoord.y, i, true, 10);
  }

}

CoordonneesXY Combat::generateurDeCoord() {

  CoordonneesXY nouvelleCoord = {0, 0};

  do {
    nouvelleCoord.x = aleatoireEntreDeuxValeurs(MIN, (int) largeurAffichage);
    nouvelleCoord.y = aleatoireEntreDeuxValeurs(MIN, (int) longueurAffichage);
  } while (placeEstOccupee(nouvelleCoord.x, nouvelleCoord.y));

  return nouvelleCoord;
}

//------------------------- contrôle de présence ------------------------
bool Combat::placeEstOccupee(int x, int y) {

  return (serpentPresent(x, y) or pommePresente(x, y));
}

bool Combat::serpentPresent(int x, int y) {

  for (Snake &serpent : serpents) {
    for (CoordonneesXY &coord : serpent.getCoord()) {
      if (coord.x == x and coord.y == y) {
        return true;
      }
    }
  }

  return false;
}

bool Combat::pommePresente(int x, int y) {

  for (Pomme &mesPommes : pommes) {
    if (mesPommes.getCoordX() == x and mesPommes.getCoordY() == y) {
      return true;
    }
  }

  return false;
}

//------------------------- méthodes du jeu -----------------------------

void Combat::mangerPomme(Snake &serpent, Pomme &pomme) {

  if (serpent.getCoordX() == pomme.getCoordX() && serpent.getCoordY() == pomme.getCoordY()) {
    CoordonneesXY nouvelleCoord = generateurDeCoord();

    serpent.longueurAAjouterSupl(pomme.getValeur());
    pomme.setCoordPomme(nouvelleCoord.x, nouvelleCoord.y);
    pomme.setValPomme();
  }
}

void Combat::combatSerpent(Snake &serpent) {

  const string txtSerpent = "Le serpent "s;
  const string txtAction = " a tuer le serpent "s;

  for (size_t i = 0; i < serpents.size(); ++i) {
    if (serpents.at(i).getId() != serpent.getId() && serpent.getEstEnVie()
        && serpents.at(i).getEstEnVie()) {
      if (serpent.combattreSerpent(serpents.at(i))) {
        if (!serpent.getEstEnVie()) {
          cout << txtSerpent + to_string(serpents.at(i).getId()) + txtAction
              + to_string(serpent.getId()) + "\n"s;
        } else {
          cout << txtSerpent + to_string(serpent.getId()) + txtAction
              + to_string(serpents.at(i).getId()) + "\n"s;
        }
        --nbSerpent;
      }
    }

  }
}

//------------------------- méthodes d'affichage ------------------------

void Combat::ajouterSerpentAffichage(Affichage2d &affichage) {

  for (Snake &serpent : serpents) {
    for (CoordonneesXY &coord : serpent.getCoord()) {
      if (serpent.getEstEnVie()) {
        affichage.ajouterElementAffichage(coord.x, coord.y, Couleur::noir);
      }
    }
  }
}

void Combat::ajouterPommeAffichage(Affichage2d &affichage) {

  for (Pomme &pomme : pommes) {
    if (pomme.estIntacte()) {
      affichage.ajouterElementAffichage(pomme.getCoordX(), pomme.getCoordY(), Couleur::rouge);
    }

  }
}

void Combat::afficher(Affichage2d &affichage) {

  affichage.nettoyerAffichage(Couleur::blanc);

  ajouterSerpentAffichage(affichage);

  ajouterPommeAffichage(affichage);

  affichage.mettreAjourAffichage();

}

void Combat::affichercombatSerpents(Affichage2d &affichage) {

  do {

    afficher(affichage);

    for (size_t d = 0; d < serpents.size(); ++d) {
      if (serpents.at(d).getEstEnVie()) {
        serpents.at(d).deplacerVersXY(pommes.at(d).getCoordX(), pommes.at(d).getCoordY());
        mangerPomme(serpents.at(d), pommes.at(d));
        combatSerpent(serpents.at(d));
      } else {
        if (pommes.at(d).estIntacte()) {
          pommes.at(d).pommeEstMangee();
        }
      }
    }
  } while (nbSerpent > 1);
}