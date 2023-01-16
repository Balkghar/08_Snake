/*
---------------------------------------------------------------------------
Fichier     : snake.hpp
Nom du labo : Labo8 - Snake
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 10.01.2022
But         : Déclaration de la classe snake et  des fonctions membres de la
              classe snake.

Remarque(s) :

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#ifndef LABO8_SNAKES_JEU_SNAKE_HPP
#define LABO8_SNAKES_JEU_SNAKE_HPP

#include <vector>

struct coordonneesXY{
  unsigned x;
  unsigned y;
} ;

enum Direction{ haut, bas, droite, gauche};

class Snake {
public:
   Snake(unsigned x,
         unsigned y,
         const unsigned id,
         bool estEnVie, unsigned longueur);

   void setCoordX(unsigned x);
   void setCoordY(unsigned y);
   void deplacerVersXY(unsigned x, unsigned y);
   void longueurAAjouterSupl(unsigned valeur);

   unsigned getCoordX();
   unsigned getCoordY();
   unsigned getId();
   std::vector<coordonneesXY> getCoord();
   unsigned combattreSerpent(Snake& serpent);

private:
   void deplacerVers(Direction dir);
   void agrandirSerpent(Direction dir);
   unsigned couperSerpent(coordonneesXY& coord);
   std::vector<coordonneesXY> coordonnee;
   const unsigned id;
   unsigned longueurAAjouter;
   bool     estEnVie;

};


#endif