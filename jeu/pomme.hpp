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
#include "../outils/struct_coordonnees.hpp"

class Pomme {
 public:
  //------------------------- constructeur --------------------------------
  Pomme(int x,
        int y,
        const unsigned id,
        bool estMangee,
        std::array<short, 4> coul = {255, 0, 0, 255}
  );

  //------------------------- getter et setter ----------------------------
  int getCoordX() const;
  int getCoordY() const;
  unsigned getValeur() const;
  bool estIntacte() const;
  void pommeEstMangee();
  void setCoordPomme(int x, int y);
  void setValPomme();

 private:
  //------------------------- Données -------------------------------------
  unsigned valeur;
  const unsigned id;
  bool estMangee;
  CoordonneesXY coordonnees;
  std::array<short, 4> couleurs;

};

#endif