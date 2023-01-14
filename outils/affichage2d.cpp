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


void Affichage2d::confCouleur(Couleur couleur){

   switch (couleur)
   {
   case Couleur::rouge:
      SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
      break;
   case Couleur::blanc:
      SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
      break;
   case Couleur::noir:
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
      break;
   }
}
bool Affichage2d::initalisationAffichage (){
   //--------------------------------------------------------------------------
   //    SDL settings
   //--------------------------------------------------------------------------
   const int  SCREEN_WIDTH  =  this->largeur;
   const int  SCREEN_HEIGTH =  this->hauteur;

   SDL_Init(SDL_INIT_VIDEO);
   SDL_CreateWindowAndRenderer(SCREEN_WIDTH*this->nbre_values, SCREEN_HEIGTH*this->nbre_values, SDL_WINDOW_SHOWN, &window, &renderer);
   if (window == nullptr or renderer == nullptr) {
      cout << "SDL not ready ... quitting" << endl;
      return true;
   }
   //SDL_RenderSetScale(renderer, (float)SCREEN_WIDTH/(float)this->nbre_values, (float)SCREEN_HEIGTH/(float)this->nbre_values);
   SDL_RenderSetScale(renderer, (float)this->nbre_values, (float)this->nbre_values);
   return false;
}
bool Affichage2d::nettoyerAffichage(Couleur couleur){
   SDL_PollEvent(&event);
   if (event.type == SDL_QUIT) {
      appIsRunning = false;
      return true;
   }
            
   confCouleur(couleur);

   SDL_RenderClear(renderer);

   return false;
}
bool Affichage2d::ajouterElementAffichage(unsigned x, unsigned y, Couleur couleur){


   confCouleur(couleur);

   SDL_RenderDrawPoint(renderer, x, y);

   return false;
}

bool Affichage2d::mettreAjourAffichage(){

   // SDL display the window
   SDL_RenderPresent(renderer);
   SDL_Delay(this->sdl_delay);
   return false;
}
bool Affichage2d::fermerAffichage (){

   // clear SDL ressources
   SDL_DestroyWindow(window);
   SDL_DestroyRenderer(renderer);
   SDL_Quit();

   return false;
}