/*
---------------------------------------------------------------------------
Fichier     : poolThreads.hpp
Nom du labo : Labo8 - Snake
Auteur(s)   : Delétraz Alexandre - Germano Hugo
Date        : 10.01.2023
But         : Groupe de threads permanents qui exécutent tous la même tâche,
              avec une barrière pour découper la tâche en phases.

Remarque(s) : Conçu pour des tâches très courtes et très fréquentes (un tour
              de jeu dure quelques microsecondes) : les threads sont créés une
              seule fois, attendent en boucle active un court instant puis
              s'endorment, et la barrière est une simple boucle active sur un
              compteur atomique, bien plus rapide qu'un mutex.

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#ifndef LABO8_SNAKES_OUTILS_POOLTHREADS_HPP
#define LABO8_SNAKES_OUTILS_POOLTHREADS_HPP

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

class PoolThreads {
 public:
  /**
   * @param nbThreads nombre total de threads, appelant compris (0 = autant
   *                  que de cœurs)
   */
  explicit PoolThreads(unsigned nbThreads);
  ~PoolThreads();

  PoolThreads(const PoolThreads &) = delete;
  PoolThreads &operator=(const PoolThreads &) = delete;

  unsigned taille() const { return nbThreads; }

  /**
   * @brief Exécute tache(i) sur chaque thread i (le thread appelant est le
   *        numéro 0) et attend que tous aient fini.
   */
  void executer(const std::function<void(unsigned)> &tache);

  /**
   * @brief À appeler par tous les threads, depuis la tâche : aucun ne
   *        continue avant que tous soient arrivés.
   */
  void barriere();

 private:
  void boucleOuvrier(unsigned numero);
  static void pause();

  unsigned nbThreads;
  std::vector<std::thread> ouvriers;
  const std::function<void(unsigned)> *tache = nullptr;

  // Distribution des tâches : les ouvriers attendent un changement de
  // generationTache, d'abord en boucle active puis endormis
  std::mutex mutex;
  std::condition_variable reveil;
  std::atomic<std::uint64_t> generationTache{0};
  std::atomic<unsigned> finis{0};
  bool arret = false;

  // Barrière : compteur d'arrivées et numéro de passage, chacun sur sa
  // propre ligne de cache pour que les threads ne se gênent pas
  alignas(64) std::atomic<unsigned> arrivees{0};
  alignas(64) std::atomic<unsigned> generationBarriere{0};
};

#endif
