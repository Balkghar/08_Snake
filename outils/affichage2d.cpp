/*
---------------------------------------------------------------------------
Fichier     : affichage2d.cpp
Nom du labo : Labo8 - Snake
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 10.01.2023
But         : Permet d'afficher une fenêtre d'une taille personnalisée en utilisant
              la libraire SDL (simple DirectMedia Layer)

Remarque(s) :

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#include <algorithm>
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

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    cout << "SDL not ready ... quitting" << endl;
    return true;
  }

  // Agrandissement au plus proche voisin : chaque case reste un carré net
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

  SDL_CreateWindowAndRenderer(int(largeur * nbre_values),
                              int(hauteur * nbre_values),
                              SDL_WINDOW_SHOWN,
                              &window,
                              &renderer
  );
  if (window == nullptr or renderer == nullptr) {
    cout << "SDL not ready ... quitting" << endl;
    return true;
  }

  // Le terrain est dessiné dans un tampon mémoire (1 pixel = 1 case) puis
  // envoyé en une seule fois à la carte graphique à chaque image, au lieu
  // d'un appel SDL par point.
  texture = SDL_CreateTexture(renderer,
                              SDL_PIXELFORMAT_ARGB8888,
                              SDL_TEXTUREACCESS_STREAMING,
                              int(largeur),
                              int(hauteur));
  format = SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888);
  if (texture == nullptr or format == nullptr) {
    cout << "SDL not ready ... quitting" << endl;
    return true;
  }

  pixels.assign(size_t(largeur) * hauteur, 0);
  debutImage = SDL_GetTicks();
  return false;
}

bool Affichage2d::ajouterElementAffichage(int x, int y, Couleur couleur) {

  if (x < 0 or y < 0 or unsigned(x) >= largeur or unsigned(y) >= hauteur) {
    return true;
  }

  const unsigned ux = unsigned(x), uy = unsigned(y);
  pixels[size_t(uy) * largeur + ux] = valeurCouleur(couleur);

  if (zoneModifiee) {
    zoneMinX = std::min(zoneMinX, ux);
    zoneMinY = std::min(zoneMinY, uy);
    zoneMaxX = std::max(zoneMaxX, ux);
    zoneMaxY = std::max(zoneMaxY, uy);
  } else {
    zoneModifiee = true;
    zoneMinX = zoneMaxX = ux;
    zoneMinY = zoneMaxY = uy;
  }

  return false;
}

bool Affichage2d::nettoyerAffichage(Couleur couleur) {

  std::fill(pixels.begin(), pixels.end(), valeurCouleur(couleur));

  zoneModifiee = true;
  zoneMinX = zoneMinY = 0;
  zoneMaxX = largeur - 1;
  zoneMaxY = hauteur - 1;

  return false;
}

bool Affichage2d::fermetureDemandee() {

  // Vide toute la file d'événements, sinon elle s'accumule et la fenêtre
  // ne répond plus
  bool quitter = false;
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      quitter = true;
    }
  }

  return quitter;
}

bool Affichage2d::fermerAffichage() {

  // clear SDL ressources (le renderer avant la fenêtre qui le possède)
  SDL_FreeFormat(format);
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  format = nullptr;
  texture = nullptr;
  renderer = nullptr;
  window = nullptr;

  return false;
}

bool Affichage2d::mettreAjourAffichage() {

  // Le tampon est conservé d'une image à l'autre : seule la zone modifiée
  // est renvoyée à la texture (rien du tout si rien n'a bougé)
  if (zoneModifiee) {
    const SDL_Rect zone = {int(zoneMinX), int(zoneMinY),
                           int(zoneMaxX - zoneMinX + 1),
                           int(zoneMaxY - zoneMinY + 1)};
    SDL_UpdateTexture(texture, &zone,
                      &pixels[size_t(zoneMinY) * largeur + zoneMinX],
                      int(largeur * sizeof(Uint32)));
    zoneModifiee = false;
  }

  // Le back buffer est indéfini après un Present : on recopie la texture
  // entière, ce qui ne coûte qu'une opération côté GPU
  SDL_RenderCopy(renderer, texture, nullptr, nullptr);
  SDL_RenderPresent(renderer);

  // Cadence fixe : on n'attend que le temps restant de l'image, le temps
  // de calcul et de rendu est donc déjà compté dans le délai
  const Uint32 ecoule = SDL_GetTicks() - debutImage;
  if (ecoule < sdl_delay) {
    SDL_Delay(sdl_delay - ecoule);
  }
  debutImage = SDL_GetTicks();

  return false;
}


//=========================== partie privée ===============================

//--------------------------- gestion couleur -----------------------------
Uint32 Affichage2d::valeurCouleur(Couleur couleur) const {

  switch (couleur) {
    case Couleur::rouge:return SDL_MapRGB(format, 255, 0, 0);
    case Couleur::noir:return SDL_MapRGB(format, 0, 0, 0);
    case Couleur::blanc:
    default:return SDL_MapRGB(format, 255, 255, 255);
  }
}
