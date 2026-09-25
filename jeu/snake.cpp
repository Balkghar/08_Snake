/*
  ---------------------------------------------------------------------------
  Fichier     : snake.cpp
  Nom du labo : Labo8 - Snake
  Auteur(s)   : Delétraz Alexandre - Germano Hugo
  Date        : 10.01.2023
  But         : Définition des fonctions membres de la classe snake.

  Remarque(s) : 

  Compilateur : gcc version 11.2.0
  ---------------------------------------------------------------------------
*/
#include "snake.hpp"
#include <vector>
#include <cstdlib>

using namespace std;


//=========================== Partie public ===============================

//--------------------------- Constructeur --------------------------------
Snake::Snake(int x,
             int y,
             const unsigned id,
             bool estEnVie,
             unsigned longueur
) : id(id), estEnVie(estEnVie) {

  coordonnees.push_front({x, y});
  casesAjoutees.push_back(coordonnees.front());

  longueurAAjouter = longueur - 1;

}

//------------------------- Déplacements --------------------------------
void Snake::deplacerVersXY(int x, int y, std::uint64_t hasard) {

  const int diffX = abs(x - getCoordX());
  const int diffY = abs(y - getCoordY());
  if (diffX == 0 and diffY == 0) {
    return;
  }

  // L'axe du pas est tiré au hasard, pondéré par la distance restante sur
  // chaque axe : le serpent suit à peu près la droite vers sa pomme, et
  // chaque pas le rapproche (chemin toujours le plus court). Prendre
  // systématiquement l'axe le plus long faisait partir tous les serpents à
  // l'horizontale en même temps : deux grandes vagues en début de partie.
  // (tirage dans [0, diffX + diffY[ par multiplication, sans division)
  const std::uint64_t tirage =
      ((hasard & 0xFFFFFFFFu) * std::uint64_t(diffX + diffY)) >> 32;
  if (tirage < std::uint64_t(diffX)) {
    deplacerVers(x > getCoordX() ? Direction::droite : Direction::gauche);
  } else {
    deplacerVers(y > getCoordY() ? Direction::bas : Direction::haut);
  }
}

void Snake::deplacerVers(Direction dir) {

  // La tête avance d'une case ; sans agrandissement la queue suit. Avec une
  // file circulaire, seules les deux extrémités bougent, quelle que soit la
  // longueur.
  CoordonneesXY tete = coordonnees.front();

  switch (dir) {
    case Direction::haut :--tete.y;
      break;
    case Direction::bas :++tete.y;
      break;
    case Direction::droite :++tete.x;
      break;
    case Direction::gauche :--tete.x;
      break;
  }

  coordonnees.push_front(tete);
  casesAjoutees.push_back(tete);

  if (longueurAAjouter) {
    longueurAAjouter -= 1;
    if (coordonnees.size() > stats.longueurMax) {
      stats.longueurMax = coordonnees.size();
    }
  } else {
    casesRetirees.push_back(coordonnees.back());
    coordonnees.pop_back();
  }
}

//--------------------------- getter et setter ----------------------------
int Snake::getCoordX() const {
  return coordonnees.front().x;
}

int Snake::getCoordY() const {
  return coordonnees.front().y;
}

unsigned Snake::getId() const {
  return id;
}

bool Snake::getEstEnVie() const {
  return estEnVie;
}

const FileCirculaire<CoordonneesXY> &Snake::getCoord() const {
  return coordonnees;
}

std::size_t Snake::getTaille() const {
  return coordonnees.size();
}

const StatsSerpent &Snake::getStats() const {
  return stats;
}

//------------------------- suivi des modifications ---------------------
const std::vector<CoordonneesXY> &Snake::getCasesAjoutees() const {
  return casesAjoutees;
}

const std::vector<CoordonneesXY> &Snake::getCasesRetirees() const {
  return casesRetirees;
}

void Snake::oublierModifications() {
  if (estEnVie) {
    casesAjoutees.clear();  // la capacité est gardée pour le tour suivant
    casesRetirees.clear();
  } else {
    std::vector<CoordonneesXY>().swap(casesAjoutees);
    std::vector<CoordonneesXY>().swap(casesRetirees);
  }
}

//------------------------- autres --------------------------------------
void Snake::longueurAAjouterSupl(unsigned valeur) {

  longueurAAjouter += valeur;
}

void Snake::mangerPomme(unsigned valeur) {
  ++stats.pommes;
  longueurAAjouterSupl(valeur);
}

//------------------------- Combat --------------------------------------
std::size_t Snake::etreMordu(std::size_t position) {
  const size_t garde = position + 1;  // le segment mordu reste
  if (garde >= coordonnees.size()) {
    return 0;
  }
  const size_t coupes = coordonnees.size() - garde;
  ++stats.morsuresSubies;
  longueurAAjouter = 0;
  for (size_t k = garde; k < coordonnees.size(); ++k) {
    casesRetirees.push_back(coordonnees[k]);
  }
  coordonnees.tronquer(garde);
  return coupes;
}

void Snake::recompenserMorsure(std::size_t longueurCoupee) {
  longueurAAjouterSupl(calculAjoutLongueur(longueurCoupee, 40));
  ++stats.morsuresInfligees;
}

void Snake::recompenserVictoire(std::size_t longueurVaincu) {
  longueurAAjouterSupl(calculAjoutLongueur(longueurVaincu, 60));
  ++stats.victimes;
}

//=========================== Partie privée ===============================

//------------------------- Agrandissement ------------------------------
unsigned Snake::calculAjoutLongueur(std::size_t longu, unsigned pourcentage) {

  // (la division entière avant la multiplication donnait 0 sous 100 cases)
  return unsigned(longu * pourcentage / 100);
}

//------------------------- Méthodes de combat --------------------------
std::size_t Snake::mourir() {

  estEnVie = false;
  const size_t longueur = coordonnees.size();
  // un serpent mort n'est plus affiché : tout son corps est libéré
  for (size_t k = 0; k < coordonnees.size(); ++k) {
    casesRetirees.push_back(coordonnees[k]);
  }

  // Seule la position de la tête reste utile (getCoordX/Y) : le reste du
  // corps est rendu à la mémoire
  const CoordonneesXY tete = coordonnees.front();
  coordonnees.liberer();
  coordonnees.push_front(tete);
  return longueur;
}
