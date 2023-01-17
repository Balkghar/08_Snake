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


class Combat {
public:
   Combat() : Combat(100, 100, 10) {}
   Combat(unsigned largeur, unsigned longueur, unsigned nbSerpent);
   void commencerCombat();



private:
   const unsigned largeur;
   const unsigned longueur;
   const unsigned nbSerpent; // pas nécessaire de stocker cette variable (selon l'assistant)
   std::vector<Snake> serpents;
   std::vector<Pomme> pommes;
   bool placeEstOccupee( unsigned x, unsigned y );
   bool serpentPresent(unsigned int x, unsigned int y);
   bool pommePresente(unsigned int x, unsigned int y);
   void initialiserPomme();
   void initialiserSerpent();
   void ajouterSerpentAffichage(Affichage2d& affichage);
   void ajouterPommeAffichage(Affichage2d& affichage);
   void mangerPomme(Snake& serpent, Pomme& pomme);
   void combatSerpents(Affichage2d& affichage);
   void afficher(Affichage2d& affichage);
   void combatSerpent(Snake& serpent);
   void eliminerSerpent(Snake& serpent);
};

#endif
