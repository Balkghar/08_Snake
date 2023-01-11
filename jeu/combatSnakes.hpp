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


class Combat {
public:
   Combat() : Combat(100, 100, 10) {}
   Combat(unsigned largeur, unsigned longueur, unsigned nbSerpent) :
          largeur(largeur), longueur(longueur), nbSerpent(nbSerpent) {}

private:
   unsigned largeur;
   unsigned longueur;
   unsigned nbSerpent;
   std::vector<Snake> serpents;
};

#endif
