#!/bin/sh
# Compilation avec optimisation guidée par profil (PGO, GCC) :
#  1. version instrumentée, 2. parties d'entraînement (sans fenêtre),
#  3. recompilation qui utilise les mesures, 4. comparaison avec une
#  compilation Release ordinaire sur la même partie.
#
# Usage : ./pgo.sh [--natif] [dossier]   (dossier par défaut : build-pgo)
#   --natif : instructions du processeur de cette machine (-march=native) ;
#             l'exécutable peut ne pas fonctionner sur un autre ordinateur.
set -e

natif=OFF
if [ "$1" = "--natif" ]; then
  natif=ON
  shift
fi
dossier=${1:-build-pgo}
reference="$dossier-ref"

# PGO n'est prévu que pour GCC (les options de Clang sont différentes)
compilateur=${CXX:-c++}
if ! "$compilateur" --version 2>/dev/null | grep -qiE "gcc|g\+\+|free software foundation"; then
  echo "pgo.sh demande GCC (compilateur trouve : $compilateur)." >&2
  echo "Essayer : CXX=g++ ./pgo.sh" >&2
  exit 1
fi

# Toujours en Release : un dossier déjà configuré en Debug le resterait,
# et serait 5 à 10 fois plus lent
options="-DCMAKE_BUILD_TYPE=Release -DSNAKE_NATIVE=$natif"

echo "== 1/4 Version instrumentee"
cmake -S . -B "$dossier" $options -DSNAKE_PGO=GENERER > /dev/null
cmake --build "$dossier" --parallel
rm -rf "$dossier/pgo"

# Entraînement, graines fixes (même exécutable à chaque fois) : une grosse
# partie parallèle (mêlée à 100 000 serpents), une partie moyenne sur un
# thread (fin de partie à quelques serpents, la moitié du temps d'une
# partie turbo) et une petite avec les images (préparation, pixels).
echo "== 2/4 Entrainement"
export SDL_VIDEODRIVER=dummy
"$dossier/08_snake" --turbo -l 1200 -H 800 -s 100000 --graine 1 -f > /dev/null
"$dossier/08_snake" -l 400 -H 300 -s 3000 -v 0 -d 0 -z 1 -f -q -j 1 --graine 2 > /dev/null
"$dossier/08_snake" -l 200 -H 150 -s 200 -v 20 -d 0 -z 2 -f -q --graine 3 > /dev/null

echo "== 3/4 Recompilation avec le profil"
cmake -S . -B "$dossier" $options -DSNAKE_PGO=UTILISER > /dev/null
cmake --build "$dossier" --parallel

echo "== 4/4 Comparaison avec une compilation Release ordinaire"
cmake -S . -B "$reference" $options -DSNAKE_PGO= > /dev/null
cmake --build "$reference" --parallel > /dev/null
# Graine différente de l'entraînement : on mesure une partie jamais vue
calcul() {
  "$1" --turbo -l 1200 -H 800 -s 100000 --graine 42 -f --profil 2>/dev/null \
    | sed -n 's/.*Moteur : calcul \([0-9]*\) ms.*/\1/p'
}
meilleur() {
  m=""
  for essai in 1 2 3; do
    t=$(calcul "$1")
    if [ -z "$m" ] || [ "$t" -lt "$m" ]; then m=$t; fi
  done
  echo "$m"
}
tRef=$(meilleur "$reference/08_snake")
tPgo=$(meilleur "$dossier/08_snake")
echo "Partie --turbo -l 1200 -H 800 -s 100000 --graine 42, calcul (meilleur de 3) :"
echo "  Release ordinaire : $tRef ms"
echo "  Release + PGO     : $tPgo ms"
echo "Executable optimise : $dossier/08_snake"
