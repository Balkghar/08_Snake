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

Pomme::Pomme(std::array<unsigned int, 2> coord, std::array<short, 4> coul, const int id,
             bool estMangee) : coordonnes(coord), couleurs(coul), id(id), estMangee(estMangee){

}
