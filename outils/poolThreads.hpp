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
              seule fois. Pour attendre (une tâche, la barrière, la fin des
              autres), un thread tourne d'abord en boucle active un temps
              limité, puis s'endort. S'il y a plus de threads que de cœurs,
              la boucle active est quasi supprimée : un thread qui tourne
              occuperait le cœur dont un autre a besoin pour avancer.

Compilateur : gcc version 11.2.0
---------------------------------------------------------------------------
*/

#ifndef LABO8_SNAKES_OUTILS_POOLTHREADS_HPP
#define LABO8_SNAKES_OUTILS_POOLTHREADS_HPP

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

// Taille d'une ligne de cache : deux données écrites par des threads
// différents doivent être sur des lignes différentes (sinon la ligne fait
// des allers-retours entre les cœurs). 64 octets sur x86 et la plupart des
// ARM, 128 sur les puces Apple (M1 et suivantes).
#if defined(__aarch64__) && defined(__APPLE__)
constexpr std::size_t TAILLE_LIGNE_CACHE = 128;
#else
constexpr std::size_t TAILLE_LIGNE_CACHE = 64;
#endif

class PoolThreads {
 public:
  /**
   * @param nbThreads nombre total de threads, appelant compris (0 = autant
   *                  que de cœurs disponibles)
   */
  explicit PoolThreads(unsigned nbThreads);
  ~PoolThreads();

  PoolThreads(const PoolThreads &) = delete;
  PoolThreads &operator=(const PoolThreads &) = delete;

  unsigned taille() const { return nbThreads; }
  bool estSurcharge() const { return surcharge; }

  /**
   * @brief Cœurs réellement utilisables par ce programme (sous Linux, tient
   *        compte des cœurs autorisés : conteneur, taskset...).
   */
  static unsigned coeursDisponibles();

  /**
   * @brief Cœurs physiques parmi les cœurs disponibles : deux threads
   *        matériels d'un même cœur (Hyper-Threading, SMT) comptent pour un.
   *        Sans information sur la topologie, égal à coeursDisponibles().
   */
  static unsigned coeursPhysiques();

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

  /**
   * @brief Indication au processeur qu'on attend en boucle active (libère
   *        des ressources pour l'autre thread du cœur, économise l'énergie).
   */
  static void pause();

 private:
  void boucleOuvrier(unsigned numero);

  // Attend que condition() devienne vraie : boucle active limitée dans le
  // temps, puis sommeil. Celui qui rend une condition vraie appelle
  // reveiller() ensuite.
  template<typename Condition>
  void attendre(Condition condition);
  void reveiller();

  unsigned nbThreads;
  bool surcharge;  // plus de threads que de cœurs
  std::vector<std::thread> ouvriers;
  const std::function<void(unsigned)> *tache = nullptr;
  std::atomic<bool> arret{false};

  // Chaque compteur sur sa propre ligne de cache : ils sont lus en boucle
  // par tous les threads
  alignas(TAILLE_LIGNE_CACHE) std::atomic<std::uint64_t> generationTache{0};
  alignas(TAILLE_LIGNE_CACHE) std::atomic<unsigned> finis{0};
  alignas(TAILLE_LIGNE_CACHE) std::atomic<unsigned> arrivees{0};
  alignas(TAILLE_LIGNE_CACHE) std::atomic<unsigned> generationBarriere{0};

  // Sommeil : les dormeurs attendent sur une variable de condition ; le
  // compteur évite de toucher au verrou quand personne ne dort
  alignas(TAILLE_LIGNE_CACHE) std::atomic<unsigned> dormeurs{0};
  std::mutex verrou;
  std::condition_variable reveil;
};

#endif
