/*
---------------------------------------------------------------------------
Fichier     : fileCirculaire.hpp
Nom du labo : Labo8 - Snake
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 10.01.2023
But         : File circulaire (tampon en anneau) : ajout en tête et retrait en
              queue en temps constant, accès indexé, mémoire contiguë.

Remarque(s) : Plus économe qu'une std::deque pour de petites files : les
              16 premiers éléments sont dans l'objet, puis la capacité
              double au besoin, alors qu'une deque réserve d'emblée un bloc
              de 512 octets et sa table.

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#ifndef LABO8_SNAKES_OUTILS_FILECIRCULAIRE_HPP
#define LABO8_SNAKES_OUTILS_FILECIRCULAIRE_HPP

#include <cstddef>
#include <vector>

template<typename T, std::size_t N = 16>
class FileCirculaire {
  static_assert(N > 0 and (N & (N - 1)) == 0, "N doit etre une puissance de 2");

 public:
  std::size_t size() const { return taille; }
  bool empty() const { return taille == 0; }

  // Élément k depuis la tête (0 = tête)
  const T &operator[](std::size_t k) const {
    return donnees()[(debut + k) & masque];
  }
  const T &front() const { return (*this)[0]; }
  const T &back() const { return (*this)[taille - 1]; }

  void push_front(const T &valeur) {
    if (taille == masque + 1) {
      agrandir();
    }
    debut = (debut + masque) & masque;
    donnees()[debut] = valeur;
    ++taille;
  }

  void pop_back() { --taille; }

  // Ne garde que les n premiers éléments (n <= size())
  void tronquer(std::size_t n) { taille = n; }

  // Vide la file et rend sa mémoire
  void liberer() {
    std::vector<T>().swap(externe);
    masque = N - 1;
    debut = taille = 0;
  }

 private:
  // Les N premiers éléments sont rangés dans l'objet lui-même (pas d'accès
  // à une zone mémoire lointaine pour les petites files, les plus nombreuses)
  // ; au-delà, un tableau alloué dont la taille double à chaque fois.
  const T *donnees() const { return externe.empty() ? interne : externe.data(); }
  T *donnees() { return externe.empty() ? interne : externe.data(); }

  // La capacité reste une puissance de 2 : le modulo devient un simple masque
  void agrandir() {
    std::vector<T> nouveau((masque + 1) * 2);
    for (std::size_t k = 0; k < taille; ++k) {
      nouveau[k] = (*this)[k];
    }
    externe.swap(nouveau);
    masque = externe.size() - 1;
    debut = 0;
  }

  T interne[N] = {};
  std::vector<T> externe;
  std::size_t masque = N - 1;
  std::size_t debut = 0;
  std::size_t taille = 0;
};

#endif
