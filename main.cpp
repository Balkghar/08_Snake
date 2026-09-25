/*
  ---------------------------------------------------------------------------
  Fichier     : main.cpp
  Nom du labo : Labo8 - Snake
  Auteur(s)   : Delétraz Alexandre - Germano Hugo
  Date        : 10.01.2023
  But         : Point d'entrée du simulateur. Les paramètres peuvent être
                donnés en options de lancement ; ceux qui manquent sont
                demandés dans la console.

  Remarque(s) : Le style de format de code est importé de Google.
                Lancer avec --aide pour la liste des options.

  Compilateur : gcc version 11.2.0
  ---------------------------------------------------------------------------
*/

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "outils/aleatoire.hpp"
#include "outils/saisie.hpp"
#include "jeu/combatSnakes.hpp"
#include "outils/poolThreads.hpp"

using namespace std;

//-----------------------------------------------------------------------------
// Un paramètre du simulateur : ses noms d'option, ses bornes et, s'il n'a pas
// été donné au lancement, la question posée dans la console (vide si le
// paramètre a une valeur par défaut et n'est donc jamais demandé).
struct Parametre {
  string nomCourt;
  string nomLong;
  string description;
  int min;
  int max;
  int valeur;           // valeur par défaut, ou -1 si à demander
  string question;
  bool donne = false;   // fourni en option de lancement
};

void afficherAide(const string &programme, const vector<Parametre> &params) {
  cout << "Utilisation : " << programme << " [options]\n\n"
       << "Options (les valeurs manquantes sont demandees dans la console) :\n";
  for (const Parametre &p : params) {
    string noms = "  " + p.nomCourt + ", " + p.nomLong + " N";
    noms.resize(max<size_t>(noms.size() + 1, 26), ' ');
    cout << noms << p.description << " [" << p.min << "-" << p.max << "]";
    if (p.valeur >= 0) {
      cout << " (defaut : " << p.valeur << ")";
    }
    cout << '\n';
  }
  cout << "  -t, --turbo             Terrain maximal, un maximum de serpents,\n"
       << "                          vitesse maximale, silencieux (les autres\n"
       << "                          options restent prioritaires)\n"
       << "  -q, --silencieux        N'annonce pas chaque mort dans la console\n"
       << "  -f, --fin-auto          Quitte a la fin sans attendre (statistiques\n"
       << "                          dans la console seulement)\n"
       << "      --vsync             Images au rythme de l'ecran, sans\n"
       << "                          dechirure (ne ralentit pas le calcul)\n"
       << "      --profil            En fin de partie, temps passe dans le\n"
       << "                          calcul, les images, l'envoi a l'ecran\n"
       << "  -g, --graine N          Rejoue exactement la meme partie (avec les\n"
       << "                          memes -l -H -s, quel que soit -j) ; la\n"
       << "                          graine est affichee par --profil\n"
       << "  -h, --aide              Affiche cette aide\n\n"
       << "En jeu : + / - pour accelerer / ralentir, ECHAP pour quitter.\n\n"
       << "Exemples : " << programme << " -l 200 -H 150 -s 20\n"
       << "           " << programme << " -l 1200 -H 800 -s 20000 -v 0\n"
       << "           " << programme << " --turbo\n"
       << "           " << programme << " --turbo --graine 42 --profil -j 4\n";
}

// Options sans valeur
struct Drapeaux {
  bool turbo = false;
  bool silencieux = false;
  bool finAuto = false;
  bool vsync = false;
  bool profil = false;
  bool graineDonnee = false;
  std::uint64_t graine = 0;
};

// Lit un entier compris dans [min, max] ; renvoie false si invalide.
bool lireEntier(const string &texte, int min, int max, int &resultat) {
  size_t fin = 0;
  try {
    resultat = stoi(texte, &fin);
  } catch (const exception &) {
    return false;
  }
  return fin == texte.size() and resultat >= min and resultat <= max;
}

