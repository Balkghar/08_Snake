/*
---------------------------------------------------------------------------
Fichier     : fileCirculaire.hpp
Nom du labo : Labo8 - Snake
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 10.01.2023
But         : File circulaire (tampon en anneau) : ajout en tête et retrait en
              queue en temps constant, accès indexé, mémoire contiguë.

Remarque(s) : Plus économe qu'une std::deque pour de petites files : la
              capacité part de 16 éléments et double au besoin, alors qu'une
              deque réserve d'emblée un bloc de 512 octets et sa table.

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#ifndef LABO8_SNAKES_OUTILS_FILECIRCULAIRE_HPP
#define LABO8_SNAKES_OUTILS_FILECIRCULAIRE_HPP

#include <cstddef>
#include <vector>

template<typename T>
class FileCirculaire {
 public:
  std::size_t size() const { return taille; }
  bool empty() const { return taille == 0; }

  // Élément k depuis la tête (0 = tête)
  const T &operator[](std::size_t k) const {
    return anneau[(debut + k) & (anneau.size() - 1)];
  }
  const T &front() const { return (*this)[0]; }
  const T &back() const { return (*this)[taille - 1]; }

  void push_front(const T &valeur) {
    if (taille == anneau.size()) {
      agrandir();
    }
    debut = (debut + anneau.size() - 1) & (anneau.size() - 1);
    anneau[debut] = valeur;
    ++taille;
  }

  void pop_back() { --taille; }

  // Ne garde que les n premiers éléments (n <= size())
  void tronquer(std::size_t n) { taille = n; }

  // Vide la file et rend sa mémoire
  void liberer() {
    std::vector<T>().swap(anneau);
    debut = taille = 0;
  }

 private:
  // La capacité reste une puissance de 2 : le modulo devient un simple masque
  void agrandir() {
    std::vector<T> nouveau(anneau.empty() ? CAPACITE_INITIALE : anneau.size() * 2);
    for (std::size_t k = 0; k < taille; ++k) {
      nouveau[k] = (*this)[k];
    }
    anneau.swap(nouveau);
    debut = 0;
  }

  static constexpr std::size_t CAPACITE_INITIALE = 16;

  std::vector<T> anneau;
  std::size_t debut = 0;
  std::size_t taille = 0;
};

#endif
