/*
---------------------------------------------------------------------------
Fichier     : combatSnakes.hpp
Nom du labo : Labo8 - Snake
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 10.01.2023
But         : Déclaration de la classe combat et des fonctions membres de la classe
              combat.

Remarque(s) :

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#ifndef LABO8_SNAKES_JEU_COMBATSNAKES_HPP
#define LABO8_SNAKES_JEU_COMBATSNAKES_HPP

#include <vector>
#include <string>
#include "snake.hpp"
#include "pomme.hpp"
#include "../outils/affichage2d.hpp"

class Combat {
 public:
  //------------------------- Constructeur --------------------------------
  Combat() : Combat(100, 100, 10) {}
  Combat(unsigned largeur,
         unsigned longueur,
         unsigned nbSerpent
  );

  //------------------------- lancement du combat -------------------------
  void commencerCombat();

 private:

  //------------------------- méthodes d'initialisation -------------------
  void initialiserPomme();
  void initialiserSerpent();
  CoordonneesXY generateurDeCoord();

  //------------------------- contrôle de présence ------------------------
  bool placeEstOccupee(int x, int y) const;

  //------------------------- grille d'occupation -------------------------
  size_t indexCase(int x, int y) const;
  void modifierCase(int x, int y, int deltaSerpent, int deltaPomme);
  void appliquerModificationsSerpents();

  //------------------------- méthodes du jeu -----------------------------
  void mangerPomme(Snake &serpent, Pomme &pomme);
  void combatSerpent(Snake &serpent);

  //------------------------- méthodes d'affichage ------------------------
  bool afficher(Affichage2d &affichage);
  void faireCombattreSerpents(Affichage2d &affichage);

  //------------------------- Données -------------------------------------
  const unsigned largeur;
  const unsigned longueur;
  unsigned largeurAffichage;
  unsigned longueurAffichage;
  unsigned nbSerpent;

  static const unsigned SDL_DELAY = 50;
  static const unsigned AUGMENT_PIXEL = 4;
  static const unsigned MIN = 0;

  std::string serpentTueur;
  std::vector<Snake> serpents;
  std::vector<Pomme> pommes;

  // Nombre de segments de serpents vivants / de pommes sur chaque case. Tenue
  // à jour de façon incrémentale : seules les cases listées dans
  // casesModifiees sont redessinées à chaque image.
  std::vector<int> occupationSerpents;
  std::vector<int> occupationPommes;
  std::vector<CoordonneesXY> casesModifiees;
};

#endif
