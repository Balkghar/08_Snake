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
#include <random>

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

   faireCombattreSerpents(affichage);

   affichage.fermerAffichage();

}

//=========================== Partie privée ===============================

//------------------------- méthodes d'initialisation -------------------
void Combat::initialiserPomme() {

   pommes.reserve(nbSerpent);

   for (unsigned i = 1; i <= Combat::nbSerpent; ++i) {
      CoordonneesXY nouvelleCoord = generateurDeCoord();

      pommes.emplace_back(nouvelleCoord, i, true);

   }
}

void Combat::initialiserSerpent() {

   serpents.reserve(nbSerpent);

   for (unsigned i = 1; i <= Combat::nbSerpent; ++i) {
      CoordonneesXY nouvelleCoord = generateurDeCoord();

      serpents.emplace_back(nouvelleCoord, i, true, 10);
   }

}

CoordonneesXY Combat::generateurDeCoord() {
   CoordonneesXY nouvelleCoord = {0, 0};

   do {
      nouvelleCoord.x = aleatoireEntreDeuxValeurs(MIN, (int) largeurAffichage);
      nouvelleCoord.y = aleatoireEntreDeuxValeurs(MIN, (int) longueurAffichage);
   } while (placeEstOccupee(nouvelleCoord));

   return nouvelleCoord;
}

//------------------------- contrôle de présence ------------------------
bool Combat::placeEstOccupee(CoordonneesXY coordonnees) {
   return (serpentPresent(coordonnees) or pommePresente(coordonnees));
}

bool Combat::serpentPresent(CoordonneesXY coordonnees) {
   for (Snake &serpent: serpents) {
      for (CoordonneesXY &coord: serpent.getCoord()) {
         if (coord.x == coordonnees.x and coord.y == coordonnees.y) {
            return true;
         }
      }
   }

   return false;
}

bool Combat::pommePresente(CoordonneesXY coordonnees) {
   for (Pomme &mesPommes: pommes) {
      if (mesPommes.getCoordX() == coordonnees.x and mesPommes.getCoordY() == coordonnees.y) {
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
      pomme.setCoordPomme(nouvelleCoord);
      pomme.setValPomme();
   }
}

void Combat::faireCombattreSerpents(Affichage2d &affichage) {

   do {
      shuffle(serpents.begin(), serpents.end(), mt19937(random_device()()));
      afficher(affichage);

      for (Snake &serpent: serpents) {
         for (Pomme &pomme: pommes) {
            if (serpent.getEstEnVie() && serpent.getId() == pomme.getId()) {
               serpent.deplacerVersXY(pomme.getCoord());
               mangerPomme(serpent, pomme);
               combatSerpent(serpent);
            } else {
               if (serpent.getId() == pomme.getId() && pomme.getEstIntacte()) {
                  pomme.pommeEstMangee();
                  suppressionPommes();
               }
            }
         }
      }

      suppressionSerpent();
   } while (nbSerpent > 1);
   cout << Combat::txtSerpent << to_string(serpents.at(0).getId())
        << Combat::txtVictoire << endl;
}

void Combat::combatSerpent(Snake &serpent) {

   for (Snake &i: serpents) {
      if (i.getId() != serpent.getId() && serpent.getEstEnVie()
          && i.getEstEnVie()) {
         if (serpent.combattreSerpent(i)) {
            if (!serpent.getEstEnVie()) {
               cout << Combat::txtSerpent << to_string(i.getId()) << Combat::txtAction
                    << to_string(serpent.getId()) << endl;
            } else {
               cout << txtSerpent << to_string(serpent.getId()) << txtAction
                    << to_string(i.getId()) << endl;
            }
            --nbSerpent;

         }
      }

   }

}

//------------------------- méthodes d'affichage ------------------------

void Combat::ajouterSerpentAffichage(Affichage2d &affichage) {

   for (Snake &serpent: serpents) {
      for (CoordonneesXY &coord: serpent.getCoord()) {
         if (serpent.getEstEnVie()) {
            affichage.ajouterElementAffichage(coord.x, coord.y, Combat::couleurSerpents);
         }
      }
   }
}

void Combat::ajouterPommeAffichage(Affichage2d &affichage) {

   for (Pomme &pomme: pommes) {
      if (pomme.getEstIntacte()) {
         affichage.ajouterElementAffichage(pomme.getCoordX(), pomme.getCoordY(),
                                           Combat::couleurPommes);
      }

   }
}

void Combat::afficher(Affichage2d &affichage) {

   affichage.nettoyerAffichage(Couleur::blanc);

   ajouterSerpentAffichage(affichage);

   ajouterPommeAffichage(affichage);

   affichage.mettreAjourAffichage();

}

void Combat::suppressionPommes() {
   for (vector<Pomme>::reverse_iterator iterPommes = pommes.rbegin(); iterPommes != pommes
      .rend();
        ++iterPommes) {
      if (!iterPommes->getEstIntacte()) {
         pommes.erase(iterPommes.base() - 1);
      }
   }
}

void Combat::suppressionSerpent() {
   for (vector<Snake>::reverse_iterator iterSerpent = serpents.rbegin(); iterSerpent != serpents
      .rend();
        ++iterSerpent) {
      if (!iterSerpent->getEstEnVie()) {
         serpents.erase(iterSerpent.base() - 1);
      }
   }
}
