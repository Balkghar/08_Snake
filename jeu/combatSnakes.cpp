/*
---------------------------------------------------------------------------
Fichier     : combatSnakes.cpp
Nom du labo : Labo8 - Snake
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 10.01.2023
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

  occupationSerpents.assign(size_t(largeur) * longueur, 0);
  occupationPommes.assign(size_t(largeur) * longueur, 0);

  initialiserSerpent();
  initialiserPomme();
}
//------------------------- lancement du combat -------------------------
void Combat::commencerCombat(unsigned delai, unsigned zoom) {

  Affichage2d affichage(largeur, longueur, delai, zoom);

  if (not affichage.initalisationAffichage()) {
    affichage.nettoyerAffichage(Couleur::blanc);
    faireCombattreSerpents(affichage);
  }

  affichage.fermerAffichage();

}

//=========================== Partie privée ===============================

//------------------------- méthodes d'initialisation -------------------
void Combat::initialiserPomme() {

  pommes.reserve(nbSerpent);

  for (unsigned i = 1; i <= Combat::nbSerpent; ++i) {
    CoordonneesXY nouvelleCoord = generateurDeCoord();

    pommes.emplace_back(nouvelleCoord.x, nouvelleCoord.y, i, true);
    modifierCase(nouvelleCoord.x, nouvelleCoord.y, 0, +1);

  }
}

void Combat::initialiserSerpent() {

  serpents.reserve(nbSerpent);

  for (unsigned i = 1; i <= Combat::nbSerpent; ++i) {
    CoordonneesXY nouvelleCoord = generateurDeCoord();

    serpents.emplace_back(nouvelleCoord.x, nouvelleCoord.y, i, true, 10);
    appliquerModificationsSerpents();
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
bool Combat::placeEstOccupee(int x, int y) const {
  const size_t i = indexCase(x, y);
  return occupationSerpents[i] > 0 or occupationPommes[i] > 0;
}

//------------------------- grille d'occupation -------------------------
size_t Combat::indexCase(int x, int y) const {
  return size_t(y) * largeur + size_t(x);
}

void Combat::modifierCase(int x, int y, int deltaSerpent, int deltaPomme) {
  if (x < 0 or y < 0 or unsigned(x) >= largeur or unsigned(y) >= longueur) {
    return;
  }
  const size_t i = indexCase(x, y);
  occupationSerpents[i] += deltaSerpent;
  occupationPommes[i] += deltaPomme;
  casesModifiees.push_back({x, y});
}

void Combat::appliquerModificationsSerpents() {
  for (Snake &serpent : serpents) {
    for (const CoordonneesXY &coord : serpent.getCasesAjoutees()) {
      modifierCase(coord.x, coord.y, +1, 0);
    }
    for (const CoordonneesXY &coord : serpent.getCasesRetirees()) {
      modifierCase(coord.x, coord.y, -1, 0);
    }
    serpent.oublierModifications();
  }
}

//------------------------- méthodes du jeu -----------------------------

void Combat::mangerPomme(Snake &serpent, Pomme &pomme) {

  if (serpent.getCoordX() == pomme.getCoordX() && serpent.getCoordY() == pomme.getCoordY()) {
    // La grille doit être à jour pour trouver une case libre
    appliquerModificationsSerpents();
    modifierCase(pomme.getCoordX(), pomme.getCoordY(), 0, -1);
    CoordonneesXY nouvelleCoord = generateurDeCoord();

    serpent.longueurAAjouterSupl(pomme.getValeur());
    pomme.setCoordPomme(nouvelleCoord.x, nouvelleCoord.y);
    modifierCase(nouvelleCoord.x, nouvelleCoord.y, 0, +1);
    pomme.setValPomme();
  }
}

void Combat::faireCombattreSerpents(Affichage2d &affichage) {

  do {

    if (afficher(affichage)) {
      return;  // fenêtre fermée par l'utilisateur
    }

    for (size_t d = 0; d < serpents.size(); ++d) {
      if (serpents.at(d).getEstEnVie()) {
        serpents.at(d).deplacerVersXY(pommes.at(d).getCoordX(), pommes.at(d).getCoordY());
        mangerPomme(serpents.at(d), pommes.at(d));
        combatSerpent(serpents.at(d));
      } else {
        if (pommes.at(d).estIntacte()) {
          pommes.at(d).pommeEstMangee();
          modifierCase(pommes.at(d).getCoordX(), pommes.at(d).getCoordY(), 0, -1);
        }
      }
    }
  } while (nbSerpent > 1);
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

bool Combat::afficher(Affichage2d &affichage) {

  appliquerModificationsSerpents();

  // Seules les cases modifiées depuis la dernière image sont redessinées
  // (une pomme reste dessinée par-dessus un serpent)
  for (const CoordonneesXY &coord : casesModifiees) {
    const size_t i = indexCase(coord.x, coord.y);
    Couleur couleur = Couleur::blanc;
    if (occupationPommes[i] > 0) {
      couleur = Couleur::rouge;
    } else if (occupationSerpents[i] > 0) {
      couleur = Couleur::noir;
    }
    affichage.ajouterElementAffichage(coord.x, coord.y, couleur);
  }
  casesModifiees.clear();

  affichage.mettreAjourAffichage();

  return affichage.fermetureDemandee();
}
