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

struct coordonneesXY {
  int x;
  int y;
};

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
  std::vector<coordonneesXY> getCoord();
  bool combattreSerpent(Snake &serpent);

 private:
  void deplacerVers(Direction dir);
  void agrandirSerpent(coordonneesXY &coord);
  void couperSerpent(coordonneesXY &coord, Snake &serpent);
  void tuerSerpent(Snake &serpent);
  unsigned calculAjoutLongueur(unsigned long long int longu, unsigned pourcentage);
  std::vector<coordonneesXY> coordonnee;
  const unsigned id;
  unsigned longueurAAjouter;
  bool estEnVie;

};

#endif