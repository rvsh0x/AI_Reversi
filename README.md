# Intelligence artificielle pour le jeu Reversi


Développement d'une intelligence artificielle pour le jeu Reversi en C. Utilisation d'algorithmes de recherche avancés (MinMax avec élagage alpha-beta) et d'heuristiques sophistiquées pour une prise de décision optimale.

## Compilation 

Pour compiler le projet :
make


Pour compiler uniquement la présentation :
make presentation


Pour nettoyer les fichiers générés :

make clean               # Nettoie les fichiers binaires du projet
make clean-presentation  # Nettoie les fichiers temporaires de la présentation
make clean-all           # Nettoie tout


## Exécution

Pour lancer le jeu principal :
bin/reversi

Par défaut, le jeu se lance en mode terminal avec une IA de niveau 1 (aléatoire).

### Options disponibles :
- `-t` : Mode terminal (par défaut)
- `-g` : Mode graphique
- `-n X` : Niveau d'IA (1 à 6)
- `-p X` : Profondeur de recherche
- `-c B/N` : Choisir sa couleur (B pour blanc, N pour noir)
- `-e X` : Profondeur supplémentaire pour endgame
- `-h` : Afficher l'aide

Exemple :
bin/reversi -g -n 6 -p 5 -c N
Lance le jeu en mode graphique, avec l'IA niveau 6, profondeur 5, et le joueur joue les noirs.

## Tests

Avant d'exécuter les tests, veuillez générer les plateaux de test :
cd test
chmod +x plateaux.sh
./plateaux.sh

cd ..

Ensuite, vous pouvez lancer les différents tests :

1. **IA contre IA** :
bin/ia_vs_ia


2. **Tests des algorithmes sur plateaux** :
test/test_algos

3. **Test de cohérence** :
test/test_coherence


4. **Test d'évaluation** :
test/test_evaluation

5. **Test mémoire** :
test/test_memoire

## Auteurs

- LAICHE
- GHODBANE
- BAADACHE


