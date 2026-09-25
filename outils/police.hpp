/*
---------------------------------------------------------------------------
Fichier     : police.hpp
Nom du labo : Labo8 - Snake
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 10.01.2023
But         : Police matricielle 5x7 minimale (chiffres, lettres majuscules,
              ponctuation courante) pour écrire du texte avec SDL sans
              dépendre de SDL_ttf ni d'un fichier de police.

Remarque(s) : Les minuscules sont affichées en majuscules ; les caractères
              inconnus (accents compris) sont laissés vides.

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#ifndef LABO8_SNAKES_OUTILS_POLICE_HPP
#define LABO8_SNAKES_OUTILS_POLICE_HPP

#include <cstdint>

const int LARGEUR_GLYPHE = 5;
const int HAUTEUR_GLYPHE = 7;

/**
 * @brief Motif du caractère c : 7 lignes de 5 bits (bit 4 = colonne gauche),
 *        ou nullptr si le caractère n'a pas de motif (espace, inconnu).
 */
const std::uint8_t *glyphe(char c);

#endif
