#!/bin/sh
# Compilation avec optimisation guidée par profil (GCC) :
#  1. version instrumentée, 2. parties d'entraînement (sans fenêtre),
#  3. recompilation qui utilise les mesures. Environ 10 % plus rapide.
# Usage : ./pgo.sh [dossier]   (par défaut build-pgo)
set -e
dossier=${1:-build-pgo}

cmake -S . -B "$dossier" -DSNAKE_PGO=GENERER
cmake --build "$dossier" --parallel
rm -rf "$dossier/pgo"

# Entraînement : une grosse partie parallèle et une partie sur un thread
SDL_VIDEODRIVER=dummy "$dossier/08_snake" --turbo -f > /dev/null
SDL_VIDEODRIVER=dummy "$dossier/08_snake" -l 400 -H 300 -s 3000 -v 0 -d 0 -f -j 1 > /dev/null

cmake -S . -B "$dossier" -DSNAKE_PGO=UTILISER
cmake --build "$dossier" --parallel
echo "Executable optimise : $dossier/08_snake"
