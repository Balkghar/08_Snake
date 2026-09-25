/*
  ---------------------------------------------------------------------------
  Fichier     : snake.hpp
  Nom du labo : Labo8 - Snake
  Auteur(s)   : Delétraz Alexandre - Germano Hugo
  Date        : 10.01.2023
  But         : Déclaration de la classe snake et  des fonctions membres de la
                classe snake.

  Remarque(s) :

  Compilateur : gcc version 11.2.0
  ---------------------------------------------------------------------------
*/

#ifndef LABO8_SNAKES_JEU_SNAKE_HPP
#define LABO8_SNAKES_JEU_SNAKE_HPP

#include <cstdint>
#include <vector>
#include <string>
#include "../outils/struct_coordonnees.hpp"
#include "../outils/fileCirculaire.hpp"

enum Direction { haut, bas, droite, gauche };

// Statistiques d'un serpent sur toute la partie
struct StatsSerpent {
  unsigned victimes = 0;           // serpents tués tête contre tête
  unsigned pommes = 0;             // pommes mangées
  unsigned morsuresInfligees = 0;  // serpents coupés
  unsigned morsuresSubies = 0;     // fois où il a été coupé
  std::size_t longueurMax = 1;     // plus grande longueur atteinte
};

class Snake {
 public:
  //------------------------- Constructeur --------------------------------
  Snake(int x,
        int y,
        const unsigned id,
        bool estEnVie,
        unsigned longueur
  );

  //------------------------- Déplacements --------------------------------
  /**
   * @brief Avance d'une case vers (x, y) par un plus court chemin. L'axe du
   *        pas est choisi au hasard, pondéré par la distance restante sur
   *        chaque axe.
   * @param hasard nombre aléatoire fourni par l'appelant (chaque serpent a
   *        sa propre suite de nombres : le résultat ne dépend ni de l'ordre
   *        des serpents ni du nombre de threads)
   */
  void deplacerVersXY(int x, int y, std::uint64_t hasard);
  /**
   * @brief Permet de se déplacer dans une certaine direction (Haut, bas droite, gauche)
   * @param dir
   */
  void deplacerVers(Direction dir);

  //------------------------- getter et setter ----------------------------
  int getCoordX() const;
  int getCoordY() const;
  unsigned getId() const;
  bool getEstEnVie() const;
  /**
   * @brief Corps du serpent, la tête en premier.
   */
  const FileCirculaire<CoordonneesXY> &getCoord() const;
  std::size_t getTaille() const;

  //------------------------- suivi des modifications ---------------------
  /**
   * @brief Cases occupées / libérées par le serpent depuis le dernier appel à
   *        oublierModifications() (déplacement, coupure, mort). Permet de ne
   *        redessiner que ce qui a changé.
   */
  const std::vector<CoordonneesXY> &getCasesAjoutees() const;
  const std::vector<CoordonneesXY> &getCasesRetirees() const;
  void oublierModifications();  // vide les listes (et leur mémoire si mort)

  const StatsSerpent &getStats() const;

  //------------------------- autres --------------------------------------
  void longueurAAjouterSupl(unsigned valeur);
  void mangerPomme(unsigned valeur);

  //------------------------- Combat --------------------------------------
  // Chaque événement est découpé en deux : ce qui arrive au serpent lui-même
  // et la récompense de l'autre, appliquée plus tard. Plusieurs threads
  // peuvent ainsi traiter des serpents différents sans jamais écrire dans
  // le même objet.

  /**
   * @brief Fait mourir ce serpent (perdant d'un tête contre tête).
   * @return sa longueur au moment de la mort
   */
  std::size_t mourir();
  /**
   * @brief Le vainqueur gagne 60 % de la longueur du vaincu.
   */
  void recompenserVictoire(std::size_t longueurVaincu);

  /**
   * @brief Une tête est sur le segment `position` du corps : le serpent est
   *        coupé après ce segment (le segment mordu reste).
   * @return le nombre de segments coupés (0 si rien à couper)
   */
  std::size_t etreMordu(std::size_t position);
  /**
   * @brief L'attaquant gagne 40 % de la longueur coupée.
   */
  void recompenserMorsure(std::size_t longueurCoupee);

 private:

  //------------------------- Agrandissement ------------------------------
  unsigned calculAjoutLongueur(std::size_t longu, unsigned pourcentage);

  //------------------------- Données -------------------------------------
  const unsigned id;
  unsigned longueurAAjouter;
  bool estEnVie;
  FileCirculaire<CoordonneesXY> coordonnees;
  StatsSerpent stats;
  std::vector<CoordonneesXY> casesAjoutees;
  std::vector<CoordonneesXY> casesRetirees;
};

#endif