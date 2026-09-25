cmake_minimum_required(VERSION 3.10)
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
