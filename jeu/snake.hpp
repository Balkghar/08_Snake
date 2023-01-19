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
#include <string>

#include "../outils/struct_coordonnees.hpp"

enum Direction { haut, bas, droite, gauche };

class Snake {
 public:
  //------------------------- Constructeur --------------------------------
  Snake(int x,
        int y,
        const unsigned id,
        bool estEnVie,
        unsigned longueur
  );

  //------------------------- Déplacements --------------------------------
  void deplacerVersXY(int x, int y);
  void deplacerVers(Direction dir);

  //------------------------- getter et setter ----------------------------
  int getCoordX() const;
  int getCoordY() const;
  unsigned getId() const;
  bool getEstEnVie() const;
  std::vector<CoordonneesXY> getCoord() const;

  //------------------------- autres --------------------------------------
  void longueurAAjouterSupl(unsigned valeur);
  bool combattreSerpent(Snake &serpent);

 private:

  //------------------------- Agrandissement ------------------------------
  void agrandirSerpent(CoordonneesXY &coord);
  unsigned calculAjoutLongueur(std::size_t longu, unsigned pourcentage);

  //------------------------- Méthodes de combat --------------------------
  void couperSerpent(Snake &serpent);
  void mourir(Snake &serpent);

  //------------------------- Données -------------------------------------
  const unsigned id;
  unsigned longueurAAjouter;
  bool estEnVie;
  std::vector<CoordonneesXY> coordonnees;

};

#endif