// Remplit params depuis argv. Renvoie false (après un message) si la ligne de
// commande est invalide.
bool lireArguments(int argc, char **argv, vector<Parametre> &params,
                   bool &aideDemandee, Drapeaux &drapeaux) {
  for (int i = 1; i < argc; ++i) {
    string arg = argv[i];

    if (arg == "-h" or arg == "--aide" or arg == "--help") {
      aideDemandee = true;
      return true;
    }
    if (arg == "-t" or arg == "--turbo") {
      drapeaux.turbo = true;
      continue;
    }
    if (arg == "-q" or arg == "--silencieux") {
      drapeaux.silencieux = true;
      continue;
    }
    if (arg == "-f" or arg == "--fin-auto") {
      drapeaux.finAuto = true;
      continue;
    }
    if (arg == "--vsync") {
      drapeaux.vsync = true;
      continue;
    }
    if (arg == "--profil") {
      drapeaux.profil = true;
      continue;
    }

    // Accepte "--largeur 200" comme "--largeur=200"
    string valeur;
    const size_t egal = arg.find('=');
    if (egal != string::npos) {
      valeur = arg.substr(egal + 1);
      arg = arg.substr(0, egal);
    }

    // La graine est un entier 64 bits, hors du cadre des autres paramètres
    if (arg == "-g" or arg == "--graine") {
      if (egal == string::npos) {
        if (i + 1 >= argc) {
          cerr << "Valeur manquante pour " << arg << '\n';
          return false;
        }
        valeur = argv[++i];
      }
      size_t fin = 0;
      try {
        drapeaux.graine = stoull(valeur, &fin);
      } catch (const exception &) {
        fin = 0;
      }
      if (valeur.empty() or valeur[0] == '-' or fin != valeur.size()) {
        cerr << "Valeur invalide pour " << arg << " : \"" << valeur
             << "\" (attendu un entier positif)\n";
        return false;
      }
      drapeaux.graineDonnee = true;
      continue;
    }

    Parametre *param = nullptr;
    for (Parametre &p : params) {
      if (arg == p.nomCourt or arg == p.nomLong) {
        param = &p;
      }
    }
    if (param == nullptr) {
      cerr << "Option inconnue : " << arg << " (voir --aide)\n";
      return false;
    }

    if (egal == string::npos) {
      if (i + 1 >= argc) {
        cerr << "Valeur manquante pour " << arg << '\n';
        return false;
      }
      valeur = argv[++i];
    }

    if (not lireEntier(valeur, param->min, param->max, param->valeur)) {
      cerr << "Valeur invalide pour " << arg << " : \"" << valeur
           << "\" (attendu un entier entre " << param->min << " et "
           << param->max << ")\n";
      return false;
    }
    param->donne = true;
  }
  return true;
}

