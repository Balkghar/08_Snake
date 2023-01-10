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
#include <vector>
#include <random>
#include <algorithm>
#include "snake.hpp"

#include <SDL2/SDL.h>

using namespace std;


//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) {

   //--------------------------------------------------------------------------
   // int argc, char* argv[] are important for SDL2 on Windows
   (void)argc;       // use these arguments
   (void)argv;       // .. to avoid a warning

   //--------------------------------------------------------------------------
   //    SDL settings
   //--------------------------------------------------------------------------
   const int  SCREEN_WIDTH  =  800;
   const int  SCREEN_HEIGTH =  600;
   const int  NBRE_VALUES   =   50;
   const int  SDL_DELAY     =  100;

   // SDL library
   SDL_Window*    window         = nullptr;
   SDL_Renderer*  renderer       = nullptr;
   SDL_Event      event;
   bool           appIsRunning   = true;

   SDL_Init(SDL_INIT_VIDEO);
   SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGTH, SDL_WINDOW_SHOWN, &window, &renderer);
   if (window == nullptr or renderer == nullptr) {
      cout << "SDL not ready ... quitting" << endl;
      return EXIT_FAILURE;
   }
   SDL_RenderSetScale(renderer, (float)SCREEN_WIDTH/(float)NBRE_VALUES, (float)SCREEN_HEIGTH/(float)NBRE_VALUES);
   //--------------------------------------------------------------------------

   

   cout << "press ENTER to quit ...";
   cin.ignore(numeric_limits<streamsize>::max(), '\n');

   // clear SDL ressources
   SDL_DestroyWindow(window);
   SDL_DestroyRenderer(renderer);
   SDL_Quit();
   
   return EXIT_SUCCESS;
}
