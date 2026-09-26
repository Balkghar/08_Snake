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
#include <utility>
#include <vector>

enum Couleur { blanc, noir, rouge };

// Image en pixel art : une chaîne par ligne, un caractère par pixel ; les
// caractères absents de la palette sont transparents.
struct MotifPixel {
  std::vector<std::string> lignes;
  std::vector<std::pair<char, SDL_Color>> palette;
};

class Affichage2d {
  //-------------------------- Constructeur --------------------------------
 public:
  Affichage2d(const unsigned larg,
              const unsigned haut,
              const unsigned sdl_del,
              const unsigned nbr_vals
  );

  //------------------------- méthode d'affichage -------------------------
  /**
   * @brief À appeler avant initalisationAffichage : images présentées au
   *        rythme de l'écran (pas de déchirure).
   */
  void activerVsync(bool actif);

  // Fenêtre sans bord qui couvre tout l'écran numéro « ecran » (numérotation
  // SDL ; -1 : fenêtre ordinaire). À appeler avant initalisationAffichage.
  void activerPleinEcran(int ecran);

  /**
   * @brief Le plus grand des écrans branchés (en nombre de pixels) : sa
   *        résolution et son numéro. Renvoie false s'il n'y en a aucun.
   */
  static bool plusGrandEcran(unsigned &largeurEcran, unsigned &hauteurEcran,
                             int &numero);
  bool initalisationAffichage();
  /**
   * @brief Fréquence de rafraîchissement de l'écran de la fenêtre, en Hz
   *        (60 si inconnue).
   */
  unsigned frequenceEcran() const;

  /**
   * @brief Temps passés dans l'envoi à la carte graphique et la
   *        présentation, et méthode d'envoi retenue (option --profil).
   */
  std::string rapportProfil() const;
  /**
   * @brief Colorie une case. Peut être appelée en même temps par plusieurs
   *        threads, à condition que chacun travaille sur ses propres bandes
   *        de HAUTEUR_BANDE lignes.
   */
  bool ajouterElementAffichage(int x, int y, Couleur couleur);
  static constexpr unsigned HAUTEUR_BANDE = 16;
  void prechargerElement(int x, int y) const;  // voir precharger.hpp
  bool nettoyerAffichage(Couleur couleur);
  /**
   * @brief Traite les événements en attente.
   * @param accelerer  +1 par appui sur + (ou flèche haut), -1 par appui sur -
   *                   (ou flèche bas)
   * @return true si l'utilisateur veut quitter (fenêtre fermée, ÉCHAP)
   */
  bool fermetureDemandee(int &accelerer);
  void definirTitre(const std::string &titre);
  bool fermerAffichage();
  bool mettreAjourAffichage();

  /**
   * @brief Assombrit le terrain et affiche un panneau par-dessus (logo puis
   *        texte), jusqu'à ce que l'utilisateur appuie sur ÉCHAP / ENTRÉE /
   *        ESPACE ou ferme la fenêtre. Les cases enValeur (en cases du
   *        terrain) restent en vert vif par-dessus l'assombrissement.
   *
   *        Style d'une ligne selon son préfixe :
   *          "# "  titre, double taille, centré     "^ "  centré
   *          "-- " intertitre                        "~ "  centré, discret
   *          sinon aligné à gauche
   */
  void afficherEcranFin(const MotifPixel &logo,
                        const std::vector<std::string> &lignes,
                        const std::vector<SDL_Point> &enValeur);
 private:
  // Façons d'envoyer les pixels modifiés à la texture. Aucune n'est la plus
  // rapide partout (cela dépend de la carte graphique et du pilote) : la
  // meilleure est mesurée au démarrage (calibrerEnvoi).
  enum class MethodeEnvoi { bandesVerrou, bandesMiseAJour, zoneUnique };
  void calibrerEnvoi();
  void marquerToutModifie();
  void envoyerZoneModifiee();
  void dessinerTexte(int x, int y, const std::string &texte, int echelle,
                     SDL_Color couleur);
  void dessinerMotif(int x, int y, const MotifPixel &motif, int taillePixel);

  Uint32 valeurCouleur(Couleur couleur) const;
  Uint32 couleurs[3] = {0, 0, 0};  // indexé par Couleur, format de la texture

  //------------------------- Données -------------------------------------
  const unsigned largeur;
  const unsigned hauteur;
  const unsigned sdl_delay;
  unsigned nbre_values;  // zoom : taille d'une case en pixels
  bool vsync = false;
  int ecranPleinEcran = -1;  // -1 : fenêtre ordinaire
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
  SDL_Texture *texture = nullptr;
  SDL_PixelFormat *format = nullptr;
  std::vector<Uint32> pixels;
  Uint32 debutImage = 0;

  // Parties modifiées depuis la dernière image, seules envoyées au GPU :
  // l'écran est découpé en bandes horizontales, chacune retenant l'intervalle
  // de colonnes touché. Deux petits serpents aux deux coins ne font plus
  // renvoyer tout l'écran.
  struct Bande {
    bool modifiee = false;
    unsigned minX = 0, maxX = 0;
  };
  std::vector<Bande> bandes;
  MethodeEnvoi methode = MethodeEnvoi::bandesVerrou;
  double calibrage[3] = {0, 0, 0};  // ms par envoi complet, par méthode

  // Profil (--profil)
  double tempsEnvoi = 0;
  double tempsPresentation = 0;
  double tempsPause = 0;
  unsigned long nbImages = 0;
};

#endif