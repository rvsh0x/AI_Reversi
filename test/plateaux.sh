#!/bin/env bash

# Configuration
REPERTOIRE_TESTS="plateaux"
EXECUTABLE="./gen_plateaux"

# Créer/nettoyer le répertoire de test
mkdir -p "$REPERTOIRE_TESTS"
echo "Répertoire $REPERTOIRE_TESTS prêt."
rm -f "$REPERTOIRE_TESTS"/*.txt

# Vérifier l'existence de l'exécutable
if [[ ! -f "$EXECUTABLE" ]]; then
  echo "ERREUR: L'exécutable $EXECUTABLE n'existe pas!"
  exit 1
fi

# Exécuter le programme de test
echo "Exécution du programme de test..."
$EXECUTABLE

# Déplacer les fichiers générés
echo "Déplacement des fichiers vers $REPERTOIRE_TESTS..."
mv plateau_test_*.txt statistiques_partie.txt "$REPERTOIRE_TESTS/" 2>/dev/null

# Afficher le résultat
echo "Terminé. $(ls "$REPERTOIRE_TESTS"/*.txt 2>/dev/null | wc -l) fichiers générés."