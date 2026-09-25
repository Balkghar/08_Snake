/*
---------------------------------------------------------------------------
Fichier     : poolThreads.cpp
Nom du labo : Labo8 - Snake
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 10.01.2023
But         : Groupe de threads permanents avec barrière.

Remarque(s) :

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#include "poolThreads.hpp"

#include <algorithm>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#include <immintrin.h>
#endif

namespace {

// Attente active avant de céder le processeur puis de s'endormir. Assez
// longue pour enchaîner les tours sans jamais dormir, assez courte pour ne
// pas occuper un cœur entre deux images quand le jeu va lentement.
const unsigned ATTENTE_ACTIVE = 4000;
const unsigned ATTENTE_CEDEE = 200;

}  // namespace

PoolThreads::PoolThreads(unsigned nbThreads) : nbThreads(nbThreads) {
  if (this->nbThreads == 0) {
    this->nbThreads = std::max(1u, std::thread::hardware_concurrency());
  }
  for (unsigned i = 1; i < this->nbThreads; ++i) {
    ouvriers.emplace_back(&PoolThreads::boucleOuvrier, this, i);
  }
}

PoolThreads::~PoolThreads() {
  {
    std::lock_guard<std::mutex> verrou(mutex);
    arret = true;
    generationTache.fetch_add(1, std::memory_order_release);
  }
  reveil.notify_all();
  for (std::thread &t : ouvriers) {
    t.join();
  }
}

void PoolThreads::executer(const std::function<void(unsigned)> &tache) {
  if (nbThreads == 1) {
    tache(0);
    return;
  }
  this->tache = &tache;
  finis.store(0, std::memory_order_relaxed);
  {
    std::lock_guard<std::mutex> verrou(mutex);
    generationTache.fetch_add(1, std::memory_order_release);
  }
  reveil.notify_all();

  tache(0);

  while (finis.load(std::memory_order_acquire) != nbThreads - 1) {
    pause();
  }
}

void PoolThreads::barriere() {
  if (nbThreads == 1) {
    return;
  }
  const unsigned passage = generationBarriere.load(std::memory_order_acquire);
  if (arrivees.fetch_add(1, std::memory_order_acq_rel) + 1 == nbThreads) {
    // Dernier arrivé : remet le compteur à zéro et libère les autres
    arrivees.store(0, std::memory_order_relaxed);
    generationBarriere.fetch_add(1, std::memory_order_release);
    return;
  }
  unsigned tours = 0;
  while (generationBarriere.load(std::memory_order_acquire) == passage) {
    if (++tours < ATTENTE_ACTIVE) {
      pause();
    } else {
      std::this_thread::yield();  // (plus de threads que de cœurs)
    }
  }
}

void PoolThreads::boucleOuvrier(unsigned numero) {
  std::uint64_t vue = 0;
  for (;;) {
    // Attente d'une nouvelle tâche : active, puis polie, puis endormie
    unsigned tours = 0;
    while (generationTache.load(std::memory_order_acquire) == vue
        and tours < ATTENTE_ACTIVE + ATTENTE_CEDEE) {
      if (++tours < ATTENTE_ACTIVE) {
        pause();
      } else {
        std::this_thread::yield();
      }
    }
    if (generationTache.load(std::memory_order_acquire) == vue) {
      std::unique_lock<std::mutex> verrou(mutex);
      reveil.wait(verrou, [&] {
        return generationTache.load(std::memory_order_acquire) != vue;
      });
    }

    vue = generationTache.load(std::memory_order_acquire);
    {
      std::lock_guard<std::mutex> verrou(mutex);
      if (arret) {
        return;
      }
    }
    (*tache)(numero);
    finis.fetch_add(1, std::memory_order_release);
  }
}

void PoolThreads::pause() {
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
  _mm_pause();
#else
  std::this_thread::yield();
#endif
}
