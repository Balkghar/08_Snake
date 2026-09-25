/*
---------------------------------------------------------------------------
Fichier     : poolThreads.cpp
Nom du labo : Labo8 - Snake
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 10.01.2023
But         : Groupe de threads permanents avec barrière.

Remarque(s) : Les attentes sont bornées en temps et non en nombre de tours
              de boucle : l'instruction de pause dure ~10 cycles sur les
              anciens Intel, ~140 depuis Skylake, autre chose sur AMD et ARM.

              Ordre mémoire : les compteurs sont lus et écrits en
              séquentiellement cohérent (seq_cst). C'est nécessaire pour
              l'endormissement (un thread incrémente « dormeurs » puis relit
              la condition, l'autre rend la condition vraie puis relit
              « dormeurs » : avec un ordre plus faible, sur ARM, chacun
              pourrait ne pas voir l'écriture de l'autre et le dormeur ne
              serait jamais réveillé). Sur x86 cela ne coûte rien de plus en
              lecture.

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#include "poolThreads.hpp"

#include <algorithm>
#include <chrono>

#if defined(_MSC_VER) && (defined(_M_ARM64) || defined(_M_ARM))
#include <intrin.h>
#elif defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#include <immintrin.h>
#endif

#if defined(__linux__)
#include <sched.h>
#endif

namespace {

using Horloge = std::chrono::steady_clock;

// Durées d'attente avant de s'endormir. Avec un cœur par thread, une
// boucle active assez longue pour enchaîner les tours sans jamais dormir
// (s'endormir puis être réveillé coûte des dizaines de microsecondes) ; en
// surcharge, presque rien : il vaut mieux laisser le cœur à un autre.
const std::chrono::microseconds ACTIVE_NORMAL(100);
const std::chrono::microseconds CEDEE_NORMAL(100);
const std::chrono::microseconds ACTIVE_SURCHARGE(1);
const std::chrono::microseconds CEDEE_SURCHARGE(20);

// L'horloge n'est lue que tous les N tours (une lecture coûte ~20 ns)
const unsigned TOURS_ENTRE_LECTURES = 32;

}  // namespace

unsigned PoolThreads::coeursDisponibles() {
#if defined(__linux__)
  // hardware_concurrency() compte tous les cœurs de la machine, même ceux
  // qu'un conteneur ou taskset nous interdit
  cpu_set_t ensemble;
  if (sched_getaffinity(0, sizeof(ensemble), &ensemble) == 0) {
    const int n = CPU_COUNT(&ensemble);
    if (n > 0) {
      return unsigned(n);
    }
  }
#endif
  return std::max(1u, std::thread::hardware_concurrency());
}

PoolThreads::PoolThreads(unsigned nbThreads) : nbThreads(nbThreads) {
  const unsigned coeurs = coeursDisponibles();
  if (this->nbThreads == 0) {
    this->nbThreads = coeurs;
  }
  surcharge = this->nbThreads > coeurs;
  for (unsigned i = 1; i < this->nbThreads; ++i) {
    ouvriers.emplace_back(&PoolThreads::boucleOuvrier, this, i);
  }
}

PoolThreads::~PoolThreads() {
  arret.store(true);
  generationTache.fetch_add(1);
  reveiller();
  for (std::thread &t : ouvriers) {
    t.join();
  }
}

void PoolThreads::pause() {
#if defined(_MSC_VER) && (defined(_M_ARM64) || defined(_M_ARM))
  __isb(_ARM64_BARRIER_SY);
#elif defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
  _mm_pause();
#elif defined(__aarch64__) || defined(__arm__)
  // « isb » plutôt que « yield », qui ne fait rien sur la plupart des ARM
  // récents (même choix que la bibliothèque standard de Rust)
  __asm__ __volatile__("isb" ::: "memory");
#else
  std::this_thread::yield();
#endif
}

template<typename Condition>
void PoolThreads::attendre(Condition condition) {
  if (condition()) {
    return;
  }

  // 1. Boucle active, puis 2. on cède le cœur à d'autres threads...
  const auto active = surcharge ? ACTIVE_SURCHARGE : ACTIVE_NORMAL;
  const auto cedee = surcharge ? CEDEE_SURCHARGE : CEDEE_NORMAL;
  const auto debut = Horloge::now();
  for (unsigned tours = 1;; ++tours) {
    if (condition()) {
      return;
    }
    if (tours % TOURS_ENTRE_LECTURES == 0) {
      const auto ecoule = Horloge::now() - debut;
      if (ecoule >= active + cedee) {
        break;
      }
      if (ecoule >= active) {
        std::this_thread::yield();
        continue;
      }
    }
    pause();
  }

  // 3. ... puis on s'endort. Le verrou est tenu entre la vérification de la
  // condition et la mise en sommeil : un réveil ne peut pas être perdu.
  std::unique_lock<std::mutex> garde(verrou);
  dormeurs.fetch_add(1);
  while (not condition()) {
    reveil.wait(garde);
  }
  dormeurs.fetch_sub(1);
}

void PoolThreads::reveiller() {
  if (dormeurs.load() > 0) {
    { std::lock_guard<std::mutex> garde(verrou); }
    reveil.notify_all();
  }
}

void PoolThreads::executer(const std::function<void(unsigned)> &tache) {
  if (nbThreads == 1) {
    tache(0);
    return;
  }
  this->tache = &tache;
  finis.store(0);
  generationTache.fetch_add(1);
  reveiller();

  tache(0);

  const unsigned attendus = nbThreads - 1;
  attendre([&] { return finis.load() == attendus; });
}

void PoolThreads::barriere() {
  if (nbThreads == 1) {
    return;
  }
  const unsigned passage = generationBarriere.load();
  if (arrivees.fetch_add(1) + 1 == nbThreads) {
    // Dernier arrivé : remet le compteur à zéro et libère les autres
    arrivees.store(0);
    generationBarriere.fetch_add(1);
    reveiller();
    return;
  }
  attendre([&] { return generationBarriere.load() != passage; });
}

void PoolThreads::boucleOuvrier(unsigned numero) {
  std::uint64_t vue = 0;
  for (;;) {
    attendre([&] { return generationTache.load() != vue; });
    vue = generationTache.load();
    if (arret.load()) {
      return;
    }
    (*tache)(numero);
    finis.fetch_add(1);
    reveiller();
  }
}
