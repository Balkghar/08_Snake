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
#include <cstring>
#include <iomanip>
#include <iostream>
#include "affichage2d.hpp"
#include "police.hpp"
#include "precharger.hpp"

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
void Affichage2d::activerVsync(bool actif) {
  vsync = actif;
}

unsigned Affichage2d::frequenceEcran() const {
  SDL_DisplayMode mode;
  const int ecran = window ? SDL_GetWindowDisplayIndex(window) : 0;
  if (SDL_GetCurrentDisplayMode(ecran < 0 ? 0 : ecran, &mode) == 0
      and mode.refresh_rate > 0) {
    return unsigned(mode.refresh_rate);
  }
  return 60;
}

bool Affichage2d::initalisationAffichage() {

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    cout << "SDL not ready ... quitting" << endl;
    return true;
  }

  // Agrandissement au plus proche voisin : chaque case reste un carré net
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
  SDL_SetHint(SDL_HINT_RENDER_VSYNC, vsync ? "1" : "0");

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
  bandes.assign((hauteur + HAUTEUR_BANDE - 1) / HAUTEUR_BANDE, Bande());
  debutImage = SDL_GetTicks();
  return false;
}

bool Affichage2d::ajouterElementAffichage(int x, int y, Couleur couleur) {

  if (x < 0 or y < 0 or unsigned(x) >= largeur or unsigned(y) >= hauteur) {
    return true;
  }

  const unsigned ux = unsigned(x), uy = unsigned(y);
  pixels[size_t(uy) * largeur + ux] = valeurCouleur(couleur);

  Bande &bande = bandes[uy / HAUTEUR_BANDE];
  if (bande.modifiee) {
    bande.minX = std::min(bande.minX, ux);
    bande.maxX = std::max(bande.maxX, ux);
  } else {
    bande.modifiee = true;
    bande.minX = bande.maxX = ux;
  }

  return false;
}

void Affichage2d::prechargerElement(int x, int y) const {
  if (x >= 0 and y >= 0 and unsigned(x) < largeur and unsigned(y) < hauteur) {
    precharger(&pixels[size_t(y) * largeur + unsigned(x)]);
  }
}

bool Affichage2d::nettoyerAffichage(Couleur couleur) {

  std::fill(pixels.begin(), pixels.end(), valeurCouleur(couleur));

  for (Bande &bande : bandes) {
    bande = {true, 0, largeur - 1};
  }

  return false;
}

bool Affichage2d::fermetureDemandee(int &accelerer) {

  // Vide toute la file d'événements, sinon elle s'accumule et la fenêtre
  // ne répond plus
  bool quitter = false;
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      quitter = true;
    } else if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:quitter = true;
          break;
        case SDLK_PLUS:
        case SDLK_KP_PLUS:
        case SDLK_EQUALS:  // touche du + sans majuscule
        case SDLK_UP:++accelerer;
          break;
        case SDLK_MINUS:
        case SDLK_KP_MINUS:
        case SDLK_DOWN:--accelerer;
          break;
        default:break;
      }
    }
  }

  return quitter;
}

