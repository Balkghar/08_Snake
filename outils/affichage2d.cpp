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
#include "police.hpp"

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

  // Réduit le zoom si la fenêtre ne tient pas à l'écran
  SDL_Rect ecran;
  if (SDL_GetDisplayUsableBounds(0, &ecran) == 0) {
    const unsigned zoomDemande = nbre_values;
    while (nbre_values > 1 and (largeur * nbre_values > unsigned(ecran.w)
        or hauteur * nbre_values > unsigned(ecran.h))) {
      --nbre_values;
    }
    if (nbre_values != zoomDemande) {
      cout << "Zoom reduit a " << nbre_values << " pour tenir a l'ecran" << endl;
    }
  }

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

  couleurs[Couleur::blanc] = SDL_MapRGB(format, 255, 255, 255);
  couleurs[Couleur::noir] = SDL_MapRGB(format, 0, 0, 0);
  couleurs[Couleur::rouge] = SDL_MapRGB(format, 255, 0, 0);

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

  envoyerZoneModifiee();

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


void Affichage2d::afficherEcranFin(const std::vector<std::string> &lignes,
                                   const std::vector<SDL_Point> &enValeur) {

  if (lignes.empty()) {
    return;
  }
  envoyerZoneModifiee();

  // Dimensions du panneau pour une échelle de texte de 1 (un caractère fait
  // 5x7 pixels, plus 1 d'espacement et 2 d'interligne ; le titre est 2x)
  const int PAS_X = LARGEUR_GLYPHE + 1;
  const int PAS_Y = HAUTEUR_GLYPHE + 2;
  const int MARGE = 6;
  size_t maxCaracteres = lignes[0].size() * 2;
  for (const std::string &ligne : lignes) {
    maxCaracteres = std::max(maxCaracteres, ligne.size());
  }
  const int panneauL = int(maxCaracteres) * PAS_X + 2 * MARGE;
  const int panneauH = PAS_Y * 2 + int(lignes.size() - 1) * PAS_Y + 2 * MARGE;

  // Plus grande échelle qui tient dans 90 % de la fenêtre ; si même
  // l'échelle 1 ne tient pas (petit terrain), on agrandit la fenêtre
  int fenL = 0, fenH = 0;
  SDL_GetRendererOutputSize(renderer, &fenL, &fenH);
  int echelle = std::max(1, std::min(fenL * 9 / 10 / panneauL,
                                     fenH * 9 / 10 / panneauH));
  if (panneauL * 10 > fenL * 9 or panneauH * 10 > fenH * 9) {
    const int facteur = std::max((panneauL * 10 + fenL * 9 - 1) / (fenL * 9),
                                 (panneauH * 10 + fenH * 9 - 1) / (fenH * 9));
    if (facteur > 1) {
      SDL_SetWindowSize(window, fenL * facteur, fenH * facteur);
      SDL_GetRendererOutputSize(renderer, &fenL, &fenH);
    }
    echelle = 1;
  }

  // Cases mises en valeur (le vainqueur), à l'échelle de la fenêtre
  const int caseL = std::max(1, fenL / int(largeur));
  const int caseH = std::max(1, fenH / int(hauteur));
  std::vector<SDL_Rect> cases;
  cases.reserve(enValeur.size());
  for (const SDL_Point &p : enValeur) {
    cases.push_back({p.x * caseL, p.y * caseH, caseL, caseH});
  }

  // Le panneau se place (centre, côtés, coins) là où il cache le moins de
  // cases mises en valeur
  const int pL = panneauL * echelle, pH = panneauH * echelle;
  const int bord = std::max(0, std::min((fenL - pL), (fenH - pH)) / 20);
  const int xs[] = {(fenL - pL) / 2, bord, fenL - pL - bord};
  const int ys[] = {(fenH - pH) / 2, bord, fenH - pH - bord};
  SDL_Rect panneau = {xs[0], ys[0], pL, pH};
  size_t moinsCachees = cases.size() + 1;
  for (int y : ys) {
    for (int x : xs) {
      const SDL_Rect candidat = {x, y, pL, pH};
      size_t cachees = 0;
      for (const SDL_Rect &c : cases) {
        cachees += SDL_HasIntersection(&candidat, &c) ? 1 : 0;
      }
      if (cachees < moinsCachees) {
        moinsCachees = cachees;
        panneau = candidat;
      }
    }
  }

  const SDL_Color OR = {255, 200, 40, 255};
  const SDL_Color VERT = {80, 220, 120, 255};
  const SDL_Color BLANC = {235, 235, 235, 255};
  const SDL_Color GRIS = {150, 150, 150, 255};

  bool continuer = true;
  while (continuer) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);

    // Terrain assombri, panneau presque opaque
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 140);
    SDL_RenderFillRect(renderer, nullptr);

    SDL_SetRenderDrawColor(renderer, VERT.r, VERT.g, VERT.b, 255);
    SDL_RenderFillRects(renderer, cases.data(), int(cases.size()));

    SDL_SetRenderDrawColor(renderer, 15, 20, 25, 235);
    SDL_RenderFillRect(renderer, &panneau);
    SDL_SetRenderDrawColor(renderer, OR.r, OR.g, OR.b, 255);
    SDL_RenderDrawRect(renderer, &panneau);

    // Titre centré en double taille, puis les lignes alignées à gauche
    const int titreL = int(lignes[0].size()) * PAS_X * 2 * echelle;
    int y = panneau.y + MARGE * echelle;
    dessinerTexte(panneau.x + (panneau.w - titreL) / 2, y, lignes[0],
                  2 * echelle, OR);
    y += PAS_Y * 2 * echelle;
    for (size_t i = 1; i < lignes.size(); ++i) {
      const bool intertitre = lignes[i].rfind("--", 0) == 0;
      const bool derniere = i + 1 == lignes.size();
      dessinerTexte(panneau.x + MARGE * echelle, y, lignes[i], echelle,
                    intertitre ? VERT : (derniere ? GRIS : BLANC));
      y += PAS_Y * echelle;
    }

    SDL_RenderPresent(renderer);

    SDL_Event event;
    if (SDL_WaitEventTimeout(&event, 250)) {
      if (event.type == SDL_QUIT) {
        continuer = false;
      } else if (event.type == SDL_KEYDOWN) {
        const SDL_Keycode touche = event.key.keysym.sym;
        continuer = not (touche == SDLK_ESCAPE or touche == SDLK_RETURN
            or touche == SDLK_KP_ENTER or touche == SDLK_SPACE);
      }
    }
  }
}

