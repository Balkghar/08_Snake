/*
  ---------------------------------------------------------------------------
  Fichier     : snake.cpp
  Nom du labo : Labo8 - Snake
  Auteur(s)   : Delétraz Alexandre - Germano Hugo
  Date        : 10.01.2022
  But         : Définition des fonctions membres de la classe snake.

  Remarque(s) : 

  Compilateur : gcc version 11.2.0
  ---------------------------------------------------------------------------
*/
#include <vector>

#include "snake.hpp"

using namespace std;

//--------------------------- Constructeur --------------------------------

Snake::Snake(unsigned x,
             unsigned y,
             const unsigned id,
             bool estEnVie) : id(id), estEnVie(estEnVie) {
   coordonnees.resize(longueur);

   coordonnees.at(id).x = x;
   coordonnees.at(id).y = y;

   longueurAAjouter = longueur;

}

//--------------------------- getter et setter ----------------------------


void Snake::setCoordX(unsigned x) {
   coordonnees.at(0).x = x;


}

void Snake::setCoordY(unsigned y) {
   coordonnees.at(0).y = y;

}

void Snake::setLongueurAAjouter(unsigned x) {
   this->longueurAAjouter = x;
}

unsigned Snake::getCoordX() {
   return this->coordonnees.at(0).x;

}

unsigned Snake::getCoordY() {
   return this->coordonnees.at(0).y;

}

std::vector<coordonneesXY> Snake::getCoord() {
   return this->coordonnees;
}

unsigned Snake::getId() const {
   return id;
}

//--------------------------- Méthode pour le déplacement -----------------

void Snake::deplacerVersObjet(coordonneesXY coordonnesObjet,
                              unsigned largeur,
                              unsigned hauteur) {
   do {
      if(this->getCoordX() < coordonnesObjet.x) {
         this->coordonnees.at(getId()).x += 1;
      }
      else if(this->getCoordX() > coordonnesObjet.x) {
         this->coordonnees.at(getId()).x -= 1;
      }
      if(this->getCoordY() < coordonnesObjet.y) {
         this->coordonnees.at(getId()).y += 1;
      }
      else if(this->getCoordY() > coordonnesObjet.y) {
         this->coordonnees.at(getId()).y -= 1;
      }
   }while(peutSeDeplacer(this->coordonnees.at(getId()).x,
                         this->coordonnees.at(getId()).y,
                         largeur, hauteur));

}

bool Snake::peutSeDeplacer(unsigned int x,
                           unsigned int y,
                           unsigned largeur,
                           unsigned hauteur) {
   if(x > largeur or y > hauteur or x == 0 or y == 0) {
      return true;
   }

   return false;
}

//------------------------ Méthode lié au combat -----------------------

void Snake::tuer(Snake& victime) {
   this->longueurAAjouter += unsigned(victime.longueur * 0.6);
   victime.estEnVie = false;
}

void Snake::mangerPomme() {
   this->longueur +=1;

}


