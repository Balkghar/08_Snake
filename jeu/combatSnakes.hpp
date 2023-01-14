/*
---------------------------------------------------------------------------
Fichier     : combatSnakes.hpp
Nom du labo : Labo8 - Snake
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 10.01.2022
But         : Déclaration de la classe combat et des fonctions membres de la classe
              combat.

Remarque(s) :

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#ifndef LABO8_SNAKES_JEU_COMBATSNAKES_HPP
#define LABO8_SNAKES_JEU_COMBATSNAKES_HPP

#include <vector>
#include "snake.hpp"
#include "pomme.hpp"
#include "../outils/affichage2d.hpp"

class AreneDeCombat {
public:
   AreneDeCombat() : AreneDeCombat(100, 100, 10) {}
   AreneDeCombat(unsigned largeur, unsigned longueur, unsigned nbSerpent);

   void commencerCombat();

private:
   unsigned largeur;
   unsigned longueur;
   unsigned nbSerpent;
   std::vector<Snake> serpents;
   std::vector<Pomme> pommes;

   bool placeEstLibre( unsigned x, unsigned y );
   bool serpentPresent(unsigned int x, unsigned int y);
   bool pommePresente(unsigned int x, unsigned int y);

   void initialiserPomme();
   void initialiserSerpent();
   void ajouterSerpentAffichage(Affichage2d& affichage);
   void ajouterPommeAffichage(Affichage2d& affichage);

};

#endif
