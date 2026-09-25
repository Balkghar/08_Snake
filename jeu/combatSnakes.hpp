/*
---------------------------------------------------------------------------
Fichier     : combatSnakes.hpp
Nom du labo : Labo8 - Snake
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 10.01.2023
But         : Déclaration de la classe combat et des fonctions membres de la classe
              combat.

Remarque(s) : Un tour de jeu est simultané : tous les serpents avancent en
              même temps, puis les conflits sont résolus. Le tour est
              découpé en phases exécutées par plusieurs threads (voir
              jouerTours) ; le résultat est identique quel que soit le
              nombre de threads.

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#ifndef LABO8_SNAKES_JEU_COMBATSNAKES_HPP
#define LABO8_SNAKES_JEU_COMBATSNAKES_HPP

#include <atomic>
#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include "snake.hpp"
#include "pomme.hpp"
#include "../outils/affichage2d.hpp"
#include "../outils/poolThreads.hpp"
#include "../outils/allocateurGrandesPages.hpp"

class Combat {
 public:
  //------------------------- Constructeur --------------------------------
  Combat() : Combat(100, 100, 10) {}
  /**
   * @param nbThreads threads de calcul (0 = un par cœur, moins un laissé
   *                  à l'affichage)
   */
  Combat(unsigned largeur,
         unsigned longueur,
         unsigned nbSerpent,
         unsigned nbThreads = 0
  );

  //------------------------- lancement du combat -------------------------
  static constexpr unsigned DELAI_DEFAUT = 50;  // ms entre deux images
  static constexpr unsigned ZOOM_DEFAUT = 4;    // pixels écran par case
  static constexpr unsigned VITESSE_DEFAUT = 1; // tours de jeu par image
  static constexpr unsigned VITESSE_AUTO = 0;   // autant que le temps permet

  /**
   * @param silencieux  ne pas annoncer chaque mort dans la console
   * @param finAuto     à la fin, statistiques dans la console seulement,
   *                    sans écran de victoire à fermer
   */
  void choisirSorties(bool silencieux, bool finAuto);

  /**
   * @brief Synchronisation verticale : les images sont montrées au rythme
   *        de l'écran, sans déchirure. Ne ralentit pas le calcul, qui tourne
   *        sur d'autres threads.
   */
  void activerVsync(bool actif);

  /**
   * @brief En fin de partie, affiche où le temps a été passé (calcul,
   *        préparation des images, envoi à la carte graphique, attentes).
   */
  void activerProfil(bool actif);

  void commencerCombat(unsigned delai = DELAI_DEFAUT,
                       unsigned zoom = ZOOM_DEFAUT,
                       unsigned vitesse = VITESSE_DEFAUT);

 private:
  static unsigned threadsParDefaut(unsigned demandes);
  static constexpr unsigned MIN = 0;
  static constexpr std::uint32_t AUCUNE_TETE = 0;

  //------------------------- grille du terrain ---------------------------
  // Une case par élément (indexCase). Toutes les informations d'une case
  // sont regroupées dans 16 octets : avec des serpents éparpillés sur tout
  // le terrain, chaque accès est un défaut de cache, autant n'en payer
  // qu'un.
  struct Case {
    // segments de serpents vivants sur la case
    std::int32_t serpents = 0;
    // XOR des numéros des segments présents : avec un seul segment, c'est
    // exactement son propriétaire (détection des morsures)
    std::uint32_t xorSerpents = 0;
    // numéro du serpent dont la tête occupe la case (une seule : quand
    // plusieurs têtes arrivent ensemble, la plus longue gagne), sa taille
    // et la parité du tour où elle est arrivée : une tête du tour précédent
    // est l'ancienne position d'un serpent qui vient de repartir
    std::uint32_t teteId = AUCUNE_TETE;
    std::uint32_t teteTaille : 19;
    std::uint32_t teteParite : 1;
    std::uint32_t pommes : 11;
    std::uint32_t marquee : 1;  // déjà dans la liste des cases à redessiner
    Case() : teteTaille(0), teteParite(0), pommes(0), marquee(0) {}
  };
  static constexpr std::uint32_t TAILLE_MAX_TETE = (1u << 19) - 1;

  // Le terrain est découpé en bandes horizontales, une par thread, alignées
  // sur les bandes d'envoi de l'affichage : chaque bande n'est modifiée que
  // par son thread, sans verrou ni opération atomique.
  struct alignas(TAILLE_LIGNE_CACHE) Region {
    std::vector<std::uint32_t> casesModifiees;  // à redessiner
    std::int64_t nbOccupees = 0;                 // cases non vides
  };

  //------------------------- messages entre phases -----------------------
  // Changement d'occupation d'une case (bit de poids fort : retrait)
  struct Mouvement {
    std::uint32_t cellule;
    std::uint32_t idEtSens;
  };
  static constexpr std::uint32_t RETRAIT = 1u << 31;
  // Tête arrivée sur une case
  struct ArriveeTete {
    std::uint32_t cellule;
    std::uint32_t id;
    std::uint32_t taille;
  };
  // Tête effacée d'une case (serpent mort), si elle y est encore
  struct Effacement {
    std::uint32_t cellule;
    std::uint32_t id;
  };
  // Récompenses à distribuer ensuite (indices dans serpents)
  struct Mort {
    std::uint32_t victime;
    std::uint32_t tueur;
    std::uint32_t longueur;
  };
  struct Morsure {
    std::uint32_t attaquant;
    std::uint32_t longueurCoupee;
  };
  // Tout ce qu'un thread produit pendant un tour, rangé par région
  // destinataire ; chaque thread écrit uniquement dans sa propre boîte
  struct alignas(TAILLE_LIGNE_CACHE) Boite {
    std::vector<std::vector<Mouvement>> mouvements;      // [région]
    std::vector<std::vector<ArriveeTete>> tetes;         // [région]
    std::vector<std::vector<Effacement>> effacements;    // [région]
    std::vector<Mort> morts;
    std::vector<Morsure> morsures;
    std::vector<std::uint32_t> repas;
  };

  //------------------------- méthodes d'initialisation -------------------
  void initialiserPomme();
  void initialiserSerpent();
  CoordonneesXY generateurDeCoord();

  //------------------------- grille ---------------------------------------
  size_t indexCase(int x, int y) const;
  unsigned regionDe(int y) const;
  static bool estOccupee(const Case &c);
  void modifierCase(size_t i, Region &region, int deltaSerpent, int deltaPomme,
                    std::uint32_t serpent);
  void modifierCase(int x, int y, int deltaSerpent, int deltaPomme,
                    std::uint32_t serpent = AUCUNE_TETE);
  void appliquerModifications(Snake &serpent);
  std::int64_t nbCasesOccupees() const;

  //------------------------- tour de jeu ---------------------------------
  unsigned long jouerTours(unsigned long nbMax, Uint32 finAuPlusTard);
  void jouerTourParallele(unsigned thread);
  void jouerTourSerie();
  void phaseDeplacement(unsigned thread, unsigned nbThreadsActifs);
  void phaseGrille(unsigned region);
  void phaseCombats(unsigned thread, unsigned nbThreadsActifs);
  void phaseConsequences(unsigned thread, unsigned nbThreadsActifs);
  void phaseRetraits(unsigned region);
  void phaseResolution();
  void emettreRetraits(Snake &serpent, std::uint32_t id, Boite &boite);
  std::uint64_t hasard(std::uint32_t serpent) const;
  std::uint32_t parite() const;
  void retirerMorts();

  //------------------------- méthodes d'affichage ------------------------
  // Ce que le moteur dépose pour l'affichage à la fin d'un lot de tours :
  // les cases à recolorier (numéro de case, couleur dans les 2 bits de
  // poids fort), par région, et de quoi écrire le titre
  struct Image {
    std::vector<std::vector<std::uint32_t>> parRegion;
    unsigned long tours = 0;
    unsigned serpents = 0;
    unsigned long toursLot = 0;
  };
  static constexpr std::uint32_t MASQUE_CASE = (1u << 30) - 1;

  void preparerImage(Image &image);
  void preparerRegion(Region &region, std::vector<std::uint32_t> &sortie);
  void appliquerImage(Affichage2d &affichage, const Image &image) const;
  bool presenter(Affichage2d &affichage, int &accelerer);
  bool faireCombattreSerpents(Affichage2d &affichage);
  Uint32 dureeLotAuto() const;
  void changerVitesse(int pas);
  void mettreAJourTitre(Affichage2d &affichage, const Image &image) const;

  //------------------------- fin de partie -------------------------------
  void afficherVictoire(Affichage2d &affichage);
  std::vector<std::string> statistiques() const;
  static MotifPixel logoGreenKatze();

  //------------------------- Données -------------------------------------
  const unsigned largeur;
  const unsigned longueur;
  unsigned largeurAffichage;
  unsigned longueurAffichage;
  unsigned nbSerpent;
  const unsigned nbSerpentsDepart;
  unsigned long nbTours = 0;
  unsigned dureeMs = 0;
  std::atomic<unsigned> vitesse{VITESSE_DEFAUT};  // changée au clavier
  unsigned delai = DELAI_DEFAUT;
  unsigned frequenceEcran = 60;  // images par seconde de l'écran
  bool vsync = false;
  bool profil = false;

  std::vector<Snake> serpents;
  std::vector<Pomme> pommes;
  std::vector<std::uint32_t> vivants;  // indices des serpents en vie
  std::vector<std::uint32_t> ancienneTete;  // case de tête au tour précédent
  size_t mortsDansVivants = 0;  // retirés de vivants de temps en temps

  // Par serpent, écrits pendant un tour par un seul thread chacun :
  //  - rencontre : numéro de l'ancienne tête trouvée à l'arrivée (0 sinon)
  //  - mortPar : numéro du vainqueur si le serpent meurt ce tour (0 sinon)
  std::vector<std::uint32_t> rencontre;
  std::vector<std::uint32_t> mortPar;

  // Grand tableau lu à des endroits aléatoires : pages de 2 Mo
  std::vector<Case, AllocateurGrandesPages<Case>> cases;
  std::vector<Region> regions;
  std::vector<std::uint16_t> regionParLigne;

  // Calcul parallèle
  PoolThreads pool;
  std::vector<Boite> boites;  // une par thread
  unsigned boitesActives = 1; // boîtes remplies pendant ce tour
  std::uint64_t graine;       // suites aléatoires des serpents
  bool continuer = false;     // décidé par le thread 0 à chaque tour
  bool silencieux = false;
  bool finAuto = false;

  // Détection des morsures : les segments d'un corps ne bougent jamais,
  // une tête ne se retrouve donc sur un corps qu'en y arrivant. Quand une
  // tête arrive sur une case avec un seul autre segment, son propriétaire
  // (donné par le XOR) est prévenu ; avec plusieurs, tout le monde vérifie
  // ce tour-ci. Seuls les serpents prévenus parcourent leur corps.
  std::unique_ptr<std::atomic<std::uint8_t>[]> menace;  // par serpent
  std::atomic<bool> victimeInconnue{false};
};

#endif
