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
   * @brief Permet de se déplacer vers une coordonnées.
   * @param x
   * @param y
   */
  void deplacerVersXY(int x, int y);
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
  /**
   * @brief Tête contre tête : le plus court meurt, le vainqueur gagne 60 % de
   *        sa longueur.
   * @return le serpent tué
   */
  Snake &combattreTete(Snake &autre);

  /**
   * @brief La tête d'attaquant est sur le segment `position` du corps : le
   *        serpent est coupé après ce segment et l'attaquant gagne 40 % de
   *        la longueur coupée.
   */
  void etreMordu(std::size_t position, Snake &attaquant);

 private:

  //------------------------- Agrandissement ------------------------------
  unsigned calculAjoutLongueur(std::size_t longu, unsigned pourcentage);

  /**
   * @brief Fait mourir ce serpent ; le vainqueur gagne 60 % de sa longueur.
   */
  void mourir(Snake &vainqueur);

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