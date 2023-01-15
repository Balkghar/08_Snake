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

/**
 * @name aleatoireEntreDeuxValeurs
 * @brief Génerer un nombre aléatoire dans l'intervalle entre deux entiers positif
 *        reçu en paramètre.
 * @param min un entier non signé. Borne inférieur de l'intervalle.
 * @param max un entier non signé. Borne supérieur de l'intervalle.
 * @return
 */
template<typename T, typename U>
T aleatoireEntreDeuxValeurs(T min, U max );

#endif