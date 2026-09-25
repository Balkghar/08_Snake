/*
---------------------------------------------------------------------------
Fichier     : combatSnakes.cpp
Nom du labo : Labo8 - Snake
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 10.01.2023
But         : Définition des fonctions membres de la classe
              combat.

Remarque(s) :

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#include "combatSnakes.hpp"
#include "../outils/aleatoire.hpp"
#include "../outils/precharger.hpp"
#include <algorithm>
#include <iostream>

using namespace std;
//=========================== Partie public ===============================

//------------------------- Constructeur --------------------------------
Combat::Combat(unsigned int largeur,
               unsigned int longueur,
               unsigned int nbSerpent
) : largeur(largeur), longueur(longueur), nbSerpent(nbSerpent),
    nbSerpentsDepart(nbSerpent) {

  // L'affichage SDL partant de 0, pour que l'affichage soit correct à l'écran,
  // il faut faire moins 1 à la valeur entrée par l'utilisateur.
  longueurAffichage = longueur - 1;
  largeurAffichage = largeur - 1;

  const size_t nbCases = size_t(largeur) * longueur;
  cases.assign(nbCases, Case());
  tetesBits.assign((nbCases + 63) / 64, 0);
  morsuresEnAttente.assign(nbSerpent, 0);
  arrivee.assign(nbSerpent, AUCUNE_TETE);
  caseMarquee.assign(nbCases, false);

  initialiserSerpent();
  initialiserPomme();
}
//------------------------- lancement du combat -------------------------
void Combat::commencerCombat(unsigned delai, unsigned zoom, unsigned vitesse) {

  this->delai = delai;
  this->vitesse = vitesse;
  Affichage2d affichage(largeur, longueur, delai, zoom);

  if (not affichage.initalisationAffichage()) {
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

  for (unsigned i = 1; i <= Combat::nbSerpent; ++i) {
    CoordonneesXY nouvelleCoord = generateurDeCoord();

    serpents.emplace_back(nouvelleCoord.x, nouvelleCoord.y, i, true, 10);
    appliquerModifications(serpents.back());
    poserTete(indexCase(nouvelleCoord.x, nouvelleCoord.y), i);
    vivants.push_back(i - 1);
  }

}

CoordonneesXY Combat::generateurDeCoord() {
  const size_t nbCases = size_t(largeur) * longueur;
  CoordonneesXY nouvelleCoord = {0, 0};

  // Tirages aléatoires tant que le terrain est peu rempli...
  for (unsigned essai = 0; essai < 64 or nbCasesOccupees >= nbCases; ++essai) {
    nouvelleCoord.x = aleatoireEntreDeuxValeurs(MIN, (int) largeurAffichage);
    nouvelleCoord.y = aleatoireEntreDeuxValeurs(MIN, (int) longueurAffichage);
    if (nbCasesOccupees >= nbCases
        or not placeEstOccupee(indexCase(nouvelleCoord.x, nouvelleCoord.y))) {
      // (terrain plein : on accepte une case occupée plutôt que de boucler)
      return nouvelleCoord;
    }
  }

  // ... sinon on parcourt le terrain depuis une case aléatoire
  const size_t depart = size_t(aleatoireEntreDeuxValeurs(0, int(nbCases - 1)));
  for (size_t k = 0; k < nbCases; ++k) {
    const size_t i = (depart + k) % nbCases;
    if (not placeEstOccupee(i)) {
      nouvelleCoord.x = int(i % largeur);
      nouvelleCoord.y = int(i / largeur);
      break;
    }
  }
  return nouvelleCoord;
}

//------------------------- grille d'occupation -------------------------
bool Combat::placeEstOccupee(size_t i) const {
  return cases[i].serpents > 0 or cases[i].pommes > 0;
}

size_t Combat::indexCase(int x, int y) const {
  return size_t(y) * largeur + size_t(x);
}

void Combat::modifierCase(int x, int y, int deltaSerpent, int deltaPomme,
                          unsigned serpent) {
  if (x < 0 or y < 0 or unsigned(x) >= largeur or unsigned(y) >= longueur) {
    return;
  }
  const size_t i = indexCase(x, y);
  const bool etaitOccupee = placeEstOccupee(i);
  cases[i].serpents += deltaSerpent;
  cases[i].pommes += deltaPomme;
  cases[i].xorSerpents ^= serpent;
  const bool estOccupee = placeEstOccupee(i);

  if (estOccupee != etaitOccupee) {
    estOccupee ? ++nbCasesOccupees : --nbCasesOccupees;
  }

  // Une case modifiée plusieurs fois entre deux images n'est redessinée
  // qu'une fois
  if (not caseMarquee[i]) {
    caseMarquee[i] = true;
    casesModifiees.push_back({x, y});
  }
}

void Combat::poserTete(size_t i, unsigned serpent) {
  cases[i].tete = serpent;
  if (serpent == AUCUNE_TETE) {
    tetesBits[i / 64] &= ~(std::uint64_t(1) << (i % 64));
  } else {
    tetesBits[i / 64] |= std::uint64_t(1) << (i % 64);
  }
}

bool Combat::tetePresente(size_t i) const {
  return (tetesBits[i / 64] >> (i % 64)) & 1;
}

void Combat::enregistrerArrivee(size_t indice) {
  const Snake &serpent = serpents[indice];
  const size_t c = indexCase(serpent.getCoordX(), serpent.getCoordY());
  const int autres = cases[c].serpents - 1;  // sans la tête elle-même
  if (autres <= 0) {
    return;
  }
  if (autres == 1) {
    const unsigned victime = cases[c].xorSerpents ^ unsigned(indice + 1);
    if (victime != indice + 1) {  // (son propre corps : pas une morsure)
      arrivee[indice] = victime;
      ++morsuresEnAttente[victime - 1];
    }
  } else {
    arrivee[indice] = VICTIME_INCONNUE;
    ++morsuresInconnues;
  }
}

void Combat::oublierArrivee(size_t indice) {
  const unsigned victime = arrivee[indice];
  if (victime == VICTIME_INCONNUE) {
    --morsuresInconnues;
  } else if (victime != AUCUNE_TETE) {
    --morsuresEnAttente[victime - 1];
  }
  arrivee[indice] = AUCUNE_TETE;
}

void Combat::appliquerModifications(Snake &serpent) {
  const unsigned id = unsigned(&serpent - serpents.data()) + 1;
  for (const CoordonneesXY &coord : serpent.getCasesAjoutees()) {
    modifierCase(coord.x, coord.y, +1, 0, id);
  }
  for (const CoordonneesXY &coord : serpent.getCasesRetirees()) {
    modifierCase(coord.x, coord.y, -1, 0, id);
  }
  serpent.oublierModifications();
}

//------------------------- méthodes du jeu -----------------------------

void Combat::mangerPomme(Snake &serpent, Pomme &pomme) {

  if (serpent.getCoordX() == pomme.getCoordX() && serpent.getCoordY() == pomme.getCoordY()) {
    modifierCase(pomme.getCoordX(), pomme.getCoordY(), 0, -1);
    CoordonneesXY nouvelleCoord = generateurDeCoord();

    serpent.mangerPomme(pomme.getValeur());
    pomme.setCoordPomme(nouvelleCoord.x, nouvelleCoord.y);
    modifierCase(nouvelleCoord.x, nouvelleCoord.y, 0, +1);
    pomme.setValPomme();
  }
}

void Combat::jouerTour() {

  // Seuls les serpents en vie sont parcourus : avec beaucoup de serpents,
  // la plupart meurent vite et les reparcourir à chaque tour coûte cher
  const size_t AVANCE = 4;
  for (size_t v = 0; v < vivants.size() and nbSerpent > 1; ++v) {
    // Préchargement des cases de la tête et de la queue d'un serpent qui
    // jouera un peu plus tard (serpents éparpillés : défauts de cache)
    if (v + AVANCE < vivants.size()) {
      const Snake &prochain = serpents[vivants[v + AVANCE]];
      const CoordonneesXY &tete = prochain.getCoord().front();
      const CoordonneesXY &queue = prochain.getCoord().back();
      precharger(&cases[indexCase(tete.x, tete.y)]);
      precharger(&cases[indexCase(queue.x, queue.y)]);
    }

    const size_t d = vivants[v];
    Snake &serpent = serpents[d];
    Pomme &pomme = pommes[d];

    if (not serpent.getEstEnVie()) {
      continue;  // tué plus tôt dans ce tour
    }

    // La tête quitte sa case
    oublierArrivee(d);
    const size_t tete = indexCase(serpent.getCoordX(), serpent.getCoordY());
    if (cases[tete].tete == d + 1) {
      poserTete(tete, AUCUNE_TETE);
    }

    serpent.deplacerVersXY(pomme.getCoordX(), pomme.getCoordY());
    // grille à jour avant de chercher une case libre pour la pomme
    appliquerModifications(serpent);
    mangerPomme(serpent, pomme);
    combatSerpent(d);
  }

  vivants.erase(remove_if(vivants.begin(), vivants.end(),
                          [this](unsigned d) {
                            return not serpents[d].getEstEnVie();
                          }),
                vivants.end());
}

void Combat::combatSerpent(size_t indice) {

  Snake &serpent = serpents[indice];
  const unsigned moi = unsigned(indice) + 1;
  const size_t tete = indexCase(serpent.getCoordX(), serpent.getCoordY());

  // Tête contre tête : une seule tête vivante possible sur la case
  const unsigned autre = cases[tete].tete;
  if (autre != AUCUNE_TETE and autre != moi) {
    Snake &adversaire = serpents[autre - 1];
    Snake &mort = serpent.combattreTete(adversaire);
    Snake &tueur = (&mort == &serpent) ? adversaire : serpent;

    tuer(mort, tueur);
    poserTete(tete, unsigned(&tueur - serpents.data()) + 1);

    if (&mort == &serpent) {
      return;
    }
  } else {
    poserTete(tete, moi);
  }
  enregistrerArrivee(indice);

  // Tête sur corps : le serpent est coupé au premier segment (depuis sa
  // tête) sur lequel se trouve la tête d'un autre serpent. Inutile de
  // parcourir le corps si aucune tête n'a pu arriver dessus.
  if (morsuresEnAttente[indice] == 0 and morsuresInconnues == 0) {
    return;
  }
  const auto &corps = serpent.getCoord();
  for (size_t k = 1; k < corps.size(); ++k) {
    const size_t c = indexCase(corps[k].x, corps[k].y);
    if (not tetePresente(c)) {
      continue;  // cas de loin le plus courant, sans lire la case
    }
    const unsigned attaquant = cases[c].tete;
    if (attaquant != moi) {
      serpent.etreMordu(k, serpents[attaquant - 1]);
      appliquerModifications(serpent);
      break;
    }
  }
}

void Combat::tuer(Snake &victime, const Snake &tueur) {
  cout << "Le serpent "s + to_string(tueur.getId()) + " a tuer le serpent "s
      + to_string(victime.getId()) + "\n"s;

  // Le corps et la pomme du mort disparaissent de l'écran
  oublierArrivee(size_t(&victime - serpents.data()));
  appliquerModifications(victime);
  Pomme &pomme = pommes[size_t(&victime - serpents.data())];
  if (pomme.estIntacte()) {
    pomme.pommeEstMangee();
    modifierCase(pomme.getCoordX(), pomme.getCoordY(), 0, -1);
  }
  --nbSerpent;
}

//------------------------- méthodes d'affichage ------------------------

bool Combat::faireCombattreSerpents(Affichage2d &affichage) {

  Uint32 dernierTitre = 0;

  while (nbSerpent > 1) {

    int accelerer = 0;
    if (afficher(affichage, accelerer)) {
      return false;  // fenêtre fermée ou ÉCHAP
    }
    if (accelerer != 0) {
      changerVitesse(accelerer);
      dernierTitre = 0;  // titre mis à jour tout de suite
    }

    const Uint32 debut = SDL_GetTicks();
    const unsigned long toursAvant = nbTours;
    if (vitesse == VITESSE_AUTO) {
      // Autant de tours que possible pendant la durée d'une image (au moins
      // ~60 images/s pour que la fenêtre reste fluide)
      const Uint32 budget = delai > 0 ? delai : 16;
      do {
        jouerTour();
        ++nbTours;
      } while (nbSerpent > 1 and SDL_GetTicks() - debut < budget);
    } else {
      for (unsigned t = 0; t < vitesse and nbSerpent > 1; ++t) {
        jouerTour();
        ++nbTours;
      }
    }
    toursDerniereImage = nbTours - toursAvant;

    if (SDL_GetTicks() - dernierTitre >= 250) {
      mettreAJourTitre(affichage);
      dernierTitre = SDL_GetTicks();
    }
  }

  int ignore = 0;
  return not afficher(affichage, ignore);  // état final
}

void Combat::changerVitesse(int pas) {

  // Paliers parcourus avec + / - ; le dernier est la vitesse automatique
  static const unsigned PALIERS[] = {1, 2, 5, 10, 20, 50, 100, 200, 500,
                                     1000, VITESSE_AUTO};
  const int nbPaliers = int(sizeof(PALIERS) / sizeof(PALIERS[0]));

  int palier = nbPaliers - 1;
  if (vitesse != VITESSE_AUTO) {
    palier = 0;
    while (palier < nbPaliers - 2 and PALIERS[palier] < vitesse) {
      ++palier;
    }
  }
  palier = std::max(0, std::min(nbPaliers - 1, palier + pas));
  vitesse = PALIERS[palier];
}

void Combat::mettreAJourTitre(Affichage2d &affichage) const {
  string titre = "Snake battle simulator - "s + to_string(nbSerpent)
      + " serpents - tour "s + to_string(nbTours) + " - vitesse "s;
  titre += vitesse == VITESSE_AUTO
      ? "auto (x"s + to_string(toursDerniereImage) + ")"s
      : "x"s + to_string(vitesse);
  affichage.definirTitre(titre + "  [+/-]"s);
}

bool Combat::afficher(Affichage2d &affichage, int &accelerer) {

  // Seules les cases modifiées depuis la dernière image sont redessinées
  // (une pomme reste dessinée par-dessus un serpent). Elles sont éparpillées
  // sur le terrain : on précharge quelques cases à l'avance pour que les
  // accès mémoire se recouvrent au lieu de s'attendre les uns les autres.
  const size_t AVANCE = 8;
  for (size_t k = 0; k < casesModifiees.size(); ++k) {
    if (k + AVANCE < casesModifiees.size()) {
      const CoordonneesXY &suivante = casesModifiees[k + AVANCE];
      precharger(&cases[indexCase(suivante.x, suivante.y)]);
      affichage.prechargerElement(suivante.x, suivante.y);
    }
    const CoordonneesXY &coord = casesModifiees[k];
    const size_t i = indexCase(coord.x, coord.y);
    Couleur couleur = Couleur::blanc;
    if (cases[i].pommes > 0) {
      couleur = Couleur::rouge;
    } else if (cases[i].serpents > 0) {
      couleur = Couleur::noir;
    }
    affichage.ajouterElementAffichage(coord.x, coord.y, couleur);
    caseMarquee[i] = false;
  }
  casesModifiees.clear();

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

  affichage.afficherEcranFin(logoGreenKatze(), panneau, cases);
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
