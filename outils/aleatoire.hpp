/*
---------------------------------------------------------------------------
Fichier     : aleatoire.hpp
Nom du labo : Labo8 - Snakes
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 11.02.2023
But         : Déclaration de la fonction aleatoireEntreDeuxEntiersPositifs.

Remarque(s) :

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#ifndef LABO8_SNAKES_OUTILS_ALEATOIRE_HPP
#define LABO8_SNAKES_OUTILS_ALEATOIRE_HPP

/**
 * @name aleatoireEntreDeuxEntiersPositifs
 * @brief Génerer un nombre aléatoire dans l'intervalle entre deux entiers positif
 *        reçu en paramètre.
 * @param min un entier non signé. Borne inférieur de l'intervalle.
 * @param max un entier non signé. Borne supérieur de l'intervalle.
 * @return
 */
unsigned aleatoireEntreDeuxEntiersPositifs(unsigned min, unsigned max);

#endif