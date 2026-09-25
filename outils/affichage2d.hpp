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

#include <string>
#include <vector>

enum Couleur { blanc, noir, rouge };

class Affichage2d {
  //-------------------------- Constructeur --------------------------------
 public:
  Affichage2d(const unsigned larg,
              const unsigned haut,
              const unsigned sdl_del,
              const unsigned nbr_vals
  );

  //------------------------- méthode d'affichage -------------------------
  bool initalisationAffichage();
  bool ajouterElementAffichage(int x, int y, Couleur couleur);
  bool nettoyerAffichage(Couleur couleur);
  bool fermetureDemandee();
  bool fermerAffichage();
  bool mettreAjourAffichage();

  /**
   * @brief Assombrit le terrain et affiche un panneau de texte par-dessus,
   *        jusqu'à ce que l'utilisateur appuie sur une touche ou ferme la
   *        fenêtre. La première ligne est le titre ; les lignes commençant
   *        par "--" sont des intertitres. Les cases enValeur (en cases du
   *        terrain) restent en vert vif par-dessus l'assombrissement.
   */
  void afficherEcranFin(const std::vector<std::string> &lignes,
                        const std::vector<SDL_Point> &enValeur);
 private:
  void envoyerZoneModifiee();
  void dessinerTexte(int x, int y, const std::string &texte, int echelle,
                     SDL_Color couleur);

  Uint32 valeurCouleur(Couleur couleur) const;
  Uint32 couleurs[3] = {0, 0, 0};  // indexé par Couleur, format de la texture

  //------------------------- Données -------------------------------------
  const unsigned largeur;
  const unsigned hauteur;
  const unsigned sdl_delay;
  unsigned nbre_values;  // zoom : taille d'une case en pixels
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  SDL_Texture *texture = nullptr;
  SDL_PixelFormat *format = nullptr;
  std::vector<Uint32> pixels;
  Uint32 debutImage = 0;

  // Zone modifiée depuis la dernière image (seule partie envoyée au GPU)
  bool zoneModifiee = false;
  unsigned zoneMinX = 0, zoneMinY = 0, zoneMaxX = 0, zoneMaxY = 0;
};

#endif