void Affichage2d::definirTitre(const std::string &titre) {
  SDL_SetWindowTitle(window, titre.c_str());
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


void Affichage2d::afficherEcranFin(const MotifPixel &logo,
                                   const std::vector<std::string> &lignes,
                                   const std::vector<SDL_Point> &enValeur) {

  envoyerZoneModifiee();

  // Découpe des lignes : style (préfixe) et texte à afficher
  enum Style { gauche, titre, intertitre, centre, discret };
  std::vector<std::pair<Style, std::string>> textes;
  for (const std::string &ligne : lignes) {
    if (ligne.rfind("# ", 0) == 0) {
      textes.emplace_back(titre, ligne.substr(2));
    } else if (ligne.rfind("^ ", 0) == 0) {
      textes.emplace_back(centre, ligne.substr(2));
    } else if (ligne.rfind("~ ", 0) == 0) {
      textes.emplace_back(discret, ligne.substr(2));
    } else if (ligne.rfind("--", 0) == 0) {
      textes.emplace_back(intertitre, ligne);
    } else {
      textes.emplace_back(gauche, ligne);
    }
  }

  // Dimensions du panneau pour une échelle de 1 : un caractère fait 5x7
  // pixels, plus 1 d'espacement et 2 d'interligne ; titre et logo sont 2x
  const int PAS_X = LARGEUR_GLYPHE + 1;
  const int PAS_Y = HAUTEUR_GLYPHE + 2;
  const int MARGE = 6;
  const int PIXEL_LOGO = 3;
  size_t logoL = 0;
  for (const std::string &l : logo.lignes) {
    logoL = std::max(logoL, l.size());
  }
  const int logoH = int(logo.lignes.size()) * PIXEL_LOGO;

  int largeurTexte = int(logoL) * PIXEL_LOGO;
  int hauteurTexte = logo.lignes.empty() ? 0 : logoH + PAS_Y / 2;
  for (const auto &t : textes) {
    const int facteur = t.first == titre ? 2 : 1;
    largeurTexte = std::max(largeurTexte, int(t.second.size()) * PAS_X * facteur);
    hauteurTexte += PAS_Y * facteur;
  }
  const int panneauL = largeurTexte + 2 * MARGE;
  const int panneauH = hauteurTexte + 2 * MARGE;

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

  // Cases mises en valeur (le vainqueur), à l'échelle de la fenêtre ; au
  // moins 3 pixels de côté pour rester visibles avec un petit zoom
  const int caseL = std::max(1, fenL / int(largeur));
  const int caseH = std::max(1, fenH / int(hauteur));
  const int trait = std::max(3, std::max(caseL, caseH));
  std::vector<SDL_Rect> cases;
  cases.reserve(enValeur.size());
  for (const SDL_Point &p : enValeur) {
    cases.push_back({p.x * caseL + (caseL - trait) / 2,
                     p.y * caseH + (caseH - trait) / 2, trait, trait});
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

    // Terrain assombri, vainqueur par-dessus, puis panneau presque opaque
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 140);
    SDL_RenderFillRect(renderer, nullptr);
    SDL_SetRenderDrawColor(renderer, VERT.r, VERT.g, VERT.b, 255);
    SDL_RenderFillRects(renderer, cases.data(), int(cases.size()));
    SDL_SetRenderDrawColor(renderer, 15, 20, 25, 235);
    SDL_RenderFillRect(renderer, &panneau);
    SDL_SetRenderDrawColor(renderer, OR.r, OR.g, OR.b, 255);
    SDL_RenderDrawRect(renderer, &panneau);

    const int gaucheX = panneau.x + MARGE * echelle;
    int y = panneau.y + MARGE * echelle;
    if (not logo.lignes.empty()) {
      dessinerMotif(panneau.x + (panneau.w - int(logoL) * PIXEL_LOGO * echelle) / 2,
                    y, logo, PIXEL_LOGO * echelle);
      y += (logoH + PAS_Y / 2) * echelle;
    }
    for (const auto &t : textes) {
      const int ech = (t.first == titre ? 2 : 1) * echelle;
      const int texteL = int(t.second.size()) * PAS_X * ech;
      const int centreX = panneau.x + (panneau.w - texteL) / 2;
      switch (t.first) {
        case titre:dessinerTexte(centreX, y, t.second, ech, OR);
          break;
        case centre:dessinerTexte(centreX, y, t.second, ech, BLANC);
          break;
        case discret:dessinerTexte(centreX, y, t.second, ech, GRIS);
          break;
        case intertitre:dessinerTexte(gaucheX, y, t.second, ech, VERT);
          break;
        case gauche:dessinerTexte(gaucheX, y, t.second, ech, BLANC);
          break;
      }
      y += PAS_Y * ech;
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

  // Le tampon est conservé d'une image à l'autre : seules les parties
  // modifiées sont recopiées dans la texture. Le verrouillage (LockTexture)
  // est la méthode prévue par SDL pour les textures qui changent souvent ;
  // SDL_UpdateTexture est documentée comme lente et destinée aux textures
  // statiques.
  for (size_t b = 0; b < bandes.size(); ++b) {
    Bande &bande = bandes[b];
    if (not bande.modifiee) {
      continue;
    }
    const unsigned y0 = unsigned(b) * HAUTEUR_BANDE;
    const unsigned lignes = std::min(HAUTEUR_BANDE, hauteur - y0);
    const unsigned colonnes = bande.maxX - bande.minX + 1;
    const SDL_Rect zone = {int(bande.minX), int(y0), int(colonnes), int(lignes)};

    void *destination = nullptr;
    int pas = 0;
    if (SDL_LockTexture(texture, &zone, &destination, &pas) == 0) {
      for (unsigned l = 0; l < lignes; ++l) {
        std::memcpy(static_cast<Uint8 *>(destination) + size_t(l) * size_t(pas),
                    &pixels[size_t(y0 + l) * largeur + bande.minX],
                    colonnes * sizeof(Uint32));
      }
      SDL_UnlockTexture(texture);
    }
    bande.modifiee = false;
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

void Affichage2d::dessinerMotif(int x, int y, const MotifPixel &motif,
                                int taillePixel) {

  // Un appel groupé par couleur de la palette
  for (const auto &entree : motif.palette) {
    std::vector<SDL_Rect> rects;
    for (size_t ligne = 0; ligne < motif.lignes.size(); ++ligne) {
      const std::string &l = motif.lignes[ligne];
      for (size_t col = 0; col < l.size(); ++col) {
        if (l[col] == entree.first) {
          rects.push_back({x + int(col) * taillePixel,
                           y + int(ligne) * taillePixel,
                           taillePixel, taillePixel});
        }
      }
    }
    const SDL_Color &c = entree.second;
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
    SDL_RenderFillRects(renderer, rects.data(), int(rects.size()));
  }
}

//--------------------------- gestion couleur -----------------------------
Uint32 Affichage2d::valeurCouleur(Couleur couleur) const {
  return couleurs[couleur];
}
