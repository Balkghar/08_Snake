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
#include "outils/struct_coordonnees.hpp"

enum Direction { haut, bas, droite, gauche };

class Snake {
 public:
  Snake(int x,
        int y,
        const unsigned id,
        bool estEnVie,
        unsigned longueur
  );

  void setCoordX(int x);
  void setCoordY(int y);
  void deplacerVersXY(int x, int y);
  void longueurAAjouterSupl(unsigned valeur);

  int getCoordX();
  int getCoordY();
  unsigned getId();
  bool getEstEnVie();
  std::vector<CoordonneesXY> getCoord();
  bool combattreSerpent(Snake &serpent);

 private:
  void deplacerVers(Direction dir);
  void agrandirSerpent(CoordonneesXY &coord);
  void couperSerpent(CoordonneesXY &coord, Snake &serpent);
  void tuerSerpent(Snake &serpent);
  unsigned calculAjoutLongueur(std::size_t longu, unsigned pourcentage);
  std::vector<CoordonneesXY> coordonnee;
  const unsigned id;
  unsigned longueurAAjouter;
  bool estEnVie;

};

#endif