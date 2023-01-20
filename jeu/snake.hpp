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

class Snake {
public:
   //------------------------- Constructeur --------------------------------
   Snake(CoordonneesXY coord,
         const unsigned id,
         bool estEnVie,
         unsigned longueur
   );

   //------------------------- Déplacements --------------------------------
   /**
    * @brief Permet de se déplacer vers une coordonnées.
    * @param coord
    * @param y
    */
   void deplacerVersXY(CoordonneesXY coord);

   //------------------------- getter et setter ----------------------------
   int getCoordX() const;

   int getCoordY() const;

   unsigned getId() const;

   bool getEstEnVie() const;

   std::vector<CoordonneesXY> getCoord() const;

   //------------------------- autres --------------------------------------
   void longueurAAjouterSupl(unsigned valeur);

   bool combattreSerpent(Snake &serpent);


private:

   enum class Direction {
      haut, bas, droite, gauche
   };

   static const Direction haut = Direction::haut;
   static const Direction bas = Direction::bas;
   static const Direction droite = Direction::droite;
   static const Direction gauche = Direction::gauche;

   /**
    * @brief Permet de se déplacer dans une certaine direction (Haut, bas droite, gauche)
    * @param dir
    */
   void deplacerVers(Direction dir);


   //------------------------- Agrandissement ------------------------------
   void agrandirSerpent(CoordonneesXY &coord);

   //------------------------- ami ------------------------------
   friend unsigned calculAjoutLongueur(std::size_t longu, unsigned pourcentage);

   //------------------------- Méthodes de combat --------------------------
   void couperSerpent(Snake &serpent);

   /**
    * @brief fait mourir le serpent envoyé en paramètre.
    * @param serpent
    */
   void mourir(Snake &serpent);

   //------------------------- Données -------------------------------------
   const unsigned id;
   unsigned longueurAAjouter;
   bool estEnVie;
   std::vector<CoordonneesXY> coordonnees;

};

#endif