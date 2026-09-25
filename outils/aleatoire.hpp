/*
---------------------------------------------------------------------------
Fichier     : aleatoire.hpp
Nom du labo : Labo8 - Snakes
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 11.02.2023
But         : Déclaration de la fonction aleatoireEntreDeuxValeurs.

Remarque(s) :

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#ifndef LABO8_SNAKES_OUTILS_ALEATOIRE_HPP
#define LABO8_SNAKES_OUTILS_ALEATOIRE_HPP

#include <cstdint>

/**
 * @name aleatoireEntreDeuxValeurs
 * @brief Génerer un nombre aléatoire dans l'intervalle entre deux entiers positif
 *        reçu en paramètre.
 * @param min un entier non signé. Borne inférieur de l'intervalle.
 * @param max un entier non signé. Borne supérieur de l'intervalle.
 * @return
 */


int aleatoireEntreDeuxValeurs(int min, int max);

/**
 * @brief Nombre aléatoire de 64 bits, tiré du même générateur, pour
 *        initialiser d'autres suites aléatoires.
 */
std::uint64_t graineAleatoire();

/**
 * @brief Fixe la graine de toute la partie : mêmes paramètres et même graine
 *        donnent exactement la même partie, quel que soit le nombre de
 *        threads. À appeler avant de créer le combat.
 */
void fixerGraine(std::uint64_t valeur);

/**
 * @brief Graine de la partie (donnée par fixerGraine ou tirée au hasard).
 */
std::uint64_t graineDePartie();

#endif