cmake_minimum_required(VERSION 3.0.0)
project(08_snake VERSION 0.1.0)

include(CTest)

enable_testing()

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(08_snake main.cpp
                        jeu/pomme.cpp
                        jeu/snake.cpp
                        jeu/combatSnakes.cpp
                        outils/affichage2d.cpp
                        outils/aleatoire.cpp
                        outils/saisie.cpp)

target_link_libraries(08_snake ${SDL2_LIBRARIES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