//=========================== partie privée ===============================

//--------------------------- envoi au GPU --------------------------------
void Affichage2d::envoyerZoneModifiee() {

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
}

//--------------------------- texte ---------------------------------------
void Affichage2d::dessinerTexte(int x, int y, const std::string &texte,
                                int echelle, SDL_Color couleur) {

  // Un rectangle par pixel allumé, tous envoyés en un seul appel
  std::vector<SDL_Rect> rects;
  for (size_t i = 0; i < texte.size(); ++i) {
    const std::uint8_t *motif = glyphe(texte[i]);
    if (motif == nullptr) {
      continue;
    }
    const int x0 = x + int(i) * (LARGEUR_GLYPHE + 1) * echelle;
    for (int ligne = 0; ligne < HAUTEUR_GLYPHE; ++ligne) {
      for (int col = 0; col < LARGEUR_GLYPHE; ++col) {
        if (motif[ligne] & (1 << (LARGEUR_GLYPHE - 1 - col))) {
          rects.push_back({x0 + col * echelle, y + ligne * echelle,
                           echelle, echelle});
        }
      }
    }
  }
  SDL_SetRenderDrawColor(renderer, couleur.r, couleur.g, couleur.b, couleur.a);
  SDL_RenderFillRects(renderer, rects.data(), int(rects.size()));
}

//--------------------------- gestion couleur -----------------------------
Uint32 Affichage2d::valeurCouleur(Couleur couleur) const {
  return couleurs[couleur];
}
