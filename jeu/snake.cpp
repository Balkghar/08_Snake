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
#include "snake.hpp"
#include <vector>
#include <cmath>

Snake::Snake(unsigned x,unsigned y, const unsigned id, bool estEnVie, unsigned longueur) : id(id), estEnVie(estEnVie) {

   coordonnee.resize(1);

   coordonnee.at(0).x = x;
   coordonnee.at(0).y = y;


   longueurAAjouter = longueur-1;


}

bool Snake::combattreSerpent(Snake& serpent){
   for(coordonneesXY coord: serpent.getCoord()){
      if(coord.x == serpent.getCoordX() && coord.y == serpent.getCoordY()){
         if(serpent.getCoordX() == this->getCoordX() && serpent.getCoordY() == this->getCoordY()){
            if(this->getCoord().size() < serpent.getCoord().size()){
               this->tuerSerpent(serpent);
               return true;
            }
            else{
               serpent.tuerSerpent((*this));
               return true;
            }
         }
         else{
            serpent.couperSerpent(coord, (*this));
         }
      }
   }
   return false;
}
void Snake::couperSerpent(coordonneesXY& coord, Snake& serpent){
   unsigned i = 1;
   for(coordonneesXY& coordo : serpent.coordonnee){
      ++i;
      if(coordo.x == coord.x && coordo.y == coord.y){
         this->longueurAAjouterSupl(calculAjoutLongueur(serpent.coordonnee.size()-1, 40));
         serpent.coordonnee.resize(i);
         break;
      }
   }

}
unsigned Snake::calculAjoutLongueur(unsigned longu, unsigned pourcentage){
   double i =  ((double)longu/100.)*(double)pourcentage;
   return (unsigned)std::round(i);
}
void Snake::setCoordX(unsigned x) {
   coordonnee.at(0).x = x;
}

void Snake::setCoordY(unsigned y) {
   coordonnee.at(0).y = y;
}

unsigned Snake::getCoordX() {
   return this->coordonnee.at(0).x;
}

unsigned Snake::getCoordY() {
   return this->coordonnee.at(0).y;
}

std::vector<coordonneesXY> Snake::getCoord(){
   return this->coordonnee;
}

void Snake::longueurAAjouterSupl(unsigned valeur){
   longueurAAjouter += valeur;
}

void Snake::deplacerVersXY(unsigned int x, unsigned int y) {

   if(getCoordX() != x || getCoordY() != y){

      int diffX = abs((int)getCoordX() - (int)x);
      int diffY = abs((int)getCoordY() - (int)y);

      
      
      if(getCoordX() <= x && getCoordY() <= y){
         if(diffX < diffY){
            deplacerVers(Direction::bas);
         }
         else{
            deplacerVers(Direction::droite);
         }
      }
      else if(getCoordX() >= x && getCoordY() <= y){
         if(diffX < diffY){
            deplacerVers(Direction::bas);
         }
         else{
            deplacerVers(Direction::gauche);
         }
      }
      else if(getCoordX() >= x && getCoordY() >= y){
         if(diffX < diffY){
            deplacerVers(Direction::haut);
         }
         else{
            deplacerVers(Direction::gauche);
         }
      }
      else if(getCoordX() <= x && getCoordY() >= y){
         if(diffX < diffY){
            deplacerVers(Direction::haut);
         }
         else{
            deplacerVers(Direction::droite);
         }
      }
   }
}

void Snake::tuerSerpent(Snake& serpent){
   this->estEnVie = false;
   serpent.longueurAAjouterSupl(calculAjoutLongueur( this->coordonnee.size(), 60));
}

void Snake::deplacerVers(Direction dir){
   coordonneesXY tmpCoord = coordonnee.back();
   if(coordonnee.size() > 1){
      for(int i = coordonnee.size()-1; i >= 1 ; --i){
         coordonnee.at(i) = coordonnee.at(i-1);
      }
   }
   
   switch(dir){
      case Direction::haut :
         --coordonnee.at(0).y;
      break;         
      case Direction::bas :
         ++coordonnee.at(0).y;
      break;
      case Direction::droite :
         ++coordonnee.at(0).x;
      break;
      case Direction::gauche :
         --coordonnee.at(0).x;
      break;
   }

   if(this->longueurAAjouter){
      agrandirSerpent(tmpCoord);
      this->longueurAAjouter -= 1;
   }
}
void Snake::agrandirSerpent(coordonneesXY coord){
   this->coordonnee.push_back(coord);
}

unsigned Snake::getId() {
   return id;
}

bool Snake::getEstEnVie(){
   return this->estEnVie;
}