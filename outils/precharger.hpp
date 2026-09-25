/*
---------------------------------------------------------------------------
Fichier     : precharger.hpp
Nom du labo : Labo8 - Snake
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 10.01.2023
But         : Demande au processeur de charger une adresse en cache avant
              qu'on en ait besoin (préchargement logiciel).

Remarque(s) : Utile quand on parcourt une liste d'adresses éparpillées connue
              à l'avance : les défauts de cache se recouvrent au lieu de
              s'additionner. Sans GCC ni Clang, la fonction ne fait rien.

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#ifndef LABO8_SNAKES_OUTILS_PRECHARGER_HPP
#define LABO8_SNAKES_OUTILS_PRECHARGER_HPP

inline void precharger(const void *adresse) {
#if defined(__GNUC__) || defined(__clang__)
  __builtin_prefetch(adresse);
#else
  (void) adresse;
#endif
}

#endif
