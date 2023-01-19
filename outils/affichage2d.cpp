/*
---------------------------------------------------------------------------
Fichier     : affichage2d.cpp
Nom du labo : Labo8 - Snake
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 10.01.2022
But         : Permet d'afficher une fenêtre d'une taille personnalisée en utilisant
              la libraire SDL (simple DirectMedia Layer)

Remarque(s) :

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#include <iomanip>
#include <iostream>
#include "affichage2d.hpp"

using namespace std;


// SDL library


//=========================== Partie public ===============================

//--------------------------- constructeur --------------------------------
Affichage2d::Affichage2d(const unsigned int larg,
                         const unsigned int haut,
                         const unsigned int sdl_del,
                         const unsigned int nbr_vals
) : largeur(larg), hauteur(haut), sdl_delay(sdl_del), nbre_values(nbr_vals) {}

//------------------------- méthode d'affichage -------------------------
bool Affichage2d::initalisationAffichage() {
  //--------------------------------------------------------------------------
  //    SDL settings
  //--------------------------------------------------------------------------
  const unsigned SCREEN_WIDTH = largeur;
  const unsigned SCREEN_HEIGTH = hauteur;

  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(int(SCREEN_WIDTH * nbre_values),
                              int(SCREEN_HEIGTH * nbre_values),
                              SDL_WINDOW_SHOWN,
                              &window,
                              &renderer
  );
  if (window == nullptr or renderer == nullptr) {
    cout << "SDL not ready ... quitting" << endl;
    return true;
  }

  SDL_RenderSetScale(renderer, (float) nbre_values, (float) nbre_values);
  return false;
}

bool Affichage2d::ajouterElementAffichage(int x, int y, Couleur couleur) {

  confCouleur(couleur);

  SDL_RenderDrawPoint(renderer, x, y);

  return false;
}

bool Affichage2d::nettoyerAffichage(Couleur couleur) {
  SDL_PollEvent(&event);
  if (event.type == SDL_QUIT) {
    return true;
  }

  confCouleur(couleur);

  SDL_RenderClear(renderer);

  return false;
}

bool Affichage2d::fermerAffichage() {

  // clear SDL ressources
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();

  return false;
}

bool Affichage2d::mettreAjourAffichage() {

  // SDL display the window
  SDL_RenderPresent(renderer);
  SDL_Delay(sdl_delay);
  return false;
}


//=========================== partie privée ===============================

//--------------------------- gestion couleur -----------------------------
void Affichage2d::confCouleur(Couleur couleur) {

  switch (couleur) {
    case Couleur::rouge:SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
      break;
    case Couleur::blanc:SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
      break;
    case Couleur::noir:SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
      break;
  }
}