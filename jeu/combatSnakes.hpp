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
  // Les numéros de serpent sont rangés sur 20 bits dans chaque case :
  // jusqu'à 1 048 575 serpents (un écran 4K en permet 2 millions, à raison
  // d'un serpent pour 4 cases)
  static constexpr unsigned BITS_NUMERO = 20;
  static constexpr std::uint32_t MAX_SERPENTS = (1u << BITS_NUMERO) - 1;

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
  void activerPleinEcran(bool actif);
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
  // tiennent dans 8 octets : avec des serpents éparpillés sur tout le
  // terrain, chaque accès est un défaut de cache, autant n'en payer qu'un ;
  // et 1200 x 800 cases font 7,7 Mo, qui tiennent dans le cache L3 d'un
  // processeur de portable (12 Mo), au lieu de 15 Mo qui n'y tenaient pas.
  // Bits : 11 + 20 + 20 + 1 + 11 + 1 = 64.
  struct Case {
    // segments de serpents vivants sur la case (au plus 2047 : une tête
    // qui arrive sur un corps le coupe ou meurt, les segments ne
    // s'empilent jamais au-delà de quelques-uns)
    std::uint64_t serpents : 11;
    // XOR des numéros des segments présents : avec un seul segment, c'est
    // exactement son propriétaire (détection des morsures)
    std::uint64_t xorSerpents : BITS_NUMERO;
    // numéro du serpent dont la tête occupe la case (une seule : quand
    // plusieurs têtes arrivent ensemble, la plus longue gagne) et parité du
    // tour où elle est arrivée : une tête du tour précédent est l'ancienne
    // position d'un serpent qui vient de repartir. (Sa taille n'est pas
    // gardée : lue dans le serpent lors d'un face-à-face, rare.)
    std::uint64_t teteId : BITS_NUMERO;
    std::uint64_t teteParite : 1;
    std::uint64_t pommes : 11;
    std::uint64_t marquee : 1;  // déjà dans la liste des cases à redessiner
    Case() : serpents(0), xorSerpents(0), teteId(AUCUNE_TETE), teteParite(0),
             pommes(0), marquee(0) {}
  };
  static_assert(sizeof(Case) == 8, "une case doit tenir dans 8 octets");
  static constexpr std::uint32_t TAILLE_MAX_TETE = (1u << 19) - 1;

  // Le terrain est découpé en bandes horizontales, une par thread, alignées
  // sur les bandes d'envoi de l'affichage : chaque bande n'est modifiée que
  // par son thread, sans verrou ni opération atomique.
  struct alignas(TAILLE_LIGNE_CACHE) Region {
    std::vector<std::uint32_t> casesModifiees;  // à redessiner
    std::int64_t nbOccupees = 0;                 // cases non vides
    std::vector<std::uint32_t> tuilesMarquees;  // à effacer en fin de tour
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
  void jouerTourSerieMesure();
  std::string detailSerie() const;  // profil des tours à un thread
  void phaseDeplacement(unsigned thread, size_t debut, size_t fin);
  void phaseGrille(unsigned region);
  void phaseCombats(size_t debut, size_t fin);
  void phaseConsequences(unsigned thread, size_t debut, size_t fin);
  void phaseRetraits(unsigned region);
  void phaseResolution();
  void emettreRetraits(Snake &serpent, std::uint32_t id, Boite &boite);
  std::uint64_t hasard(std::uint32_t serpent) const;
  std::uint32_t parite() const;
  // Taille d'un serpent telle que rangée dans les arrivées de têtes
  std::uint32_t tailleTete(std::uint32_t id) const;
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
  bool pleinEcran = false;

  std::vector<Snake> serpents;
  std::vector<Pomme> pommes;
  std::vector<std::uint32_t> vivants;  // indices des serpents en vie
  std::vector<std::uint32_t> ancienneTete;  // case de tête au tour précédent
  // Résumé de chaque serpent, tenu à jour à côté de l'objet Snake (près de
  // 300 octets : 100 000 serpents font 28 Mo, bien plus que le cache) :
  // 8 octets par serpent suffisent aux combats et aux conséquences, qui ne
  // touchent plus l'objet complet que pour les morts et les mordus.
  struct Resume {
    std::uint32_t teteCase;  // case de la tête
    std::uint32_t taille;    // 0 : mort
  };
  std::vector<Resume> resumes;
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

  // Répartition dynamique avec vol de travail : chaque thread commence par
  // sa propre part (les mêmes serpents et régions à chaque phase et à chaque
  // tour, donc déjà dans son cache), prise par tranches dans son propre
  // compteur ; une fois sa part finie, il vole des tranches dans celles des
  // autres. Sur un processeur hybride (cœurs rapides et lents), les cœurs
  // rapides finissent la part des lents au lieu de les attendre à chaque
  // barrière ; sur un processeur uniforme, presque personne ne vole et rien
  // ne change. Un compteur par phase et par thread, chacun sur sa ligne de
  // cache, remis à zéro par le thread 0 pendant la résolution.
  struct alignas(TAILLE_LIGNE_CACHE) Compteur {
    std::atomic<size_t> valeur{0};
  };
  enum { PHASE_DEPLACEMENT, PHASE_GRILLE, PHASE_COMBATS, PHASE_CONSEQUENCES,
         PHASE_RETRAITS, PHASE_IMAGE, NB_COMPTEURS };
  std::unique_ptr<Compteur[]> compteurs;  // [phase * nbThreads + thread]
  void remettreCompteurs(unsigned phase);
  template<typename Travail>
  void repartir(unsigned phase, unsigned thread, size_t total, size_t tranche,
                Travail travail);
  size_t trancheSerpents() const;
  unsigned boitesActives = 1; // boîtes remplies pendant ce tour
  std::uint64_t graine;       // suites aléatoires des serpents
  std::uint64_t nbMouvements = 0; // déplacements joués (pour le profil)
  // [0] tours joués à un thread, [1] à plusieurs (profil)
  std::uint64_t dureeMode[2] = {0, 0};
  std::uint64_t toursMode[2] = {0, 0};
  std::uint64_t cyclesSerie[6] = {};  // par phase, tours à un thread
  bool continuer = false;     // décidé par le thread 0 à chaque tour
  bool silencieux = false;
  bool finAuto = false;

  // Détection des morsures : les segments d'un corps ne bougent jamais,
  // une tête ne se retrouve donc sur un corps qu'en y arrivant. Quand une
  // tête arrive sur une case avec un seul autre segment, son propriétaire
  // (donné par le XOR) est prévenu ; avec plusieurs, tout le monde vérifie
  // ce tour-ci. Seuls les serpents prévenus parcourent leur corps.
  std::unique_ptr<std::atomic<std::uint8_t>[]> menace;  // par serpent
  // Tuiles de 8 x 8 cases où une tête est arrivée sur plusieurs corps
  // (propriétaires inconnus). Seuls les serpents dont le corps peut
  // atteindre une tuile marquée vérifient tout leur corps, au lieu de tous
  // les serpents : chacun de ces parcours lit la grille sous chaque
  // segment, un défaut de cache par segment.
  static constexpr unsigned BITS_TUILE = 3;
  std::vector<std::uint8_t> tuilesAmbigues;
  std::uint32_t tuilesParLigne = 0;
  std::uint32_t tuileDe(std::uint32_t cellule) const;
  bool peutAtteindreAmbigue(Resume resume) const;
  std::atomic<bool> victimeInconnue{false};
};

#endif
