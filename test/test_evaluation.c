/**
 * test_evaluation.c - Test des fonctions d'évaluation pour le jeu Reversi
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/ia.h"
#include "../include/reversi.h"
#include "../include/evaluation.h"
#include "../include/affichage.h"

/* Structure pour analyser un coup */
typedef struct {
    Position coup;
    int score;
    int positions_strategiques;
    int mobilite;
    int stabilite;
    int disk_square;
    int parite;
    int ensembles;
} AnalyseCoup;

/* Fonction pour initialiser un plateau de test - configuration de début */
void initialiser_plateau_test_debut(Plateau *plateau) {
    int i, j;
    
    /* Initialisation du plateau vide */
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            plateau->cases[i][j] = VIDE;
        }
    }
    
    /* Configuration de début de partie */
    plateau->cases[2][2] = BLANC;
    plateau->cases[2][3] = BLANC;
    plateau->cases[2][4] = BLANC;
    plateau->cases[3][2] = NOIR;
    plateau->cases[3][3] = NOIR;
    plateau->cases[3][4] = BLANC;
    plateau->cases[4][3] = NOIR;
    plateau->cases[4][4] = NOIR;
    plateau->cases[5][5] = NOIR;
    
    /* Configurer les joueurs */
    plateau->ordinateur = NOIR;
    plateau->joueur_humain = BLANC;
}

/* Fonction pour initialiser un plateau de test - configuration milieu */
void initialiser_plateau_test_milieu(Plateau *plateau) {
    int i, j;
    
    /* Initialisation du plateau vide */
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            plateau->cases[i][j] = VIDE;
        }
    }
    
    /* Configuration spécifique phase milieu */
    plateau->cases[2][3] = NOIR;
    plateau->cases[2][4] = BLANC;
    plateau->cases[3][2] = NOIR;
    plateau->cases[3][3] = BLANC;
    plateau->cases[3][4] = BLANC;
    plateau->cases[4][1] = NOIR;
    plateau->cases[4][2] = NOIR;
    plateau->cases[4][3] = BLANC;
    plateau->cases[4][4] = BLANC;
    plateau->cases[5][2] = BLANC;
    plateau->cases[5][3] = BLANC;
    plateau->cases[5][4] = NOIR;
    plateau->cases[5][5] = NOIR;
    plateau->cases[6][1] = BLANC;
    plateau->cases[6][2] = BLANC;
    plateau->cases[6][3] = BLANC;
    plateau->cases[6][4] = NOIR;
    plateau->cases[7][2] = BLANC;
    plateau->cases[7][4] = NOIR;
    plateau->cases[7][5] = NOIR;
    
    /* Configurer les joueurs */
    plateau->ordinateur = NOIR;
    plateau->joueur_humain = BLANC;
}

