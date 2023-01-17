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

class Pomme {
public:
   Pomme(unsigned x,
         unsigned y,
         const unsigned id,
         bool estMangee,
         std::array <short,4> coul = {255, 0, 0, 255});

   void     pommeEstMangee();

   unsigned getCoordX();
   unsigned getCoordY();
   unsigned getId();
   unsigned getValeur();
   bool     getEstMangee();

   void nouvellePomme(unsigned x, unsigned y);

private:
   std::array<unsigned, 2> coordonnes;
   unsigned valeur;
   const unsigned id;
   bool estMangee;
   std::array <short,4> couleurs;

};



#endif