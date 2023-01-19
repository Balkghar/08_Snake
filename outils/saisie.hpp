/*
  ---------------------------------------------------------------------------
  Fichier     : saisie.hpp
  Nom du labo : Labo7 - Robots
  Auteur(s)   : Delétraz Alexandre - Germano Hugo
  Date        : 13.12.2023
  But         : Déclaration des fonctions nécessaires à la saisie sécurisée d'un
                entier.

  Remarque(s) :

  Compilateur : gcc version 11.2.0
  ---------------------------------------------------------------------------
*/

#ifndef LABO8_SNAKES_OUTILS_SAISIE_HPP
#define LABO8_SNAKES_OUTILS_SAISIE_HPP

#include <string>

void viderBuffer();

int saisirIntervalle(int minInt,
                     int maxInt,
                     const std::string &message,
                     const std::string &message_erreur
);

#endif
