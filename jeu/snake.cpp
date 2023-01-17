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
   //le serpent qui appelle la fonction et celui qui est en paramètre est celui qui reçoit la longueur
   
}
unsigned Snake::calculAjoutLongueur(unsigned longueur, unsigned pourcentage){
   return 0;
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
   serpent.longueurAAjouterSupl(this->coordonnee.size());
}

void Snake::deplacerVers(Direction dir){

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
      agrandirSerpent(dir);
      this->longueurAAjouter -= 1;
   }
}
void Snake::agrandirSerpent(Direction dir){
   coordonneesXY coord = this->coordonnee.back();

   switch(dir){
         case Direction::haut :
            ++coord.y;
         break;
         case Direction::bas :
            --coord.y;
         break;
         case Direction::droite :
            --coord.x;
         break;
         case Direction::gauche :
            ++coord.x;
         break;
   }
   this->coordonnee.push_back(coord);
}

unsigned Snake::getId() {
   return id;
}

bool Snake::getEstEnVie(){
   return this->estEnVie;
}