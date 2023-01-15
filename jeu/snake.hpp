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

#include "outils/structureCoordonnees.hpp"

class Snake {
public:
   //------------------------ Constructeur --------------------------------
   Snake(unsigned x,
         unsigned y,
         const unsigned id,
         bool estEnVie);

   //------------------------ Setter et getter ----------------------------
   void setCoordX(unsigned x);
   void setCoordY(unsigned y);
   void setLongueurAAjouter(unsigned x);

   unsigned getCoordX();
   unsigned getCoordY();
   unsigned getId() const;

   std::vector<coordonneesXY> getCoord();

   //------------------------ Méthode pour le déplacement -----------------

   void deplacerVersObjet(coordonneesXY coordonnesObjet,
                          unsigned largeur,
                          unsigned hauteur);

   static bool peutSeDeplacer(unsigned int x,
                              unsigned int y,
                              unsigned largeur,
                              unsigned hauteur) ;

   //------------------------ Méthode lié au combat -----------------------
   void tuer(Snake& victime);
   void mangerPomme();


private:
   std::vector<coordonneesXY> coordonnees;
   unsigned longueur = 10;
   const unsigned id;
   unsigned longueurAAjouter;
   bool     estEnVie;

};


#endif