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
   Pomme(std::array<unsigned, 2> coord,
         std::array <short,4> coul,
         const int id,
         bool estMangee);

private:

   std::array<unsigned, 2> coordonnes;
   std::array <short,4> couleurs;
   const int id;
   bool estMangee;



};



#endif