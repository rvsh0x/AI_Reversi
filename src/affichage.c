#ifndef _AFFICHAGE_C_
#define _AFFICHAGE_C_

#include <stdio.h>
#include "../include/affichage.h"

void afficher_plateau(Plateau *plateau) {
    int i, j;
    printf("   ________________\n");
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        printf("%d |", i + 1);
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            /* Affiche le contenu de la case en fonction de l'énumération */
            if (plateau->cases[i][j] == VIDE) {
                printf(" .");
            } else if (plateau->cases[i][j] == NOIR) {
                printf(" N");
            } else if (plateau->cases[i][j] == BLANC) {
                printf(" B");
            }
        }
        printf(" |\n");
    }
    printf("   ----------------\n");
    printf("    A B C D E F G H\n");
}

void afficher_coup_ordinateur(Position coup) {
    printf("L'ordinateur joue en %d%c\n", coup.ligne + 1, 'A' + coup.colonne);
}

void demander_coup(Position *coup) {
    char colonne;
    int ligne;
    int result;

    do {
        printf("Entrez votre coup (ligne [1-8] colonne [A-H]): ");
        result = scanf("%d %c", &ligne, &colonne);

        /* Vider le buffer d'entrée */
        while (getchar() != '\n');

        if (result != 2 || ligne < 1 || ligne > 8 || colonne < 'A' || colonne > 'H') {
            printf("Erreur de saisie. Veuillez réessayer.\n");
            coup->ligne = -1;  /* Position invalide pour forcer la répétition */
            coup->colonne = -1;  /* Position invalide pour forcer la répétition */
        } else {
            coup->ligne = ligne - 1;
            coup->colonne = colonne - 'A';
        }
    } while (coup->ligne == -1 || coup->colonne == -1);
}

void afficher_resultat_final(Plateau *plateau) {
    int compte_noir = 0, compte_blanc = 0;
    int i, j;

    /* Compte les pions de chaque couleur */
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            if (plateau->cases[i][j] == NOIR) {
                compte_noir++;
            } else if (plateau->cases[i][j] == BLANC) {
                compte_blanc++;
            }
        }
    }
    
    printf("\nFin de la partie !\n");
    printf("Score final :\n");
    printf("Noir : %d pions\n", compte_noir);
    printf("Blanc : %d pions\n", compte_blanc);
    
    if (compte_noir > compte_blanc) {
        printf("Le joueur Noir gagne !\n");
    } else if (compte_blanc > compte_noir) {
        printf("Le joueur Blanc gagne !\n");
    } else {
        printf("Match nul !\n");
    }
}

void afficher_bienvenue() {
    printf("*********************************************\n");
    printf("        BIENVENUE AU JEU REVERSI       \n");
    printf("*********************************************\n");
}

/* Affiche l'aide et les options disponibles */
/* Affiche l'aide et les options disponibles */
void afficher_aide(char *nom_programme) {
    printf("Usage: %s [options]\n\n", nom_programme);
    printf("Options:\n");
    printf("  -t               Mode terminal (par défaut)\n");
    printf("  -g               Mode graphique\n");
    printf("  -c <couleur>     Couleur du joueur (B: Blanc, N: Noir)\n");
    printf("  -n <niveau>      Niveau de l'IA (1-6):\n");
    printf("                     1: IA aléatoire\n");
    printf("                     2: Arbre fixe de profondeur 2\n");
    printf("                     3: Arbre de profondeur variable\n");
    printf("                     4: Alpha-Beta\n");
    printf("                     5: Optimisation mémoire\n");
    printf("                     6: IA avancée avec adaptation phase de jeu\n");
    printf("  -p <profondeur>  Profondeur de l'arbre (pour niveaux 3+, défaut: 3)\n");
    printf("  -e <profondeur>  Profondeur supplémentaire pour l'endgame (pour niveau 6, défaut: 6)\n");
    printf("  -h               Affiche cette aide\n");
    printf("\nExemples:\n");
    printf("  %s -n 1                  Jouer contre l'IA aléatoire en mode terminal\n", nom_programme);
    printf("  %s -n 3 -p 5 -c B        Jouer avec les Blancs contre l'IA de niveau 3 avec profondeur 5\n", nom_programme);
    printf("  %s -g -n 6 -p 4 -e 8 -c N Jouer avec les Noirs contre l'IA avancée avec profondeur 4 (+8 en endgame)\n", nom_programme);
}

#endif /* _AFFICHAGE_C_ */