/* Fonction pour initialiser un plateau de test - configuration fin */
void initialiser_plateau_test_fin(Plateau *plateau) {
    int i, j;
    
    /* Initialisation du plateau vide */
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            plateau->cases[i][j] = VIDE;
        }
    }
    
    /* Configuration spécifique phase fin (plateau presque rempli) */
    plateau->cases[0][4] = NOIR;
    plateau->cases[0][5] = NOIR;
    plateau->cases[1][3] = NOIR;
    plateau->cases[1][4] = NOIR;
    plateau->cases[1][5] = NOIR;
    plateau->cases[2][0] = NOIR;
    plateau->cases[2][1] = NOIR;
    plateau->cases[2][2] = NOIR;
    plateau->cases[2][3] = BLANC;
    plateau->cases[2][4] = BLANC;
    plateau->cases[3][0] = NOIR;
    plateau->cases[3][1] = NOIR;
    plateau->cases[3][2] = NOIR;
    plateau->cases[3][3] = NOIR;
    plateau->cases[3][4] = BLANC;
    plateau->cases[4][0] = NOIR;
    plateau->cases[4][1] = NOIR;
    plateau->cases[4][2] = NOIR;
    plateau->cases[4][3] = BLANC;
    plateau->cases[4][4] = NOIR;
    plateau->cases[4][5] = BLANC;
    plateau->cases[4][6] = BLANC;
    plateau->cases[4][7] = BLANC;
    plateau->cases[5][0] = NOIR;
    plateau->cases[5][1] = NOIR;
    plateau->cases[5][2] = NOIR;
    plateau->cases[5][3] = BLANC;
    plateau->cases[5][4] = NOIR;
    plateau->cases[5][5] = NOIR;
    plateau->cases[5][6] = BLANC;
    plateau->cases[5][7] = BLANC;
    plateau->cases[6][1] = NOIR;
    plateau->cases[6][2] = BLANC;
    plateau->cases[6][3] = BLANC;
    plateau->cases[6][4] = BLANC;
    plateau->cases[6][5] = BLANC;
    plateau->cases[6][6] = NOIR;
    plateau->cases[6][7] = BLANC;
    plateau->cases[7][0] = NOIR;
    plateau->cases[7][1] = BLANC;
    plateau->cases[7][2] = BLANC;
    plateau->cases[7][3] = BLANC;
    plateau->cases[7][4] = BLANC;
    plateau->cases[7][5] = BLANC;
    plateau->cases[7][6] = BLANC;
    plateau->cases[7][7] = NOIR;
    
    /* Configurer les joueurs */
    plateau->ordinateur = NOIR;
    plateau->joueur_humain = BLANC;
}

/* Fonction pour obtenir les coefficients de pondération selon la phase */
void obtenir_coefficients(const char* phase, int *coeff_pos, int *coeff_mob,
                        int *coeff_stab, int *coeff_disk, int *coeff_par, int *coeff_ens) {
    if (strcmp(phase, "debut") == 0) {
        *coeff_pos = 2;
        *coeff_mob = 5;
        *coeff_stab = 2;
        *coeff_disk = 1;
        *coeff_par = 0;
        *coeff_ens = 1;
    } else if (strcmp(phase, "milieu") == 0) {
        *coeff_pos = 3;
        *coeff_mob = 4;
        *coeff_stab = 3;
        *coeff_disk = 1;
        *coeff_par = 1;
        *coeff_ens = 3;
    } else { /* "fin" ou "endgame" */
        *coeff_pos = 2;
        *coeff_mob = 1;
        *coeff_stab = 6;
        *coeff_disk = 0;
        *coeff_par = 3;
        *coeff_ens = 2;
    }
}

/* Fonction pour analyser un coup en détail avec pondération affichée */
AnalyseCoup analyser_coup(Plateau *plateau, Position coup, Joueur joueur) {
    AnalyseCoup analyse;
    Plateau plateau_apres;
    
    /* Initialiser l'analyse avec le coup */
    analyse.coup = coup;
    
    /* Copier le plateau et jouer le coup */
    plateau_apres = *plateau;
    jouer_coup(&plateau_apres, coup, joueur);
    
    /* Évaluer les différents critères */
    analyse.positions_strategiques = evaluation_positions_strategiques(&plateau_apres, joueur);
    analyse.mobilite = evaluation_mobilite(&plateau_apres, joueur);
    analyse.stabilite = evaluation_stabilite_etape7(&plateau_apres, joueur);
    analyse.disk_square = evaluation_disk_square(&plateau_apres, joueur);
    analyse.parite = evaluation_parite(&plateau_apres, joueur);
    analyse.ensembles = evaluation_ensembles_consecutifs(&plateau_apres, joueur);
    
    /* Calculer le score pondéré */
    analyse.score = evaluation_ponderee7(&plateau_apres, joueur);
    
    return analyse;
}

/* Fonction de comparaison pour trier les analyses de coups */
int comparer_analyses(const void *a, const void *b) {
    AnalyseCoup *analyse_a = (AnalyseCoup *)a;
    AnalyseCoup *analyse_b = (AnalyseCoup *)b;
    
    /* Tri par score décroissant */
    return analyse_b->score - analyse_a->score;
}

