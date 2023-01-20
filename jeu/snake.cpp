/*
  ---------------------------------------------------------------------------
  Fichier     : snake.cpp
  Nom du labo : Labo8 - Snake
  Auteur(s)   : Delétraz Alexandre - Germano Hugo
  Date        : 10.01.2023
  But         : Définition des fonctions membres de la classe snake.

  Remarque(s) : 

  Compilateur : gcc version 11.2.0
  ---------------------------------------------------------------------------
*/
#include "snake.hpp"
#include <vector>
#include <cstdlib>

using namespace std;


//=========================== Partie public ===============================

//--------------------------- Constructeur --------------------------------
Snake::Snake(int x,
             int y,
             const unsigned id,
             bool estEnVie,
             unsigned longueur
) : id(id), estEnVie(estEnVie) {

  coordonnees.resize(1);

  coordonnees.at(0).x = x;
  coordonnees.at(0).y = y;

  longueurAAjouter = longueur - 1;

}

//------------------------- Déplacements --------------------------------
void Snake::deplacerVersXY(int x, int y) {

  if (getCoordX() != x || getCoordY() != y) {

    int diffX = abs(getCoordX() - x);
    int diffY = abs(getCoordY() - y);

    if (getCoordX() <= x && getCoordY() <= y) {
      if (diffX < diffY) {
        deplacerVers(Snake::bas);
      } else {
        deplacerVers(Snake::droite);
      }
    } else if (getCoordX() >= x && getCoordY() <= y) {
      if (diffX < diffY) {
        deplacerVers(Snake::bas);
      } else {
        deplacerVers(Snake::gauche);
      }
    } else if (getCoordX() >= x && getCoordY() >= y) {
      if (diffX < diffY) {
        deplacerVers(Snake::haut);
      } else {
        deplacerVers(Snake::gauche);
      }
    } else if (getCoordX() <= x && getCoordY() >= y) {
      if (diffX < diffY) {
        deplacerVers(Snake::haut);
      } else {
        deplacerVers(Snake::droite);
      }
    }
  }
}

void Snake::deplacerVers(Direction dir) {

  CoordonneesXY tmpCoord = coordonnees.back();

  if (coordonnees.size() > 1) {
    for (size_t i = coordonnees.size() - 1; i >= 1; --i) {
      coordonnees.at(i) = coordonnees.at(i - 1);
    }
  }

  switch (dir) {
    case Snake::haut :--coordonnees.at(0).y;
      break;
    case Snake::bas :++coordonnees.at(0).y;
      break;
    case Snake::droite :++coordonnees.at(0).x;
      break;
    case Snake::gauche :--coordonnees.at(0).x;
      break;
  }

  if (longueurAAjouter) {
    agrandirSerpent(tmpCoord);
    longueurAAjouter -= 1;
  }
}

//--------------------------- getter et setter ----------------------------
int Snake::getCoordX() const {
  return coordonnees.at(0).x;
}

int Snake::getCoordY() const {
  return coordonnees.at(0).y;
}

unsigned Snake::getId() const {
  return id;
}

bool Snake::getEstEnVie() const {
  return estEnVie;
}

std::vector<CoordonneesXY> Snake::getCoord() const {
  return coordonnees;
}

//------------------------- autres --------------------------------------
void Snake::longueurAAjouterSupl(unsigned valeur) {

  longueurAAjouter += valeur;
}

bool Snake::combattreSerpent(Snake &serpent) {
  if (serpent.getCoordX() == getCoordX() && serpent.getCoordY() == getCoordY()) {
    if (coordonnees.size() < serpent.coordonnees.size()) {
      mourir(serpent);
      return true;
    } else {
      serpent.mourir((*this));
      return true;
    }
  } else {
    serpent.couperSerpent((*this));
  }
  return false;
}

//=========================== Partie privée ===============================



//------------------------- Agrandissement ------------------------------
unsigned calculAjoutLongueur(std::size_t longu, unsigned pourcentage) {

  return (unsigned) ((longu /100.)*pourcentage);
}

void Snake::agrandirSerpent(CoordonneesXY &coord) {
  coordonnees.push_back(coord);
}

//------------------------- Méthodes de combat --------------------------
void Snake::couperSerpent(Snake &serpent) {
  unsigned i = 1;
  for (const CoordonneesXY &coordo : serpent.coordonnees) {
    if (coordo.x == (*this).getCoordX() && coordo.y == (*this).getCoordY()) {
      longueurAAjouterSupl(calculAjoutLongueur(serpent.coordonnees.size() - i, 40));
      serpent.longueurAAjouter = 0;
      serpent.coordonnees.resize(i);
      break;
    }
    ++i;
  }
}

void Snake::mourir(Snake &serpent) {

  estEnVie = false;
  serpent.longueurAAjouterSupl(calculAjoutLongueur(coordonnees.size(), 60));
}