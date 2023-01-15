/*
  ---------------------------------------------------------------------------
  Fichier     : saisie.cpp
  Nom du labo : Labo8 - snake
  Auteur(s)   : Delétraz Alexandre - Germano Hugo
  Date        : 11.01.2023
  But         : Définition des fonctions nécessaires à la saisie sécurisée d'un
                entier.

  Remarque(s) :

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

template <typename T, typename U>
T saisirIntervalle(T minInt,
                   U maxInt,
                   const string &message,
                   const string &message_erreur){

   T saisie;
   bool erreur;

   //continue jusqu'à ce que les conditions soient remplies
   do {

      // message et saisie
      cout << message;
      cin  >> saisie;

      erreur = cin.fail() or saisie < minInt or saisie > maxInt;

      if ( erreur ) {

         cout << message_erreur << endl;
         cin.clear();

      }

      viderBuffer();

   } while(erreur);

   return saisie;

}