/* Fonction pour afficher la pondération selon la phase */
void afficher_ponderation_phase(const char* phase) {
    printf("\n=== PONDÉRATION DES CRITÈRES EN PHASE %s ===\n", phase);
    printf("|--------------------|-------|--------|----------|------------|--------|----------|\n");
    printf("| Phase              | Pos.  | Mobil. | Stabil.  | Disk-Square | Parité | Ensembles |\n");
    printf("|--------------------|-------|--------|----------|------------|--------|----------|\n");
    
    if (strcmp(phase, "debut") == 0) {
        printf("| Début de partie    |   x2  |   x5   |    x2    |     x1     |   x0   |    x1    |\n");
        printf("|                    |       |        |          |            |        |          |\n");
        printf("| Priorité: Mobilité et positions stratégiques                                    |\n");
    } else if (strcmp(phase, "milieu") == 0) {
        printf("| Milieu de partie   |   x3  |   x4   |    x3    |     x1     |   x1   |    x3    |\n");
        printf("|                    |       |        |          |            |        |          |\n");
        printf("| Priorité: Équilibre entre les critères avec accent sur mobilité                 |\n");
    } else { /* "fin" ou "endgame" */
        printf("| Fin de partie      |   x2  |   x1   |    x6    |     x0     |   x3   |    x2    |\n");
        printf("|                    |       |        |          |            |        |          |\n");
        printf("| Priorité: Stabilité et parité                                                  |\n");
    }
    printf("|--------------------|-------|--------|----------|------------|--------|----------|\n\n");
}

/* Fonction pour afficher le détail du score pondéré */
void afficher_detail_score(AnalyseCoup analyse, const char* phase) {
    int coeff_pos, coeff_mob, coeff_stab, coeff_disk, coeff_par, coeff_ens;
    int pos_ponderee, mob_ponderee, stab_ponderee, disk_ponderee, par_ponderee, ens_ponderee;
    
    /* Obtenir les coefficients selon la phase */
    obtenir_coefficients(phase, &coeff_pos, &coeff_mob, &coeff_stab, &coeff_disk, &coeff_par, &coeff_ens);
    
    /* Calculer les scores pondérés */
    pos_ponderee = analyse.positions_strategiques * coeff_pos;
    mob_ponderee = analyse.mobilite * coeff_mob;
    stab_ponderee = analyse.stabilite * coeff_stab;
    disk_ponderee = analyse.disk_square * coeff_disk;
    par_ponderee = analyse.parite * coeff_par;
    ens_ponderee = analyse.ensembles * coeff_ens;
    
    /* Afficher le détail du calcul */
    printf("Détail du score pondéré:\n");
    printf("- Positions stratégiques: %d x coeff = %d\n", analyse.positions_strategiques, pos_ponderee);
    printf("- Mobilité:               %d x coeff = %d\n", analyse.mobilite, mob_ponderee);
    printf("- Stabilité:              %d x coeff = %d\n", analyse.stabilite, stab_ponderee);
    printf("- Disk-Square:            %d x coeff = %d\n", analyse.disk_square, disk_ponderee);
    printf("- Parité:                 %d x coeff = %d\n", analyse.parite, par_ponderee);
    printf("- Ensembles consécutifs:  %d x coeff = %d\n", analyse.ensembles, ens_ponderee);
}

