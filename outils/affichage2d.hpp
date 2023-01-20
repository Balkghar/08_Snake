/*
---------------------------------------------------------------------------
Fichier     : affichage2d.hpp
Nom du labo : Labo8 - Snake
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 10.01.2023
But         : Déclaration des fonctions qui permettent d'afficher une fenêtre d'une
              taille personnalisée en utilisant la libraire SDL
              (simple DirectMedia Layer)

Remarque(s) :

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#ifndef LABO8_SNAKES_OUTILS_AFFICHAGE2D_HPP
#define LABO8_SNAKES_OUTILS_AFFICHAGE2D_HPP

#if defined(__linux__)  // Or #if __linux__
#include <SDL2/SDL.h>
#elif _WIN32
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

enum class Couleur { blanc, noir, rouge };

class Affichage2d {
  //-------------------------- Constructeur --------------------------------
 public:
  Affichage2d(const unsigned larg,
              const unsigned haut,
              const unsigned sdl_del,
              const unsigned nbr_vals
  );

  //------------------------- méthode d'affichage -------------------------
  void initalisationAffichage();
  void ajouterElementAffichage(int x, int y, Couleur couleur);
  void nettoyerAffichage(Couleur couleur);
  void fermerAffichage();
  void mettreAjourAffichage();
 private:
  void configCouleur(Couleur couleur);

  //------------------------- Données -------------------------------------
  const unsigned largeur;
  const unsigned hauteur;
  const unsigned sdl_delay;
  const unsigned nbre_values;
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  SDL_Event event;
};

#endif