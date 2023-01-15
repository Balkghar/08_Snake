/*
---------------------------------------------------------------------------
Fichier     : pomme.hpp
Nom du labo : Labo8 - Snake
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 10.01.2022
But         : Déclaration de la classe pomme et des fonctions membres de la
              classe pomme.

Remarque(s) :

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#ifndef LABO8_SNAKES_JEU_POMME_HPP
#define LABO8_SNAKES_JEU_POMME_HPP

#include <array>
#include "outils/structureCoordonnees.hpp"

class Pomme {
public:
   //------------------------ constructeur --------------------------------
   Pomme(unsigned x,
         unsigned y,
         const unsigned id,
         bool estMangee,
         std::array <short,4> coul = {255, 0, 0, 255});

   //------------------------ getter et setter ----------------------------

   unsigned getCoordX();
   unsigned getCoordY();
   unsigned getId();
   coordonneesXY getCoordonnes();

   void setX(unsigned x);
   void setY(unsigned y);

private:

//   std::array<unsigned, 2> coordonnes;
   coordonneesXY coordonnes;
   const unsigned id;
   bool estMangee;
   std::array <short,4> couleurs;

};



#endif