/* Fonction principale d'analyse des coups */
void analyser_coups(Plateau *plateau, Joueur joueur) {
    int nb_coups, i;
    Position *coups_possibles;
    AnalyseCoup *analyses;
    clock_t debut, fin;
    double temps_total;
    const char* phase;
    Plateau plateau_apres;
    
    /* Déterminer la phase avec la fonction existante */
    phase = determiner_phase_partie(plateau);
    
    printf("=== ANALYSE DES COUPS POSSIBLES ===\n\n");
    printf("Phase de jeu: %s\n", phase);
    
    /* Afficher la pondération pour cette phase */
    afficher_ponderation_phase(phase);
    
    /* Afficher le plateau */
    printf("Plateau actuel:\n");
    afficher_plateau(plateau);
    
    /* Générer les coups possibles */
    coups_possibles = generer_coups_possibles(plateau, joueur, &nb_coups);
    if (nb_coups == 0) {
        printf("Aucun coup possible pour le joueur %s.\n", 
               joueur == NOIR ? "NOIR" : "BLANC");
        return;
    }
    
    /* Allouer de la mémoire pour les analyses */
    analyses = malloc(nb_coups * sizeof(AnalyseCoup));
    if (analyses == NULL) {
        printf("Erreur: Impossible d'allouer de la mémoire pour les analyses.\n");
        free(coups_possibles);
        return;
    }
    
    debut = clock();
    
    /* Analyser chaque coup possible */
    for (i = 0; i < nb_coups; i++) {
        analyses[i] = analyser_coup(plateau, coups_possibles[i], joueur);
    }
    
    /* Trier les coups par score */
    qsort(analyses, nb_coups, sizeof(AnalyseCoup), comparer_analyses);
    
    fin = clock();
    temps_total = (double)(fin - debut) / CLOCKS_PER_SEC;
    
    /* Afficher les résultats */
    printf("\nCoups possibles par ordre de qualité (%d coups):\n\n", nb_coups);
    printf("| Rang | Coup    | Score | Positions | Mobilité | Stabilité | Disk-Square | Parité | Ensembles |\n");
    printf("|------|---------|-------|-----------|----------|-----------|-------------|--------|----------|\n");
    
    for (i = 0; i < nb_coups; i++) {
        printf("| %-4d | (%d,%d) | %-5d | %-9d | %-8d | %-9d | %-11d | %-6d | %-9d |\n", 
               i + 1,
               analyses[i].coup.ligne, analyses[i].coup.colonne,
               analyses[i].score,
               analyses[i].positions_strategiques,
               analyses[i].mobilite,
               analyses[i].stabilite,
               analyses[i].disk_square,
               analyses[i].parite,
               analyses[i].ensembles);
    }
    
    /* Afficher le détail du score pour le meilleur coup */
    printf("\nDétail du meilleur coup (%d,%d):\n", 
           analyses[0].coup.ligne, analyses[0].coup.colonne);
    afficher_detail_score(analyses[0], phase);
    
    /* Afficher le plateau après avoir joué le meilleur coup */
    plateau_apres = *plateau;
    jouer_coup(&plateau_apres, analyses[0].coup, joueur);
    
    printf("\nPlateau après avoir joué le meilleur coup:\n");
    afficher_plateau(&plateau_apres);
    
    printf("\nScore final: %d\n", analyses[0].score);
    printf("Temps d'analyse: %.6f secondes\n", temps_total);
    
    /* Libérer la mémoire */
    free(coups_possibles);
    free(analyses);
}

/* Menu principal */
void menu_tests() {
    int choix;
    Plateau plateau;
    int scanf_result;
    
    do {
        printf("\n=== MENU TESTS D'ÉVALUATION ===\n");
        printf("1. Analyser les coups possibles (phase début)\n");
        printf("2. Analyser les coups possibles (phase milieu)\n");
        printf("3. Analyser les coups possibles (phase fin)\n");
        printf("0. Quitter\n");
        printf("\nVotre choix: ");
        
        scanf_result = scanf("%d", &choix);
        if (scanf_result != 1) {
            while (getchar() != '\n'); /* Vider le buffer */
            choix = -1;
        } else {
            switch (choix) {
                case 1:
                    initialiser_plateau_test_debut(&plateau);
                    analyser_coups(&plateau, NOIR);
                    break;
                    
                case 2:
                    initialiser_plateau_test_milieu(&plateau);
                    analyser_coups(&plateau, NOIR);
                    break;
                    
                case 3:
                    initialiser_plateau_test_fin(&plateau);
                    analyser_coups(&plateau, NOIR);
                    break;
                    
                case 0:
                    printf("Au revoir!\n");
                    break;
                    
                default:
                    printf("Choix invalide!\n");
            }
        }
    } while (choix != 0);
}

/* Fonction principale */
int main() {
    printf("=== TESTS DES FONCTIONS D'ÉVALUATION POUR REVERSI ===\n\n");
    menu_tests();
    exit(EXIT_SUCCESS);
}