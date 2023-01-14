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

class Snake {
public:
   Snake(unsigned x,
         unsigned y,
         const unsigned id,
         bool estEnVie);

   void setCoordX(unsigned x);
   void setCoordY(unsigned y);
   void deplacerVersPomme(unsigned int xPomme, unsigned int yPomme,
                          const unsigned idPomme,
                          unsigned largeur,
                          unsigned hauteur,
                          std::vector<Snake>& serpents);

   unsigned getCoordX();
   unsigned getCoordY();
   unsigned getId();

   bool peutSeDeplacer(unsigned int x, unsigned int y, unsigned largeur,
                       unsigned hauteur) const;

private:
   std::vector<std::vector<unsigned>> coordonnee;
   unsigned longueur = 10;
   const unsigned id;
   unsigned longueurAAjouter;
   bool     estEnVie;

};


#endif