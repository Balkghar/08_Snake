/*
  ---------------------------------------------------------------------------
  Fichier     : main.cpp
  Nom du labo : Labo8 - Snake
  Auteur(s)   : Delétraz Alexandre - Germano Hugo
  Date        : 10.01.2022
  But         : 

  Remarque(s) :

  Compilateur : gcc version 11.2.0
  ---------------------------------------------------------------------------
*/

#include <iostream>
#include <cstdlib>
#include <limits>
#include <outils/saisie.hpp>

#include "jeu/combatSnakes.hpp"

using namespace std;

//-----------------------------------------------------------------------------
int main(int argv, char **args) {

  // Les paramètres argv et args sont indispensables pour SDL2 sur Windows
  (void) argv;       // On utilise ces aruments pour
  (void) args;       // éviter un warning

  //=========================== Constantes ===================================

  //--------------------------- Numériques -----------------------------------
  const unsigned MIN_SERPENT = 2;
  const unsigned MAX_SERPENT = 1'000;
  const unsigned MIN_HAUTEUR = 50;
  const unsigned MAX_HAUTEUR = 800;
  const unsigned MIN_LARGEUR = 50;
  const unsigned MAX_LARGEUR = 1200;

  //------------------------------ Textes -----------------------------------------
  const string MSG_LARG_TERRAIN = "Veuillez choisir la largeur du terrain de "s
      "combat.\n"s +
      "Elle doit etre comprise entre "s +
      to_string(MIN_LARGEUR) +
      " et "s +
      to_string(MAX_LARGEUR) +
      " : "s;
  const string MSG_HAUT_TERRAIN = "Veuillez choisir la hauteur du terrain de "s
      "combat"s
          ".\n"s +
      "Elle doit etre comprise entre "s +
      to_string(MIN_HAUTEUR) +
      " et "s +
      to_string(MAX_HAUTEUR) +
      " : "s;
  const string MSG_ERR_TAILLE = "Taille invalide, veuillez recommencer"s;

  const string MSG_SAISIE_SERPENT = "Choisisez combien de serpents devront "s
      "combattre.\n"s +
      "Ce nombre doit etre compris entre "s +
      to_string(MIN_SERPENT) +
      " et "s +
      to_string(MAX_SERPENT) +
      " : "s;

  const string MSG_ERR_SERPENT = "Valeur invalide, veuillez recommencer"s;

  const string MSG_DEBUT = "Bienvenue dans Snake battle simulator© de la "s
      "Green Katze Korporation™ !\n"s
          "Ici, vous pouvez faire combattre à mort des "s
              "serpents innocents de tous crimes, jusqu'à "s
                  "ce qu'il n'en reste plus qu'un !\n"s;
  const string MSG_FIN = "La Green Katze Korporation™ vous remercie "s
      "d'avoir utilisé Snake battle simulator©.\n"s
          "Grâce à votre simulation, nous avons pu "s
              "récolter de précieuses informations pour "s
                  "l'avenir de l'humanité.\n"s +
      "Veuillez appuyer sur ENTER pour quitter."s;

  //======================== Début du programme ===================================

  Combat combat((unsigned) saisirIntervalle(MIN_LARGEUR, MAX_LARGEUR,
                                            MSG_LARG_TERRAIN, MSG_ERR_TAILLE),
                (unsigned) saisirIntervalle(MIN_HAUTEUR, MAX_HAUTEUR,
                                            MSG_HAUT_TERRAIN, MSG_ERR_TAILLE),
                (unsigned) saisirIntervalle(MIN_SERPENT, MAX_SERPENT,
                                            MSG_SAISIE_SERPENT,
                                            MSG_ERR_SERPENT));

  combat.commencerCombat();
  return EXIT_SUCCESS;
}
