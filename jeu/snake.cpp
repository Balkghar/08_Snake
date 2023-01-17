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

unsigned Snake::combattreSerpent(Snake& serpent){
   for(coordonneesXY coord: serpent.getCoord()){
      if(coord.x == serpent.getCoordX() && coord.y == serpent.getCoordY()){
         if(serpent.getCoordX() == this->getCoordX() && serpent.getCoordY() == this->getCoordY()){
            if(this->getCoord().size() < serpent.getCoord().size()){

               return 1;
            }
            else{

               return 2;
            }
         }
         else{
            this->longueurAAjouterSupl(serpent.couperSerpent(coord));
            return 0;
         }
      }
   }
   return 0;
}
unsigned Snake::couperSerpent(coordonneesXY& coord){
   unsigned i = coordonnee.size();
   for(std::vector<coordonneesXY>::iterator it = coordonnee.begin(); it != coordonnee.end(); ++it) {
      --i;
      if((*it).x == coord.x && (*it).y == coord.y){
         //coordonnee.erase(it, coordonnee.end());
         return 0;
      }
   }
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
// TODO
void Snake::deplacerVersXY(unsigned int x, unsigned int y) {

   if(getCoordX() != x || getCoordY() != y){

      unsigned diffX = abs(getCoordX() - x);
      unsigned diffY = abs(getCoordY() - y);

      
      
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
