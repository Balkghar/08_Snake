/*
---------------------------------------------------------------------------
Fichier     : cycles.hpp
Nom du labo : Labo8 - Snake
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 10.01.2023
But         : Compteur très bon marché pour chronométrer des morceaux de
              code de l'ordre de la microseconde (profil).

Remarque(s) : Sur x86, le compteur de cycles du processeur (rdtsc, ~20
              cycles, sans appel système quelle que soit la source
              d'horloge du noyau) ; ailleurs, l'horloge monotone. Les
              valeurs ne servent qu'à des proportions.

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#ifndef LABO8_SNAKES_OUTILS_CYCLES_HPP
#define LABO8_SNAKES_OUTILS_CYCLES_HPP

#include <chrono>
#include <cstdint>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <x86intrin.h>
#endif
inline std::uint64_t compteurCycles() { return __rdtsc(); }
#else
inline std::uint64_t compteurCycles() {
  return std::uint64_t(std::chrono::steady_clock::now().time_since_epoch().count());
}
#endif

#endif
