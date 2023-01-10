/*
  ---------------------------------------------------------------------------
  Fichier     : saisie.cpp
  Nom du labo : Labo7 - Robots
  Auteur(s)   : Delétraz Alexandre - Germano Hugo
  Date        : 13.12.2022
  But         : Définition des fonctions nécessaires à la saisie sécurisée d'un
                entier.

  Remarque(s) : à compléter

  Compilateur : gcc version 11.2.0
  ---------------------------------------------------------------------------
*/

#include <iostream>
#include <limits>
#include "saisie.hpp"

using namespace std;

void viderBuffer() {
   cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

int saisirIntervaleInt(int minInt, int maxInt, const string& message, const string& message_erreur){

   int saisie;
   bool erreur;

   //continue jusque que le condition soit remplies
   do {

      // message et saisie
      cout << message;
      cin >> saisie;

      erreur = cin.fail() or saisie < minInt or saisie > maxInt;

      if ( erreur ) {

         cout << message_erreur << endl;
         cin.clear();

      }

      viderBuffer();

   } while(erreur);

   return saisie;

}