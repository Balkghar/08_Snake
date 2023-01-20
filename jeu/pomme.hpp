/*
---------------------------------------------------------------------------
Fichier     : pomme.hpp
Nom du labo : Labo8 - Snake
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 10.01.2023
But         : Déclaration de la classe pomme et des fonctions membres de la
              classe pomme.

Remarque(s) :

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#ifndef LABO8_SNAKES_JEU_POMME_HPP
#define LABO8_SNAKES_JEU_POMME_HPP

#include <array>
#include "../outils/struct_coordonnees.hpp"

class Pomme {
public:
   //------------------------- constructeur --------------------------------
   Pomme(CoordonneesXY coord,
         const unsigned id,
         bool estIntacte);

   //------------------------- getter et setter ----------------------------
   int getCoordX() const;

   int getCoordY() const;

   CoordonneesXY getCoord() const;

   unsigned getValeur() const;

   unsigned getId() const;

   bool estIntacte() const;

   void pommeEstMangee();

   void setCoordPomme(CoordonneesXY coord);

   void setValPomme();

private:
   //------------------------- Données -------------------------------------
   const unsigned id;
   unsigned valeur;
   bool entiere;
   CoordonneesXY coordonnees;

};

#endif