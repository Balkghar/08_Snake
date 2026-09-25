cmake_minimum_required(VERSION 3.13)
project(08_snake VERSION 0.1.0)

include(CTest)

enable_testing()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(SDL2 REQUIRED)
find_package(Threads REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(08_snake main.cpp
                        jeu/pomme.cpp
                        jeu/snake.cpp
                        jeu/combatSnakes.cpp
                        outils/affichage2d.cpp
                        outils/aleatoire.cpp
                        outils/police.cpp
                        outils/poolThreads.cpp
                        outils/saisie.cpp)

target_link_libraries(08_snake ${SDL2_LIBRARIES} Threads::Threads)

# Options d'optimisation supplémentaires (GCC)
#  -DSNAKE_NATIVE=ON : instructions du processeur de la machine qui compile
#                      (le programme peut ne pas marcher sur un autre)
#  -DSNAKE_PGO=GENERER puis UTILISER : optimisation guidée par un profil
#                      d'exécution (voir pgo.sh, qui enchaîne tout)
option(SNAKE_NATIVE "Compiler pour le processeur de cette machine" OFF)
set(SNAKE_PGO "" CACHE STRING "Optimisation guidee par profil : vide, GENERER ou UTILISER")
set(SNAKE_PGO_DOSSIER "${CMAKE_BINARY_DIR}/pgo" CACHE PATH "Dossier des profils")
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(SNAKE_NATIVE)
    target_compile_options(08_snake PRIVATE -march=native)
  endif()
  if(SNAKE_PGO STREQUAL "GENERER")
    target_compile_options(08_snake PRIVATE -fprofile-generate
                           -fprofile-update=atomic -fprofile-dir=${SNAKE_PGO_DOSSIER})
    target_link_options(08_snake PRIVATE -fprofile-generate)
  elseif(SNAKE_PGO STREQUAL "UTILISER")
    target_compile_options(08_snake PRIVATE -fprofile-use -fprofile-partial-training
                           -fprofile-dir=${SNAKE_PGO_DOSSIER})
  endif()
elseif(SNAKE_NATIVE OR SNAKE_PGO)
  message(WARNING "SNAKE_NATIVE et SNAKE_PGO ne sont prévus que pour GCC")
endif()

# Optimisation à l'édition de liens (LTO) : permet d'intégrer (inline) les
# petits accesseurs définis dans d'autres fichiers .cpp, appelés des millions
# de fois par partie
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_disponible OUTPUT ipo_message)
if(ipo_disponible)
  set_property(TARGET 08_snake PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
