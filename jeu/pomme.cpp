/*
---------------------------------------------------------------------------
Fichier     : pomme.cpp
Nom du labo : Labo8 - Snake
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 10.01.2022
But         : Définition des fonctions membres de la classe pomme.

Remarque(s) :

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#include "pomme.hpp"

Pomme::Pomme(unsigned x, unsigned y, const unsigned id,
             bool estMangee, std::array<short, 4> coul) : id(id), estMangee(estMangee),couleurs(coul){
   coordonnes.at(0) = x;
   coordonnes.at(1) = y;
   valeur=1;

}

unsigned Pomme::getCoordX() {
   return coordonnes.at(0);
}

unsigned Pomme::getCoordY() {
   return coordonnes.at(1);
}

unsigned Pomme::getValeur(){
   return valeur;
}
// TODO
void Pomme::nouvellePomme(unsigned x, unsigned y) {
   coordonnes.at(0) = x;
   coordonnes.at(1) = y;

}

unsigned Pomme::getId() {
   return id;
}
