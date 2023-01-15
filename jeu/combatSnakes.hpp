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
   //------------------------ Constructeurs -------------------------------
   AreneDeCombat() : AreneDeCombat(100, 100, 10) {}
   AreneDeCombat(unsigned largeur, unsigned longueur, unsigned nbSerpent);

   void commencerCombat();

private:
   unsigned largeur;
   unsigned hauteur;
   unsigned nbSerpent;
   std::vector<Snake> serpents;
   std::vector<Pomme> pommes;

   //------------------------ Méthodes d'initialisation --------------------
   bool placeEstOccupee(unsigned x, unsigned y );
   bool serpentPresent(unsigned int x, unsigned int y);
   bool pommePresente(unsigned int x, unsigned int y);
   void initialiserPommes();
   void initialiserSerpent();
   void nouvellePomme(Pomme pommeMangee);


   //------------------------ Méthodes pour le déplacement ----------------
   static coordonneesXY bonnePomme(const std::vector<Snake>& serpents,
                                   const std::vector<Pomme>& pommes);

   void tourDeJeu();


   //------------------------ Méthode pour l'affichage --------------------

   void ajouterSerpentAffichage(Affichage2d& affichage);
   void ajouterPommeAffichage(Affichage2d& affichage);


};

#endif
