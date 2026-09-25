/*
---------------------------------------------------------------------------
Fichier     : allocateurGrandesPages.hpp
Nom du labo : Labo8 - Snake
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 10.01.2023
But         : Allocateur pour std::vector qui demande au système des pages
              mémoire de 2 Mo au lieu de 4 Ko (Linux, « transparent huge
              pages »).

Remarque(s) : Pour un grand tableau lu à des endroits aléatoires, chaque
              accès doit aussi traduire l'adresse (TLB) ; avec des pages de
              2 Mo, 512 fois moins d'entrées sont nécessaires. Ailleurs que
              sous Linux, allocation ordinaire.

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#ifndef LABO8_SNAKES_OUTILS_ALLOCATEURGRANDESPAGES_HPP
#define LABO8_SNAKES_OUTILS_ALLOCATEURGRANDESPAGES_HPP

#include <cstddef>
#include <cstdlib>
#include <new>

#if defined(__linux__)
#include <sys/mman.h>
#endif

template<typename T>
class AllocateurGrandesPages {
 public:
  using value_type = T;

  AllocateurGrandesPages() = default;
  template<typename U>
  AllocateurGrandesPages(const AllocateurGrandesPages<U> &) {}

  T *allocate(std::size_t n) {
    const std::size_t octets = n * sizeof(T);
#if defined(__linux__)
    if (octets >= GRANDE_PAGE) {
      // Alignée sur 2 Mo et arrondie à un multiple de 2 Mo, puis signalée
      // au noyau avant le premier accès
      const std::size_t arrondi = (octets + GRANDE_PAGE - 1) / GRANDE_PAGE * GRANDE_PAGE;
      if (void *p = std::aligned_alloc(GRANDE_PAGE, arrondi)) {
        madvise(p, arrondi, MADV_HUGEPAGE);
        return static_cast<T *>(p);
      }
      throw std::bad_alloc();
    }
#endif
    if (void *p = std::malloc(octets == 0 ? 1 : octets)) {
      return static_cast<T *>(p);
    }
    throw std::bad_alloc();
  }

  void deallocate(T *p, std::size_t) { std::free(p); }

  template<typename U>
  bool operator==(const AllocateurGrandesPages<U> &) const { return true; }
  template<typename U>
  bool operator!=(const AllocateurGrandesPages<U> &) const { return false; }

 private:
  static constexpr std::size_t GRANDE_PAGE = std::size_t(2) << 20;
};

#endif
