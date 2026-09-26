/*
---------------------------------------------------------------------------
Fichier     : combatSnakes.cpp
Nom du labo : Labo8 - Snake
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 10.01.2023
But         : Définition des fonctions membres de la classe
              combat.

Remarque(s) : Déroulement d'un tour (voir jouerTours) :
                1. déplacement : chaque serpent avance ; les changements de
                   cases sont rangés par région de destination
                2. grille : chaque région applique les changements qui la
                   concernent et départage les têtes arrivées ensemble
                3. décisions : chaque serpent regarde s'il a perdu un
                   tête contre tête, s'il est mordu, s'il a mangé
                4. résolution (un seul thread, dans l'ordre des numéros) :
                   morts, coupures, pommes
              Les phases 1 à 3 tournent sur tous les threads ; aucune case
              n'est écrite par deux threads à la fois, sans verrou.

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#include "combatSnakes.hpp"
#include "../outils/aleatoire.hpp"
#include "../outils/cycles.hpp"
#include "../outils/precharger.hpp"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <climits>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <iostream>

using namespace std;

namespace {

// En dessous, un seul thread : se synchroniser coûterait plus que le calcul
const size_t SEUIL_PARALLELE = 512;
const unsigned TOURS_ENTRE_HORLOGES = 64;
// Redessin parallèle au-delà de ce nombre de cases modifiées
const size_t SEUIL_DESSIN_PARALLELE = 16384;
// Préchargement : nombre d'éléments d'avance
const size_t AVANCE = 4;
const size_t AVANCE_MESSAGES = 8;

// Mélangeur de splitmix64 : transforme un compteur en nombre aléatoire de
// bonne qualité, sans état partagé entre threads
inline uint64_t melanger(uint64_t z) {
  z += 0x9E3779B97F4A7C15ull;
  z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
  z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
  return z ^ (z >> 31);
}

// Tête contre tête : la plus longue gagne. À égalité, un tirage fixé par
// les numéros (et non le plus petit numéro : les numéros suivent la position
// de départ, cela avantagerait le haut du terrain).
inline bool gagneContre(uint32_t taille, uint32_t id,
                        uint32_t tailleAutre, uint32_t idAutre) {
  if (taille != tailleAutre) {
    return taille > tailleAutre;
  }
  const uint64_t a = melanger(id), b = melanger(idAutre);
  return a != b ? a < b : id < idAutre;
}

}  // namespace

// Par défaut, un thread par cœur physique, moins un cœur laissé à
// l'affichage, qui travaille en même temps que le calcul (voir
// faireCombattreSerpents). Les deux threads matériels d'un même cœur (SMT)
// se partagent ses unités de calcul et ses caches, et chaque barrière attend
// le plus lent : deux threads de calcul sur un même cœur ralentissent plus
// qu'ils n'aident. -j permet toujours d'essayer autre chose.
unsigned Combat::threadsParDefaut(unsigned demandes) {
  if (demandes != 0) {
    return demandes;
  }
  const unsigned coeurs = PoolThreads::coeursPhysiques();
  return coeurs > 1 ? coeurs - 1 : 1;
}

//=========================== Partie public ===============================

//------------------------- Constructeur --------------------------------
Combat::Combat(unsigned int largeur,
               unsigned int longueur,
               unsigned int nbSerpent,
               unsigned int nbThreads
) : largeur(largeur), longueur(longueur), nbSerpent(nbSerpent),
    nbSerpentsDepart(nbSerpent), pool(threadsParDefaut(nbThreads)) {

  // L'affichage SDL partant de 0, pour que l'affichage soit correct à l'écran,
  // il faut faire moins 1 à la valeur entrée par l'utilisateur.
  longueurAffichage = longueur - 1;
  largeurAffichage = largeur - 1;

  cases.assign(size_t(largeur) * longueur, Case());

  // Une région (bande de lignes) par thread, de hauteur multiple de celle
  // des bandes de l'affichage pour que le redessin parallèle ne se croise
  // jamais. (Plus de régions équilibrerait mieux un processeur hybride, mais
  // chaque région de plus coûte une boîte par thread à parcourir : mesuré
  // plus lent. Un thread rapide peut de toute façon voler une région
  // entière.)
  const unsigned bande = Affichage2d::HAUTEUR_BANDE;
  const unsigned nbBandes = (longueur + bande - 1) / bande;
  const unsigned voulues = max(1u, min(nbBandes, pool.taille()));
  unsigned hauteurRegion = (longueur + voulues - 1) / voulues;
  hauteurRegion = (hauteurRegion + bande - 1) / bande * bande;
  const unsigned nbRegions = (longueur + hauteurRegion - 1) / hauteurRegion;
  regions.resize(nbRegions);
  regionParLigne.resize(longueur);
  for (unsigned y = 0; y < longueur; ++y) {
    regionParLigne[y] = uint16_t(min(y / hauteurRegion, nbRegions - 1));
  }

  // (les régions ont une hauteur multiple de 16 : chaque ligne de tuiles
  // appartient à une seule région)
  static_assert((Affichage2d::HAUTEUR_BANDE >> BITS_TUILE) << BITS_TUILE
                    == Affichage2d::HAUTEUR_BANDE, "tuiles et bandes");
  tuilesParLigne = ((largeur - 1) >> BITS_TUILE) + 1;
  tuilesAmbigues.assign(size_t(tuilesParLigne) * (((longueur - 1) >> BITS_TUILE) + 1), 0);

  boites.resize(pool.taille());
  compteurs.reset(new Compteur[size_t(NB_COMPTEURS) * pool.taille()]);
  for (Boite &boite : boites) {
    boite.mouvements.resize(nbRegions);
    boite.tetes.resize(nbRegions);
    boite.effacements.resize(nbRegions);
  }
  menace = make_unique<atomic<uint8_t>[]>(nbSerpent);
  rencontre.assign(nbSerpent, AUCUNE_TETE);
  mortPar.assign(nbSerpent, AUCUNE_TETE);
  graine = graineAleatoire();

  initialiserSerpent();
  initialiserPomme();
}

//------------------------- lancement du combat -------------------------
void Combat::choisirSorties(bool silencieux, bool finAuto) {
  this->silencieux = silencieux;
  this->finAuto = finAuto;
}

void Combat::activerVsync(bool actif) {
  vsync = actif;
}

void Combat::activerPleinEcran(bool actif) {
  pleinEcran = actif;
}

void Combat::activerProfil(bool actif) {
  profil = actif;
}

void Combat::commencerCombat(unsigned delai, unsigned zoom, unsigned vitesse) {

  this->delai = delai;
  this->vitesse = vitesse;
  Affichage2d affichage(largeur, longueur, delai, zoom);
  affichage.activerVsync(vsync);
  affichage.activerPleinEcran(pleinEcran);

  if (not affichage.initalisationAffichage()) {
    frequenceEcran = affichage.frequenceEcran();
    affichage.nettoyerAffichage(Couleur::blanc);

    const Uint32 debut = SDL_GetTicks();
    const bool termine = faireCombattreSerpents(affichage);
    dureeMs = SDL_GetTicks() - debut;

    if (termine) {
      afficherVictoire(affichage);
    } else {
      cout << "Partie interrompue apres " << nbTours << " tours." << endl;
    }
  }

  affichage.fermerAffichage();

}

//=========================== Partie privée ===============================

//------------------------- méthodes d'initialisation -------------------
void Combat::initialiserPomme() {

  pommes.reserve(nbSerpent);

  for (unsigned i = 1; i <= Combat::nbSerpent; ++i) {
    CoordonneesXY nouvelleCoord = generateurDeCoord();

    pommes.emplace_back(nouvelleCoord.x, nouvelleCoord.y, i, true);
    modifierCase(nouvelleCoord.x, nouvelleCoord.y, 0, +1);

  }
}

void Combat::initialiserSerpent() {

  serpents.reserve(nbSerpent);
  vivants.reserve(nbSerpent);
  ancienneTete.reserve(nbSerpent);
  resumes.reserve(nbSerpent);

  // Places tirées au hasard, puis numérotées dans l'ordre du terrain : les
  // serpents voisins ont des numéros proches, donc leurs données sont
  // voisines en mémoire, et chaque thread (qui traite une tranche de
  // numéros) travaille surtout dans sa propre région. Les places sont
  // réservées pendant le tirage pour ne pas tomber deux fois au même endroit.
  vector<CoordonneesXY> places;
  places.reserve(nbSerpent);
  for (unsigned i = 1; i <= Combat::nbSerpent; ++i) {
    places.push_back(generateurDeCoord());
    modifierCase(places.back().x, places.back().y, +1, 0);
  }
  for (const CoordonneesXY &place : places) {
    modifierCase(place.x, place.y, -1, 0);
  }
  sort(places.begin(), places.end(),
       [](const CoordonneesXY &a, const CoordonneesXY &b) {
         return a.y != b.y ? a.y < b.y : a.x < b.x;
       });

  for (unsigned i = 1; i <= Combat::nbSerpent; ++i) {
    const CoordonneesXY nouvelleCoord = places[i - 1];

    serpents.emplace_back(nouvelleCoord.x, nouvelleCoord.y, i, true, 10);
    appliquerModifications(serpents.back());
    const size_t cellule = indexCase(nouvelleCoord.x, nouvelleCoord.y);
    Case &c = cases[cellule];
    c.teteId = i;
    c.teteParite = 1;  // comme arrivée au tour « -1 »
    vivants.push_back(i - 1);
    ancienneTete.push_back(uint32_t(cellule));
    resumes.push_back({uint32_t(cellule), uint32_t(serpents.back().getTaille())});
  }

}

CoordonneesXY Combat::generateurDeCoord() {
  const int64_t nbCases = int64_t(cases.size());
  const bool plein = nbCasesOccupees() >= nbCases;
  CoordonneesXY nouvelleCoord = {0, 0};

  // Tirages aléatoires tant que le terrain est peu rempli...
  for (unsigned essai = 0; essai < 64 or plein; ++essai) {
    nouvelleCoord.x = aleatoireEntreDeuxValeurs(MIN, (int) largeurAffichage);
    nouvelleCoord.y = aleatoireEntreDeuxValeurs(MIN, (int) longueurAffichage);
    if (plein or not estOccupee(cases[indexCase(nouvelleCoord.x, nouvelleCoord.y)])) {
      // (terrain plein : on accepte une case occupée plutôt que de boucler)
      return nouvelleCoord;
    }
  }

  // ... sinon on parcourt le terrain depuis une case aléatoire
  const size_t depart = size_t(aleatoireEntreDeuxValeurs(0, int(nbCases - 1)));
  for (size_t k = 0; k < size_t(nbCases); ++k) {
    const size_t i = (depart + k) % size_t(nbCases);
    if (not estOccupee(cases[i])) {
      nouvelleCoord.x = int(i % largeur);
      nouvelleCoord.y = int(i / largeur);
      break;
    }
  }
  return nouvelleCoord;
}

//------------------------- grille ---------------------------------------
size_t Combat::indexCase(int x, int y) const {
  return size_t(y) * largeur + size_t(x);
}

unsigned Combat::regionDe(int y) const {
  return regionParLigne[size_t(y)];
}

bool Combat::estOccupee(const Case &c) {
  return c.serpents > 0 or c.pommes > 0;
}

void Combat::modifierCase(size_t i, Region &region, int deltaSerpent,
                          int deltaPomme, uint32_t serpent) {
  Case &c = cases[i];
  const bool etaitOccupee = estOccupee(c);
  c.serpents = uint32_t(int32_t(c.serpents) + deltaSerpent);
  c.pommes = uint32_t(int(c.pommes) + deltaPomme);
  c.xorSerpents ^= serpent;
  const bool estOccupeeMaintenant = estOccupee(c);

  if (estOccupeeMaintenant != etaitOccupee) {
    region.nbOccupees += estOccupeeMaintenant ? 1 : -1;
  }

  // Une case modifiée plusieurs fois entre deux images n'est redessinée
  // qu'une fois
  if (not c.marquee) {
    c.marquee = 1;
    region.casesModifiees.push_back(uint32_t(i));
  }
}

void Combat::modifierCase(int x, int y, int deltaSerpent, int deltaPomme,
                          uint32_t serpent) {
  if (x < 0 or y < 0 or unsigned(x) >= largeur or unsigned(y) >= longueur) {
    return;
  }
  modifierCase(indexCase(x, y), regions[regionDe(y)], deltaSerpent, deltaPomme,
               serpent);
}

void Combat::appliquerModifications(Snake &serpent) {
  const uint32_t id = uint32_t(&serpent - serpents.data()) + 1;
  for (const CoordonneesXY &coord : serpent.getCasesAjoutees()) {
    modifierCase(coord.x, coord.y, +1, 0, id);
  }
  for (const CoordonneesXY &coord : serpent.getCasesRetirees()) {
    modifierCase(coord.x, coord.y, -1, 0, id);
  }
  serpent.oublierModifications();
}

int64_t Combat::nbCasesOccupees() const {
  int64_t total = 0;
  for (const Region &region : regions) {
    total += region.nbOccupees;
  }
  return total;
}

//------------------------- tour de jeu ---------------------------------

unsigned long Combat::jouerTours(unsigned long nbMax, Uint32 finAuPlusTard) {

  // finAuPlusTard : instant (SDL_GetTicks) après lequel on s'arrête, 0 si
  // seul le nombre de tours compte. Au moins un tour est toujours joué.
  const unsigned long depart = nbTours;
  auto encore = [&]() {
    return nbSerpent > 1 and nbTours - depart < nbMax
        and (finAuPlusTard == 0 or SDL_GetTicks() < finAuPlusTard);
  };

  do {
    const Uint64 debutTours = SDL_GetPerformanceCounter();
    const unsigned long toursAvant = nbTours;
    const bool parallele = pool.taille() > 1
        and vivants.size() - mortsDansVivants >= SEUIL_PARALLELE;
    if (parallele) {
      boitesActives = pool.taille();
      // Tous les threads enchaînent les tours sans rendre la main, séparés
      // par des barrières ; le thread 0 décide de continuer ou non
      pool.executer([&](unsigned thread) {
        do {
          jouerTourParallele(thread);
          if (thread == 0) {
            continuer = encore()
                and vivants.size() - mortsDansVivants >= SEUIL_PARALLELE;
          }
          pool.barriere();
        } while (continuer);
      });
    } else {
      // (seule la boîte du thread 0 sert : inutile de parcourir les autres)
      boitesActives = 1;
      // Un tour à quelques serpents ne dure qu'une microseconde : l'horloge
      // n'est lue que tous les TOURS_ENTRE_HORLOGES tours. (Les serpents ne
      // font que mourir : inutile de revérifier le seuil du parallèle.)
      unsigned sansHorloge = 0;
      do {
        jouerTourSerie();
      } while (nbSerpent > 1 and nbTours - depart < nbMax
               and ++sansHorloge < TOURS_ENTRE_HORLOGES);
    }
    const int mode = parallele ? 1 : 0;
    dureeMode[mode] += SDL_GetPerformanceCounter() - debutTours;
    toursMode[mode] += nbTours - toursAvant;
  } while (encore());

  return nbTours - depart;
}

void Combat::remettreCompteurs(unsigned phase) {
  const unsigned n = pool.taille();
  for (unsigned t = 0; t < n; ++t) {
    compteurs[phase * n + t].valeur.store(0, memory_order_relaxed);
  }
}

template<typename Travail>
void Combat::repartir(unsigned phase, unsigned thread, size_t total,
                      size_t tranche, Travail travail) {
  const unsigned n = pool.taille();
  // Sa propre part d'abord, puis celles des suivants (vol)
  for (unsigned k = 0; k < n; ++k) {
    const unsigned proprietaire = (thread + k) % n;
    const size_t debutPart = total * proprietaire / n;
    const size_t taillePart = total * (proprietaire + 1) / n - debutPart;
    Compteur &compteur = compteurs[phase * n + proprietaire];
    // (lecture simple avant de tenter un vol : une part finie ne coûte
    // qu'une lecture partagée, pas une écriture sur la ligne de cache)
    if (k > 0 and compteur.valeur.load(memory_order_relaxed) >= taillePart) {
      continue;
    }
    for (;;) {
      const size_t debut = compteur.valeur.fetch_add(tranche, memory_order_relaxed);
      if (debut >= taillePart) {
        break;
      }
      travail(debutPart + debut, debutPart + min(debut + tranche, taillePart));
    }
  }
}

size_t Combat::trancheSerpents() const {
  // Assez de tranches pour équilibrer (~16 par thread), assez grosses pour
  // que le compteur commun ne coûte rien
  return max<size_t>(64, vivants.size() / (size_t(pool.taille()) * 16));
}

void Combat::jouerTourParallele(unsigned thread) {
  const size_t nbVivants = vivants.size();
  const size_t tranche = trancheSerpents();
  auto parRegion = [&](size_t debut, size_t fin, void (Combat::*phase)(unsigned)) {
    for (size_t r = debut; r < fin; ++r) {
      (this->*phase)(unsigned(r));
    }
  };

  repartir(PHASE_DEPLACEMENT, thread, nbVivants, tranche,
           [&](size_t debut, size_t fin) { phaseDeplacement(thread, debut, fin); });
  pool.barriere();
  repartir(PHASE_GRILLE, thread, regions.size(), 1,
           [&](size_t debut, size_t fin) { parRegion(debut, fin, &Combat::phaseGrille); });
  pool.barriere();
  repartir(PHASE_COMBATS, thread, nbVivants, tranche,
           [&](size_t debut, size_t fin) { phaseCombats(debut, fin); });
  pool.barriere();
  repartir(PHASE_CONSEQUENCES, thread, nbVivants, tranche,
           [&](size_t debut, size_t fin) { phaseConsequences(thread, debut, fin); });
  pool.barriere();
  repartir(PHASE_RETRAITS, thread, regions.size(), 1,
           [&](size_t debut, size_t fin) { parRegion(debut, fin, &Combat::phaseRetraits); });
  pool.barriere();
  if (thread == 0) {
    phaseResolution();
  }
}

void Combat::jouerTourSerie() {
  // Peu de serpents : mêmes phases, sur un seul thread (même résultat)
  if (profil) {
    jouerTourSerieMesure();
    return;
  }
  phaseDeplacement(0, 0, vivants.size());
  for (unsigned r = 0; r < regions.size(); ++r) {
    phaseGrille(r);
  }
  phaseCombats(0, vivants.size());
  phaseConsequences(0, 0, vivants.size());
  for (unsigned r = 0; r < regions.size(); ++r) {
    phaseRetraits(r);
  }
  phaseResolution();
}

uint32_t Combat::tuileDe(uint32_t cellule) const {
  const uint32_t x = cellule % largeur, y = cellule / largeur;
  return (y >> BITS_TUILE) * tuilesParLigne + (x >> BITS_TUILE);
}

bool Combat::peutAtteindreAmbigue(Resume resume) const {
  // Le corps est un chemin de cases voisines partant de la tête : tous ses
  // segments sont à moins de « taille » pas de la tête. Seules les tuiles
  // de ce carré peuvent contenir une case du corps.
  const int portee = int(resume.taille) - 1;
  const int x = int(resume.teteCase % largeur), y = int(resume.teteCase / largeur);
  const int tx0 = max(0, x - portee) >> BITS_TUILE;
  const int tx1 = min(int(largeur) - 1, x + portee) >> BITS_TUILE;
  const int ty0 = max(0, y - portee) >> BITS_TUILE;
  const int ty1 = min(int(longueur) - 1, y + portee) >> BITS_TUILE;
  if ((tx1 - tx0 + 1) * (ty1 - ty0 + 1) > portee) {
    return true;  // long serpent : parcourir son corps coûte moins cher
  }
  for (int ty = ty0; ty <= ty1; ++ty) {
    for (int tx = tx0; tx <= tx1; ++tx) {
      if (tuilesAmbigues[size_t(ty) * tuilesParLigne + size_t(tx)]) {
        return true;
      }
    }
  }
  return false;
}

uint32_t Combat::tailleTete(uint32_t id) const {
  return min(resumes[id - 1].taille, TAILLE_MAX_TETE);
}

string Combat::detailSerie() const {
  if (toursMode[0] == 0) {
    return "";
  }
  uint64_t total = 0;
  for (uint64_t c : cyclesSerie) {
    total += c;
  }
  const double nsParTour = double(dureeMode[0]) * 1e9
      / double(SDL_GetPerformanceFrequency()) / double(toursMode[0]);
  const char *noms[] = {"deplacement", "grille", "combats", "consequences",
                        "retraits", "resolution"};
  ostringstream texte;
  texte << fixed << setprecision(0) << "  un tour a un thread : " << nsParTour
        << " ns (";
  for (unsigned p = 0; p < 6; ++p) {
    texte << (p ? ", " : "") << noms[p] << " "
          << (total ? nsParTour * double(cyclesSerie[p]) / double(total) : 0.0);
  }
  texte << ")\n";

  // Coût d'une lecture d'horloge (certains portables utilisent une source
  // d'horloge lente, un appel système à chaque lecture)
  const unsigned N = 20000;
  const uint64_t debut = SDL_GetPerformanceCounter();
  Uint64 somme = 0;
  for (unsigned i = 0; i < N; ++i) {
    somme += SDL_GetTicks();
  }
  const uint64_t fin = SDL_GetPerformanceCounter();
  texte << setprecision(1) << "  lecture d'horloge : "
        << double(fin - debut) * 1e9 / double(SDL_GetPerformanceFrequency()) / N
        << " ns" << (somme == 0 ? " " : "");
#if defined(__linux__)
  ifstream source("/sys/devices/system/clocksource/clocksource0/current_clocksource");
  string nom;
  if (getline(source, nom) and not nom.empty()) {
    texte << " (source " << nom << ")";
  }
#endif
  texte << "\n";
  return texte.str();
}

void Combat::jouerTourSerieMesure() {
  // Même tour, chaque phase chronométrée (pour --profil)
  uint64_t t = compteurCycles();
  auto top = [&](unsigned phase) {
    const uint64_t maintenant = compteurCycles();
    cyclesSerie[phase] += maintenant - t;
    t = maintenant;
  };
  phaseDeplacement(0, 0, vivants.size());
  top(0);
  for (unsigned r = 0; r < regions.size(); ++r) {
    phaseGrille(r);
  }
  top(1);
  phaseCombats(0, vivants.size());
  top(2);
  phaseConsequences(0, 0, vivants.size());
  top(3);
  for (unsigned r = 0; r < regions.size(); ++r) {
    phaseRetraits(r);
  }
  top(4);
  phaseResolution();
  top(5);
}

uint32_t Combat::parite() const {
  return uint32_t(nbTours & 1);
}

uint64_t Combat::hasard(uint32_t serpent) const {
  // Chaque serpent a sa propre suite (numéro, tour) : le résultat ne dépend
  // ni de l'ordre de traitement ni du nombre de threads
  return melanger(graine + (uint64_t(serpent) << 32) + (nbTours & 0xFFFFFFFFu));
}

//--- 1. Déplacement (par serpent) ---------------------------------------
void Combat::phaseDeplacement(unsigned thread, size_t debut, size_t fin) {

  Boite &boite = boites[thread];

  // (les premiers aussi : avec peu de serpents, ce sont presque tous)
  for (size_t v = debut; v < min(debut + AVANCE, fin); ++v) {
    precharger(&cases[ancienneTete[vivants[v]]]);
  }
  for (size_t v = debut; v < fin; ++v) {
    // Préchargement de la case (ancienne tête) qu'un serpent à venir va
    // modifier
    if (v + AVANCE < fin) {
      precharger(&cases[ancienneTete[vivants[v + AVANCE]]]);
    }

    const uint32_t d = vivants[v];
    Snake &serpent = serpents[d];
    if (not serpent.getEstEnVie()) {
      continue;
    }
    const uint32_t id = d + 1;
    const Pomme &pomme = pommes[d];

    // La position de tête d'il y a deux tours est oubliée ; celle du tour
    // précédent reste visible pendant ce tour (« ancienne tête »). Aucun
    // autre thread ne touche à ces cases pendant cette phase.
    Case &avant = cases[ancienneTete[d]];
    if (avant.teteId == id and avant.teteParite == parite()) {
      avant.teteId = AUCUNE_TETE;
    }
    ancienneTete[d] = uint32_t(indexCase(serpent.getCoordX(), serpent.getCoordY()));

    serpent.deplacerVersXY(pomme.getCoordX(), pomme.getCoordY(), hasard(d));

    for (const CoordonneesXY &c : serpent.getCasesAjoutees()) {
      boite.mouvements[regionDe(c.y)].push_back({uint32_t(indexCase(c.x, c.y)), id});
    }
    for (const CoordonneesXY &c : serpent.getCasesRetirees()) {
      boite.mouvements[regionDe(c.y)].push_back(
          {uint32_t(indexCase(c.x, c.y)), id | RETRAIT});
    }
    serpent.oublierModifications();

    const CoordonneesXY &tete = serpent.getCoord().front();
    const uint32_t celluleTete = uint32_t(indexCase(tete.x, tete.y));
    resumes[d] = {celluleTete, uint32_t(serpent.getTaille())};
    boite.tetes[regionDe(tete.y)].push_back({celluleTete, id, tailleTete(id)});
  }
}

//--- 2. Grille (par région) ---------------------------------------------
void Combat::phaseGrille(unsigned r) {

  Region &region = regions[r];

  // D'abord toutes les arrivées et départs de segments...
  for (unsigned b = 0; b < boitesActives; ++b) {
    Boite &boite = boites[b];
    vector<Mouvement> &liste = boite.mouvements[r];
    // (les premiers aussi : sur une courte liste, ce sont presque tous)
    for (size_t k = 0; k < min(AVANCE_MESSAGES, liste.size()); ++k) {
      precharger(&cases[liste[k].cellule]);
    }
    for (size_t k = 0; k < liste.size(); ++k) {
      if (k + AVANCE_MESSAGES < liste.size()) {
        precharger(&cases[liste[k + AVANCE_MESSAGES].cellule]);
      }
      const Mouvement &m = liste[k];
      const bool retrait = (m.idEtSens & RETRAIT) != 0;
      modifierCase(m.cellule, region, retrait ? -1 : +1, 0, m.idEtSens & ~RETRAIT);
    }
    liste.clear();
  }

  const uint32_t tour = parite();

  // ... puis les anciennes têtes trouvées à l'arrivée, avant que les
  // nouvelles ne les remplacent (toutes les arrivées doivent les voir)...
  for (unsigned b = 0; b < boitesActives; ++b) {
    const Boite &boite = boites[b];
    for (const ArriveeTete &a : boite.tetes[r]) {
      const Case &c = cases[a.cellule];
      const bool ancienne = c.teteId != AUCUNE_TETE and c.teteId != a.id
          and c.teteParite != tour;
      rencontre[a.id - 1] = ancienne ? c.teteId : AUCUNE_TETE;
    }
  }

  // ... puis les nouvelles têtes
  for (unsigned b = 0; b < boitesActives; ++b) {
    Boite &boite = boites[b];
    vector<ArriveeTete> &liste = boite.tetes[r];
    // (les premiers aussi : sur une courte liste, ce sont presque tous)
    for (size_t k = 0; k < min(AVANCE_MESSAGES, liste.size()); ++k) {
      precharger(&cases[liste[k].cellule]);
    }
    for (size_t k = 0; k < liste.size(); ++k) {
      if (k + AVANCE_MESSAGES < liste.size()) {
        precharger(&cases[liste[k + AVANCE_MESSAGES].cellule]);
      }
      const ArriveeTete &a = liste[k];
      Case &c = cases[a.cellule];

      // Plusieurs têtes sur la même case : la plus longue la garde
      if (c.teteId == AUCUNE_TETE or c.teteParite != tour
          or gagneContre(a.taille, a.id, tailleTete(c.teteId), uint32_t(c.teteId))) {
        c.teteId = a.id;
        c.teteParite = tour;
      }

      // Tête arrivée sur un corps : prévenir la victime possible
      const int32_t autres = int32_t(c.serpents) - 1;
      if (autres == 1) {
        const uint32_t proprietaire = c.xorSerpents ^ a.id;
        if (proprietaire != a.id and proprietaire - 1 < nbSerpentsDepart) {
          menace[proprietaire - 1].store(1, memory_order_relaxed);
        }
      } else if (autres >= 2) {
        // Propriétaires inconnus : la tuile de la case est marquée, et les
        // serpents qui peuvent l'atteindre vérifieront tout leur corps
        victimeInconnue.store(true, memory_order_relaxed);
        uint8_t &tuile = tuilesAmbigues[tuileDe(a.cellule)];
        if (not tuile) {
          tuile = 1;
          region.tuilesMarquees.push_back(tuileDe(a.cellule));
        }
      }
    }
    liste.clear();
  }
}

//--- 3. Combats (par serpent) : qui meurt ? -------------------------------
void Combat::phaseCombats(size_t debut, size_t fin) {

  // Règles, évaluées pour tous en même temps (l'ordre ne compte pas) :
  //  - plusieurs têtes sur la même case : seule la plus longue survit
  //  - une tête arrive sur la case que la tête d'un autre vient de quitter :
  //    les deux se battent et le plus court meurt. Sans cette règle, deux
  //    têtes ne se rencontreraient que si leurs positions avaient la même
  //    parité (comme les cases d'un damier) et des serpents de parités
  //    différentes ne pourraient jamais se tuer.
  // Un serpent qui perd l'un de ses combats meurt.
  const uint32_t tour = parite();

  for (size_t v = debut; v < min(debut + AVANCE, fin); ++v) {
    precharger(&cases[resumes[vivants[v]].teteCase]);
  }
  for (size_t v = debut; v < fin; ++v) {
    if (v + AVANCE < fin) {
      precharger(&cases[resumes[vivants[v + AVANCE]].teteCase]);
    }

    const uint32_t d = vivants[v];
    const Resume resume = resumes[d];
    if (resume.taille == 0) {
      continue;  // mort
    }
    const uint32_t id = d + 1;
    const uint32_t taille = resume.taille;

    uint32_t tueur = AUCUNE_TETE;
    const uint32_t occupant = uint32_t(cases[resume.teteCase].teteId);
    if (occupant != id) {
      tueur = occupant;  // perdu sur la case d'arrivée
    } else {
      // Arrivé sur l'ancienne tête d'un autre
      const uint32_t ancien = rencontre[d];
      if (ancien != AUCUNE_TETE
          and not gagneContre(taille, id, resumes[ancien - 1].taille, ancien)) {
        tueur = ancien;
      }
      // Un autre est arrivé sur mon ancienne tête
      const Case &quittee = cases[ancienneTete[d]];
      const uint32_t arrivant = uint32_t(quittee.teteId);
      if (tueur == AUCUNE_TETE and arrivant != AUCUNE_TETE and arrivant != id
          and quittee.teteParite == tour
          and not gagneContre(taille, id, resumes[arrivant - 1].taille, arrivant)) {
        tueur = arrivant;
      }
    }
    mortPar[d] = tueur;
  }
}

//--- 4. Conséquences (par serpent) : morts, morsures, repas --------------
void Combat::emettreRetraits(Snake &serpent, uint32_t id, Boite &boite) {
  for (const CoordonneesXY &c : serpent.getCasesRetirees()) {
    boite.mouvements[regionDe(c.y)].push_back(
        {uint32_t(indexCase(c.x, c.y)), id | RETRAIT});
  }
  serpent.oublierModifications();
}

void Combat::phaseConsequences(unsigned thread, size_t debut, size_t fin) {

  Boite &boite = boites[thread];
  const bool tousVerifient = victimeInconnue.load(memory_order_relaxed);
  const uint32_t tour = parite();

  for (size_t v = debut; v < fin; ++v) {
    const uint32_t d = vivants[v];
    if (resumes[d].taille == 0) {
      continue;  // mort
    }
    const uint32_t id = d + 1;

    // Mort : le corps disparaît, ses têtes (actuelle et précédente) aussi ;
    // la récompense du vainqueur est distribuée à la résolution
    if (mortPar[d] != AUCUNE_TETE) {
      Snake &serpent = serpents[d];
      const CoordonneesXY &tete = serpent.getCoord().front();
      const uint32_t cellule = uint32_t(indexCase(tete.x, tete.y));
      boite.effacements[regionDe(tete.y)].push_back({cellule, id});
      boite.effacements[regionDe(int(ancienneTete[d] / largeur))].push_back(
          {ancienneTete[d], id});
      const size_t longueur = serpent.mourir();
      resumes[d].taille = 0;
      boite.morts.push_back({d, mortPar[d] - 1, uint32_t(longueur)});
      emettreRetraits(serpent, id, boite);
      menace[d].store(0, memory_order_relaxed);
      continue;
    }

    // Tête sur corps : coupé au premier segment (depuis la tête) sur lequel
    // se trouve la tête d'un autre serpent encore en vie. Seuls les
    // serpents menacés parcourent leur corps.
    if (menace[d].load(memory_order_relaxed)
        or (tousVerifient and peutAtteindreAmbigue(resumes[d]))) {
      menace[d].store(0, memory_order_relaxed);
      Snake &serpent = serpents[d];
      const FileCirculaire<CoordonneesXY> &corps = serpent.getCoord();
      for (size_t k = 1; k < corps.size(); ++k) {
        const Case &c = cases[indexCase(corps[k].x, corps[k].y)];
        const uint32_t autre = uint32_t(c.teteId);
        if (autre != AUCUNE_TETE and autre != id and c.teteParite == tour
            and mortPar[autre - 1] == AUCUNE_TETE) {
          const size_t coupes = serpent.etreMordu(k);
          resumes[d].taille = uint32_t(serpent.getTaille());
          if (coupes > 0) {
            boite.morsures.push_back({autre - 1, uint32_t(coupes)});
            emettreRetraits(serpent, id, boite);
          }
          break;
        }
      }
    }

    const Pomme &pomme = pommes[d];
    if (resumes[d].teteCase == indexCase(pomme.getCoordX(), pomme.getCoordY())) {
      boite.repas.push_back(d);
    }
  }
}

//--- 5. Retraits (par région) --------------------------------------------
void Combat::phaseRetraits(unsigned r) {

  Region &region = regions[r];
  for (unsigned b = 0; b < boitesActives; ++b) {
    Boite &boite = boites[b];
    vector<Mouvement> &liste = boite.mouvements[r];
    // (les premiers aussi : sur une courte liste, ce sont presque tous)
    for (size_t k = 0; k < min(AVANCE_MESSAGES, liste.size()); ++k) {
      precharger(&cases[liste[k].cellule]);
    }
    for (size_t k = 0; k < liste.size(); ++k) {
      if (k + AVANCE_MESSAGES < liste.size()) {
        precharger(&cases[liste[k + AVANCE_MESSAGES].cellule]);
      }
      const Mouvement &m = liste[k];
      modifierCase(m.cellule, region, -1, 0, m.idEtSens & ~RETRAIT);
    }
    liste.clear();

    for (const Effacement &e : boite.effacements[r]) {
      if (cases[e.cellule].teteId == e.id) {
        cases[e.cellule].teteId = AUCUNE_TETE;
      }
    }
    boite.effacements[r].clear();
  }

  for (uint32_t tuile : region.tuilesMarquees) {
    tuilesAmbigues[tuile] = 0;
  }
  region.tuilesMarquees.clear();
}

//--- 6. Résolution (un seul thread) --------------------------------------
void Combat::phaseResolution() {

  nbMouvements += nbSerpent;

  // (tous les threads ont fini : compteurs de répartition remis à zéro pour
  // le tour suivant)
  for (unsigned phase = 0; phase < NB_COMPTEURS; ++phase) {
    remettreCompteurs(phase);
  }

  // Dans l'ordre des numéros de serpent : le résultat ne dépend pas de la
  // façon dont les serpents étaient répartis entre threads
  vector<Mort> &morts = boites[0].morts;
  vector<Morsure> &morsures = boites[0].morsures;
  vector<uint32_t> &repas = boites[0].repas;
  for (size_t t = 1; t < boitesActives; ++t) {
    morts.insert(morts.end(), boites[t].morts.begin(), boites[t].morts.end());
    morsures.insert(morsures.end(), boites[t].morsures.begin(), boites[t].morsures.end());
    repas.insert(repas.end(), boites[t].repas.begin(), boites[t].repas.end());
    boites[t].morts.clear();
    boites[t].morsures.clear();
    boites[t].repas.clear();
  }

  // Morts : annonce, récompense du vainqueur, pomme retirée
  sort(morts.begin(), morts.end(),
       [](const Mort &a, const Mort &b) { return a.victime < b.victime; });
  string annonces;
  for (const Mort &m : morts) {
    if (not silencieux) {
      annonces += "Le serpent ";
      annonces += to_string(m.tueur + 1);
      annonces += " a tuer le serpent ";
      annonces += to_string(m.victime + 1);
      annonces += '\n';
    }
    serpents[m.tueur].recompenserVictoire(m.longueur);
    Pomme &pomme = pommes[m.victime];
    if (pomme.estIntacte()) {
      pomme.pommeEstMangee();
      modifierCase(pomme.getCoordX(), pomme.getCoordY(), 0, -1);
    }
    --nbSerpent;
    ++mortsDansVivants;
  }
  if (not annonces.empty()) {
    cout << annonces;
  }

  // Morsures (additions : l'ordre ne compte pas)
  for (const Morsure &m : morsures) {
    serpents[m.attaquant].recompenserMorsure(m.longueurCoupee);
  }

  // Pommes mangées (dans l'ordre : les nouvelles places sont tirées au sort)
  sort(repas.begin(), repas.end());
  for (uint32_t d : repas) {
    Snake &serpent = serpents[d];
    Pomme &pomme = pommes[d];
    modifierCase(pomme.getCoordX(), pomme.getCoordY(), 0, -1);
    const CoordonneesXY nouvelleCoord = generateurDeCoord();
    serpent.mangerPomme(pomme.getValeur());
    pomme.setCoordPomme(nouvelleCoord.x, nouvelleCoord.y);
    modifierCase(nouvelleCoord.x, nouvelleCoord.y, 0, +1);
    pomme.setValPomme();
  }

  morts.clear();
  morsures.clear();
  repas.clear();
  victimeInconnue.store(false, memory_order_relaxed);

  ++nbTours;
  // Les morts restent dans vivants (et sont sautés) jusqu'à ce qu'ils soient
  // nombreux
  if (mortsDansVivants * 8 > vivants.size() or nbSerpent <= 1) {
    retirerMorts();
  }
}

void Combat::retirerMorts() {
  // (l'ordre des numéros est gardé : il suit la position de départ)
  vivants.erase(remove_if(vivants.begin(), vivants.end(),
                          [this](uint32_t d) {
                            return resumes[d].taille == 0;
                          }),
                vivants.end());
  mortsDansVivants = 0;
}

//------------------------- méthodes d'affichage ------------------------

bool Combat::faireCombattreSerpents(Affichage2d &affichage) {

  // Deux threads qui ne s'attendent presque jamais :
  //  - le moteur enchaîne les lots de tours sur le groupe de threads ;
  //  - le thread principal, sur son propre cœur, prend chaque liste de
  //    cases à recolorier, colorie les pixels, envoie l'image à l'écran et
  //    lit le clavier.
  // Boîte aux lettres : une seule liste déposée à la fois. En vitesse
  // automatique, si l'affichage n'a pas encore pris la précédente, le
  // moteur ne l'attend pas : il continue de jouer et les cases modifiées
  // s'accumulent (sans doublons) jusqu'à ce que la boîte se libère. Il ne
  // prépare donc des listes qu'au rythme où l'écran les consomme, quelle que
  // soit la lenteur de l'affichage. En vitesse fixe (N tours par image),
  // c'est l'affichage qui donne la cadence : le moteur attend.
  mutex verrou;
  condition_variable signal;
  bool deposee = false, termine = false, arreter = false;
  Image enPreparation, prete, affichee;

  // Profil : chaque compteur n'est écrit que par un thread et lu après join
  auto maintenantMs = [] {
    return double(SDL_GetPerformanceCounter()) * 1000.0
        / double(SDL_GetPerformanceFrequency());
  };
  double moteurCalcul = 0, moteurPreparation = 0, moteurAttente = 0;
  unsigned long nbDepots = 0, nbLotsSansDepot = 0;
  double principalPixels = 0, principalAttente = 0, principalPresentation = 0;

  thread moteur([&] {
    unsigned long faits = 0;  // tours depuis la dernière liste déposée
    for (;;) {
      const unsigned v = vitesse;
      const double t0 = maintenantMs();
      if (nbSerpent > 1) {
        faits += v == VITESSE_AUTO
            ? jouerTours(ULONG_MAX, SDL_GetTicks() + dureeLotAuto())
            : jouerTours(v, 0);
      }
      moteurCalcul += maintenantMs() - t0;
      const bool fin = nbSerpent <= 1;

      {
        lock_guard<mutex> garde(verrou);
        if (arreter) {
          return;
        }
        if (deposee and v == VITESSE_AUTO and not fin) {
          ++nbLotsSansDepot;
          continue;  // boîte pleine : on joue encore, on déposera plus tard
        }
      }

      // Seul le moteur remplit la boîte : si elle était libre, elle le reste
      const double t1 = maintenantMs();
      preparerImage(enPreparation);
      moteurPreparation += maintenantMs() - t1;
      ++nbDepots;
      enPreparation.tours = nbTours;
      enPreparation.serpents = nbSerpent;
      enPreparation.toursLot = faits;
      faits = 0;
      {
        unique_lock<mutex> garde(verrou);
        const double t2 = maintenantMs();
        signal.wait(garde, [&] { return not deposee or arreter; });
        moteurAttente += maintenantMs() - t2;
        if (arreter) {
          return;
        }
        swap(enPreparation, prete);
        deposee = true;
        termine = fin;
      }
      signal.notify_all();
      if (fin) {
        return;
      }
    }
  });

  bool quitter = false;
  bool fini = false;
  Uint32 dernierTitre = 0;
  while (not fini) {
    {
      unique_lock<mutex> garde(verrou);
      const double t0 = maintenantMs();
      signal.wait(garde, [&] { return deposee; });
      principalAttente += maintenantMs() - t0;
      swap(prete, affichee);
      deposee = false;
      fini = termine;
    }
    signal.notify_all();  // le moteur peut déposer la suivante

    const double t1 = maintenantMs();
    appliquerImage(affichage, affichee);
    const double t2 = maintenantMs();
    int accelerer = 0;
    quitter = presenter(affichage, accelerer);
    principalPixels += t2 - t1;
    principalPresentation += maintenantMs() - t2;
    if (quitter) {
      break;  // fenêtre fermée ou ÉCHAP
    }
    if (accelerer != 0) {
      changerVitesse(accelerer);
      dernierTitre = 0;  // titre mis à jour tout de suite
    }
    if (SDL_GetTicks() - dernierTitre >= 250) {
      mettreAJourTitre(affichage, affichee);
      dernierTitre = SDL_GetTicks();
    }
  }

  {
    lock_guard<mutex> garde(verrou);
    arreter = true;
  }
  signal.notify_all();
  moteur.join();

  if (profil) {
    auto ms = [](double v) { return to_string(long(v)) + " ms"; };
    cout << "\n--- Profil (" << pool.taille() << " threads de calcul, ecran "
         << frequenceEcran << " Hz, lots de " << dureeLotAuto() << " ms en vitesse auto)\n"
         << "Partie : graine " << graineDePartie() << ", " << nbTours << " tours, "
         << nbMouvements << " deplacements, "
         << (nbMouvements ? moteurCalcul * 1e6 / double(nbMouvements) : 0.0)
         << " ns de calcul par deplacement\n"
         << "  tours a plusieurs threads : " << toursMode[1] << " en "
         << ms(double(dureeMode[1]) * 1000.0 / double(SDL_GetPerformanceFrequency()))
         << ", a un thread (moins de " << SEUIL_PARALLELE << " serpents) : "
         << toursMode[0] << " en "
         << ms(double(dureeMode[0]) * 1000.0 / double(SDL_GetPerformanceFrequency()))
         << "\n" << detailSerie()
         << "Moteur : calcul " << ms(moteurCalcul) << ", preparation des images "
         << ms(moteurPreparation) << ", attente de l'affichage " << ms(moteurAttente)
         << "\n  " << nbDepots << " images deposees, " << nbLotsSansDepot
         << " lots sans depot (affichage occupe)\n"
         << "Affichage : attente du moteur " << ms(principalAttente)
         << ", pixels " << ms(principalPixels) << ", envoi + presentation + clavier "
         << ms(principalPresentation) << "\n"
         << affichage.rapportProfil() << endl;
  }

  return not quitter;
}

Uint32 Combat::dureeLotAuto() const {
  // Vitesse automatique : un lot dure une image de l'écran, mais au moins
  // 16 ms. Sur un écran à 144 Hz ou plus, des lots plus courts multiplient
  // les images (préparation, coloriage, envoi à la carte graphique) sans
  // rien montrer de plus utile : le calcul ralentissait au début de partie.
  if (delai > 0) {
    return delai;
  }
  return max(16u, 1000u / max(1u, frequenceEcran));
}

void Combat::changerVitesse(int pas) {

  // Paliers parcourus avec + / - ; le dernier est la vitesse automatique
  static const unsigned PALIERS[] = {1, 2, 5, 10, 20, 50, 100, 200, 500,
                                     1000, VITESSE_AUTO};
  const int nbPaliers = int(sizeof(PALIERS) / sizeof(PALIERS[0]));

  const unsigned actuelle = vitesse;
  int palier = nbPaliers - 1;
  if (actuelle != VITESSE_AUTO) {
    palier = 0;
    while (palier < nbPaliers - 2 and PALIERS[palier] < actuelle) {
      ++palier;
    }
  }
  palier = std::max(0, std::min(nbPaliers - 1, palier + pas));
  vitesse = PALIERS[palier];
}

void Combat::mettreAJourTitre(Affichage2d &affichage, const Image &image) const {
  const unsigned v = vitesse;
  string titre = "Snake battle simulator - "s + to_string(image.serpents)
      + " serpents - tour "s + to_string(image.tours) + " - vitesse "s;
  titre += v == VITESSE_AUTO
      ? "auto (x"s + to_string(image.toursLot) + ")"s
      : "x"s + to_string(v);
  affichage.definirTitre(titre + "  [+/-]"s);
}

void Combat::preparerImage(Image &image) {

  // Côté moteur : les cases modifiées depuis la dernière image deviennent
  // une liste (case, couleur). Seul ce passage lit le terrain ; le
  // coloriage des pixels se fera sur le thread principal pendant que le
  // moteur repart. Chaque région par son thread s'il y a beaucoup de cases.
  image.parRegion.resize(regions.size());
  size_t total = 0;
  for (const Region &region : regions) {
    total += region.casesModifiees.size();
  }
  if (pool.taille() > 1 and total >= SEUIL_DESSIN_PARALLELE) {
    remettreCompteurs(PHASE_IMAGE);
    pool.executer([&](unsigned thread) {
      repartir(PHASE_IMAGE, thread, regions.size(), 1, [&](size_t debut, size_t fin) {
        for (size_t r = debut; r < fin; ++r) {
          preparerRegion(regions[r], image.parRegion[r]);
        }
      });
    });
  } else {
    for (size_t r = 0; r < regions.size(); ++r) {
      preparerRegion(regions[r], image.parRegion[r]);
    }
  }
}

void Combat::preparerRegion(Region &region, vector<uint32_t> &sortie) {

  // Une pomme reste dessinée par-dessus un serpent. Les cases sont
  // éparpillées : préchargées quelques-unes à l'avance.
  sortie.clear();
  vector<uint32_t> &liste = region.casesModifiees;
  for (size_t k = 0; k < min(AVANCE_MESSAGES, liste.size()); ++k) {
    precharger(&cases[liste[k]]);
  }
  for (size_t k = 0; k < liste.size(); ++k) {
    if (k + AVANCE_MESSAGES < liste.size()) {
      precharger(&cases[liste[k + AVANCE_MESSAGES]]);
    }
    const uint32_t i = liste[k];
    Case &c = cases[i];
    uint32_t couleur = Couleur::blanc;
    if (c.pommes > 0) {
      couleur = Couleur::rouge;
    } else if (c.serpents > 0) {
      couleur = Couleur::noir;
    }
    sortie.push_back(i | (couleur << 30));
    c.marquee = 0;
  }
  liste.clear();
}

void Combat::appliquerImage(Affichage2d &affichage, const Image &image) const {
  // Côté affichage : les pixels sont coloriés depuis la liste, sans toucher
  // au terrain (que le moteur est déjà en train de modifier)
  for (const vector<uint32_t> &liste : image.parRegion) {
    for (size_t k = 0; k < liste.size(); ++k) {
      if (k + AVANCE_MESSAGES < liste.size()) {
        const uint32_t suivante = liste[k + AVANCE_MESSAGES] & MASQUE_CASE;
        affichage.prechargerElement(int(suivante % largeur), int(suivante / largeur));
      }
      const uint32_t i = liste[k] & MASQUE_CASE;
      affichage.ajouterElementAffichage(int(i % largeur), int(i / largeur),
                                        Couleur(liste[k] >> 30));
    }
  }
}

bool Combat::presenter(Affichage2d &affichage, int &accelerer) {
  affichage.mettreAjourAffichage();
  return affichage.fermetureDemandee(accelerer);
}

//------------------------- fin de partie -------------------------------

void Combat::afficherVictoire(Affichage2d &affichage) {

  // Le vainqueur reste en évidence sur le terrain assombri
  const Snake &gagnant = serpents[vivants.front()];
  const auto &corps = gagnant.getCoord();
  vector<SDL_Point> cases;
  cases.reserve(corps.size());
  for (size_t k = 0; k < corps.size(); ++k) {
    cases.push_back({corps[k].x, corps[k].y});
  }

  const vector<string> stats = statistiques();
  cout << "\nVICTOIRE DU SERPENT #" << gagnant.getId() << " !\n\n";
  for (const string &ligne : stats) {
    cout << ligne << '\n';
  }
  cout << endl;

  // Rapport officiel de la Green Katze Korporation
  vector<string> panneau = {
      "^ GREEN KATZE KORPORATION",
      "~ RAPPORT DE SIMULATION",
      "",
      "# VICTOIRE !",
      "^ LE SERPENT #" + to_string(gagnant.getId()) + " A GAGNE",
      "",
  };
  panneau.insert(panneau.end(), stats.begin(), stats.end());
  panneau.insert(panneau.end(), {
      "",
      "^ LA GREEN KATZE KORPORATION",
      "^ VOUS REMERCIE D'AVOIR UTILISE",
      "^ SNAKE BATTLE SIMULATOR.",
      "~ VOS PRECIEUSES DONNEES SERVIRONT",
      "~ L'AVENIR DE L'HUMANITE.",
      "",
      "~ ECHAP / ENTREE : QUITTER",
  });

  if (not finAuto) {
    affichage.afficherEcranFin(logoGreenKatze(), panneau, cases);
  }
}

MotifPixel Combat::logoGreenKatze() {
  // Le chat vert de la Korporation, 15 x 13 pixels
  return {
      {
          "g.............g",
          "gg...........gg",
          "gpg.........gpg",
          "gppgggggggggppg",
          "ggggggggggggggg",
          "ggyyygggggyyygg",
          "ggykygggggykygg",
          "ggyyygggggyyygg",
          "gggggggpggggggg",
          "wwgggggkgggggww",
          ".gggggkgkggggg.",
          "..ggggggggggg..",
          "....ggggggg....",
      },
      {
          {'g', {60, 200, 90, 255}},    // pelage
          {'p', {240, 140, 170, 255}},  // oreilles et truffe
          {'y', {250, 220, 60, 255}},   // yeux
          {'k', {15, 15, 15, 255}},     // pupilles et bouche
          {'w', {235, 235, 235, 255}},  // moustaches
      },
  };
}

vector<string> Combat::statistiques() const {

  const Snake &gagnant = serpents[vivants.front()];
  const StatsSerpent &sg = gagnant.getStats();

  // Totaux et records de la partie, morts compris
  unsigned long pommesTotal = 0, morsuresTotal = 0;
  const Snake *plusLong = &gagnant, *plusMeurtrier = &gagnant;
  for (const Snake &s : serpents) {
    const StatsSerpent &st = s.getStats();
    pommesTotal += st.pommes;
    morsuresTotal += st.morsuresInfligees;
    if (st.longueurMax > plusLong->getStats().longueurMax) {
      plusLong = &s;
    }
    if (st.victimes > plusMeurtrier->getStats().victimes) {
      plusMeurtrier = &s;
    }
  }

  const unsigned secondes = dureeMs / 1000;
  const string duree = secondes >= 60
      ? to_string(secondes / 60) + " MIN " + to_string(secondes % 60) + " S"
      : to_string(secondes) + "." + to_string(dureeMs % 1000 / 100) + " S";

  // Libellé aligné sur 19 caractères (police à chasse fixe)
  auto ligne = [](string libelle, const string &valeur) {
    libelle.resize(19, ' ');
    return libelle + valeur;
  };
  auto id = [](const Snake &s) { return "#" + to_string(s.getId()); };

  return {
      "-- LE VAINQUEUR",
      ligne("LONGUEUR FINALE", to_string(gagnant.getCoord().size())),
      ligne("LONGUEUR MAX", to_string(sg.longueurMax)),
      ligne("VICTIMES", to_string(sg.victimes)),
      ligne("POMMES MANGEES", to_string(sg.pommes)),
      ligne("MORSURES DONNEES", to_string(sg.morsuresInfligees)),
      ligne("MORSURES SUBIES", to_string(sg.morsuresSubies)),
      "",
      "-- LA PARTIE",
      ligne("SERPENTS", to_string(nbSerpentsDepart)),
      ligne("TERRAIN", to_string(largeur) + " X " + to_string(longueur)),
      ligne("DUREE", duree),
      ligne("TOURS", to_string(nbTours)),
      ligne("POMMES MANGEES", to_string(pommesTotal)),
      ligne("MORSURES", to_string(morsuresTotal)),
      ligne("PLUS LONG", to_string(plusLong->getStats().longueurMax)
          + " (" + id(*plusLong) + ")"),
      ligne("PLUS DE VICTIMES", to_string(plusMeurtrier->getStats().victimes)
          + " (" + id(*plusMeurtrier) + ")"),
  };
}
