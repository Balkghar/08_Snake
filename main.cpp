/*
  ---------------------------------------------------------------------------
  Fichier     : main.cpp
  Nom du labo : Labo8 - Snake
  Auteur(s)   : Delétraz Alexandre - Germano Hugo
  Date        : 10.01.2023
  But         : Le but de ce programme est de simuler des serpents qui se déplacent
                vers une pomme. Lors de leurs déplacements, s'ils croisent un de
                leur congénère, ils vont se battre et le plus grand l'emportera.
                À chaque fois qu'un serpent mange la pomme qu'il cherche, une autre
                pomme apparaît ailleurs et le serpent se met en chemin pour aller la
                manger.

  Remarque(s) : Le style de format de code est importé de Google.

  Compilateur : gcc version 11.2.0
  ---------------------------------------------------------------------------
*/

#include <cstdlib>
#include <iostream>

#include "outils/saisie.hpp"
#include "jeu/combatSnakes.hpp"

using namespace std;

//-----------------------------------------------------------------------------
int main(int argv, char **args) {

   // Les paramètres argv et args sont indispensables pour SDL2 sur Windows
   (void) argv;       // On utilise ces arguments pour
   (void) args;       // éviter un warning

   //=========================== Constantes ===================================

   //--------------------------- Numériques -----------------------------------
   const unsigned MIN_SERPENT = 2;
   const unsigned MAX_SERPENT = 1'000;
   const unsigned MIN_LONGUEUR = 50;
   const unsigned MAX_LONGUEUR = 800;
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
   const string MSG_LONG_TERRAIN = "Veuillez choisir la longueur du terrain de "s
                                      "combat"s
                                         ".\n"s +
                                   "Elle doit etre comprise entre "s +
                                   to_string(MIN_LONGUEUR) +
                                   " et "s +
                                   to_string(MAX_LONGUEUR) +
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

   const string MSG_DEBUT = "Bienvenue dans Snake battle simulator de la "s
      "Green Katze Korporation !\n"s
         "Ici, vous pouvez faire combattre a mort des "s
            "serpents innocents de tous crimes, jusqu'a "s
               "ce qu'il n'en reste plus qu'un !\n"s;
   const string MSG_FIN = "La Green Katze Korporation vous remercie "s
                             "d'avoir utilise Snake battle simulator.\n"s
                                "Grace a votre simulation, nous avons pu "s
                                   "recolter de précieuses informations pour "s
                                      "l'avenir de l'humanite.\n"s +
                          "Veuillez appuyer sur ENTER pour quitter."s;

   //======================== Début du programme ===================================

   cout << MSG_DEBUT << endl;
   Combat combat((unsigned) saisirIntervalle(MIN_LARGEUR, MAX_LARGEUR,
                                             MSG_LARG_TERRAIN, MSG_ERR_TAILLE),
                 (unsigned) saisirIntervalle(MIN_LONGUEUR, MAX_LONGUEUR,
                                             MSG_LONG_TERRAIN, MSG_ERR_TAILLE),
                 (unsigned) saisirIntervalle(MIN_SERPENT, MAX_SERPENT,
                                             MSG_SAISIE_SERPENT,
                                             MSG_ERR_SERPENT));

   combat.commencerCombat();
   cout << MSG_FIN << endl;
   viderBuffer();
   return EXIT_SUCCESS;
}
