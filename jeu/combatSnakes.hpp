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
  static constexpr unsigned DELAI_DEFAUT = 50;  // ms entre deux images
  static constexpr unsigned ZOOM_DEFAUT = 4;    // pixels écran par case
  static constexpr unsigned VITESSE_DEFAUT = 1; // tours de jeu par image
  static constexpr unsigned VITESSE_AUTO = 0;   // autant que le temps permet

  void commencerCombat(unsigned delai = DELAI_DEFAUT,
                       unsigned zoom = ZOOM_DEFAUT,
                       unsigned vitesse = VITESSE_DEFAUT);

 private:

  //------------------------- méthodes d'initialisation -------------------
  void initialiserPomme();
  void initialiserSerpent();
  CoordonneesXY generateurDeCoord();

  //------------------------- grille d'occupation -------------------------
  bool placeEstOccupee(size_t i) const;
  size_t indexCase(int x, int y) const;
  void modifierCase(int x, int y, int deltaSerpent, int deltaPomme);
  void appliquerModifications(Snake &serpent);

  //------------------------- méthodes du jeu -----------------------------
  void jouerTour();
  void mangerPomme(Snake &serpent, Pomme &pomme);
  void combatSerpent(size_t indice);
  void tuer(Snake &victime, const Snake &tueur);

  //------------------------- méthodes d'affichage ------------------------
  bool afficher(Affichage2d &affichage, int &accelerer);
  bool faireCombattreSerpents(Affichage2d &affichage);
  void changerVitesse(int pas);
  void mettreAJourTitre(Affichage2d &affichage) const;

  //------------------------- fin de partie -------------------------------
  void afficherVictoire(Affichage2d &affichage);
  std::vector<std::string> statistiques() const;
  static MotifPixel logoGreenKatze();

  //------------------------- Données -------------------------------------
  const unsigned largeur;
  const unsigned longueur;
  unsigned largeurAffichage;
  unsigned longueurAffichage;
  unsigned nbSerpent;
  const unsigned nbSerpentsDepart;
  unsigned long nbTours = 0;
  unsigned dureeMs = 0;
  unsigned vitesse = VITESSE_DEFAUT;
  unsigned delai = DELAI_DEFAUT;
  unsigned long toursDerniereImage = 0;

  static constexpr unsigned MIN = 0;
  static constexpr unsigned AUCUNE_TETE = 0;

  std::vector<Snake> serpents;
  std::vector<Pomme> pommes;
  std::vector<unsigned> vivants;  // indices des serpents en vie, dans l'ordre

  // Grilles du terrain (une case par élément, indexCase(x, y)) :
  //  - nombre de segments de serpents vivants et de pommes par case, tenus à
  //    jour de façon incrémentale ; seules les casesModifiees sont redessinées
  //  - teteSurCase : indice + 1 du serpent vivant dont la tête est sur la
  //    case (il ne peut y en avoir qu'une : deux têtes qui se rencontrent se
  //    battent aussitôt). Évite de comparer chaque serpent à tous les autres.
  std::vector<int> occupationSerpents;
  std::vector<int> occupationPommes;
  std::vector<unsigned> teteSurCase;
  std::vector<bool> caseMarquee;
  std::vector<CoordonneesXY> casesModifiees;
  size_t nbCasesOccupees = 0;
};

#endif