//-----------------------------------------------------------------------------
int main(int argc, char **argv) {

  // cout garde son propre tampon au lieu d'écrire à chaque ligne : des
  // dizaines de milliers de morts annoncées ne ralentissent plus la partie
  ios::sync_with_stdio(false);

  //=========================== Paramètres ===================================
  const string MSG_TERRAIN = "Veuillez choisir la "s;
  const string MSG_INTERVALLE = "Elle doit etre comprise entre "s;

  // (les numéros de serpent sont rangés sur 17 bits dans chaque case)
  static_assert(100'000 <= Combat::MAX_SERPENTS, "trop de serpents");
  vector<Parametre> params = {
      {"-l", "--largeur", "Largeur du terrain (cases)", 50, 1200, -1,
       MSG_TERRAIN + "largeur du terrain de combat.\n" + MSG_INTERVALLE},
      {"-H", "--hauteur", "Hauteur du terrain (cases)", 50, 800, -1,
       MSG_TERRAIN + "hauteur du terrain de combat.\n" + MSG_INTERVALLE},
      {"-s", "--serpents", "Nombre de serpents", 2, 100'000, -1,
       "Choisisez combien de serpents devront combattre.\n"
       "Ce nombre doit etre compris entre "s},
      {"-d", "--delai", "Delai entre deux images (ms)", 0, 1000,
       int(Combat::DELAI_DEFAUT), ""},
      {"-z", "--zoom", "Taille d'une case (pixels)", 1, 16,
       int(Combat::ZOOM_DEFAUT), ""},
      {"-v", "--vitesse", "Tours par image (0 = auto)", 0, 1000,
       int(Combat::VITESSE_DEFAUT), ""},
      {"-j", "--threads", "Threads de calcul (0 = coeurs physiques - 1)", 0, 256,
       0, ""},
  };
  enum { LARGEUR, HAUTEUR, SERPENTS, DELAI, ZOOM, VITESSE, THREADS };

  // Chaque serpent et sa pomme doivent trouver leur place au départ : au plus
  // un serpent pour 4 cases
  auto maxSerpents = [&params]() {
    return int(min<long>(params[SERPENTS].max,
                         long(params[LARGEUR].valeur) * params[HAUTEUR].valeur / 4));
  };

  //------------------------------ Textes -----------------------------------------
  const string MSG_ERR = "Valeur invalide, veuillez recommencer"s;

  const string MSG_DEBUT = "Bienvenue dans Snake battle simulator de la "s
      "Green Katze Korporation !\n"s
          "Ici, vous pouvez faire combattre a mort des "s
              "serpents innocents de tous crimes, jusqu'a "s
                  "ce qu'il n'en reste plus qu'un !\n"s;
  const string MSG_FIN = "La Green Katze Korporation vous remercie "s
      "d'avoir utilise Snake battle simulator.\n"s
          "Grace a votre simulation, nous avons pu "s
              "recolter de précieuses informations pour "s
                  "l'avenir de l'humanite.\n"s;
  const string MSG_QUITTER = "Veuillez appuyer sur ENTER pour quitter."s;

  //======================== Début du programme ===================================

  bool aideDemandee = false;
  Drapeaux drapeaux;
  if (not lireArguments(argc, argv, params, aideDemandee, drapeaux)) {
    return EXIT_FAILURE;
  }
  if (aideDemandee) {
    afficherAide(argv[0], params);
    return EXIT_SUCCESS;
  }

  // Mode turbo : tout au maximum, sauf ce qui a été donné explicitement (le
  // nombre de serpents est fixé plus bas, une fois le terrain connu)
  if (drapeaux.turbo) {
    auto parDefaut = [&params](int indice, int valeur) {
      if (not params[size_t(indice)].donne) {
        params[size_t(indice)].valeur = valeur;
      }
    };
    parDefaut(LARGEUR, params[LARGEUR].max);
    parDefaut(HAUTEUR, params[HAUTEUR].max);
    parDefaut(DELAI, 0);
    parDefaut(ZOOM, 1);
    parDefaut(VITESSE, 0);
  }

  if (drapeaux.graineDonnee) {
    fixerGraine(drapeaux.graine);
  }

  cout << MSG_DEBUT << endl;

  // Les paramètres sans valeur (ni option, ni défaut) sont demandés, dans
  // l'ordre de la liste
  bool modeInteractif = false;
  for (Parametre &p : params) {
    if (&p == &params[SERPENTS] and params[LARGEUR].valeur > 0
        and params[HAUTEUR].valeur > 0) {
      p.max = maxSerpents();
      if (drapeaux.turbo and not p.donne) {
        p.valeur = p.max;
      }
      if (p.donne and p.valeur > p.max) {
        cerr << "Trop de serpents pour ce terrain : " << p.max
             << " au maximum" << endl;
        return EXIT_FAILURE;
      }
    }
    if (p.valeur < 0) {
      p.valeur = saisirIntervalle(p.min, p.max,
                                  p.question + to_string(p.min) + " et "s
                                      + to_string(p.max) + " : "s,
                                  MSG_ERR);
      modeInteractif = true;
    }
  }

  const unsigned coeurs = PoolThreads::coeursDisponibles();
  if (unsigned(params[THREADS].valeur) > coeurs) {
    cout << "Attention : " << params[THREADS].valeur << " threads pour "
         << coeurs << " coeurs, les threads en trop attendront endormis "
            "(plus lent qu'avec -j " << coeurs << ")." << endl;
  }

  Combat combat(unsigned(params[LARGEUR].valeur),
                unsigned(params[HAUTEUR].valeur),
                unsigned(params[SERPENTS].valeur),
                unsigned(params[THREADS].valeur));

  // Annoncer 100 000 morts dans un terminal peut coûter plus cher que la
  // simulation elle-même : le mode turbo est silencieux
  combat.choisirSorties(drapeaux.silencieux or drapeaux.turbo, drapeaux.finAuto);
  combat.activerVsync(drapeaux.vsync);
  combat.activerProfil(drapeaux.profil);
  combat.commencerCombat(unsigned(params[DELAI].valeur),
                         unsigned(params[ZOOM].valeur),
                         unsigned(params[VITESSE].valeur));
  cout << MSG_FIN;

  // Garde la console ouverte seulement si l'utilisateur est devant (lancement
  // par double-clic) ; en ligne de commande complète, on rend la main direct
  if (modeInteractif) {
    cout << MSG_QUITTER << endl;
    viderBuffer();
  }
  return EXIT_SUCCESS;
}
