#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> /* Pour sleep() */
#include <getopt.h>

#include "../include/reversi.h"
#include "../include/affichage.h"
#include "../include/ia.h"
#include "../include/evaluation.h"
#include "../include/endgame.h"
#include "../include/nega_memoire.h"
#include "../include/affichage_graphique.h"

/* Configuration */
typedef struct {
    int profondeur_standard;       /* Profondeur initiale pour les deux IAs */
    int profondeur_endgame;        /* Profondeur accrue pour l'IA adaptative en phase endgame */
    int delai;                     /* Délai entre les coups */
    int ia_adaptative_commence;    /* 1 si l'IA adaptative commence */
    int mode_graphique;            /* 1 pour le mode graphique */
} Config;

/* Affiche le plateau simplement */
void afficher_plateau_simple(Plateau *plateau) {
    int i;
    int j;
    
    printf("\n  ");
    for (j = 0; j < TAILLE_PLATEAU; j++) {
        printf("%d ", j);
    }
    printf("\n");
    
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        printf("%d ", i);
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            if (plateau->cases[i][j] == VIDE) {
                printf(". ");
            } else if (plateau->cases[i][j] == NOIR) {
                printf("N ");
            } else {
                printf("B ");
            }
        }
        printf("\n");
    }
    printf("\n");
}

/* Joue un coup avec l'IA standard (NegaScout) */
Position jouer_coup_standard(Plateau *plateau, Joueur joueur, int profondeur) {
    Position coup;
    
    /* Version optimisée en mémoire avec tri statique */
    coup = negascout_memoire_optimisee_tri_statique(plateau, joueur, profondeur);
    
    printf("IA Standard joue en (%d,%d)\n", coup.ligne, coup.colonne);
    
    return coup;
}

/* Joue un coup avec l'IA adaptative (qui peut basculer vers Endgame) */
Position jouer_coup_adaptative(Plateau *plateau, Joueur joueur, int profondeur_standard, 
                              int profondeur_endgame, int est_endgame) {
    Position coup;
    
    if (est_endgame) {
        printf("Mode ENDGAME activé avec profondeur %d\n", profondeur_endgame);
        /* Utiliser l'algorithme d'endgame avec profondeur accrue */
        coup = choisir_meilleur_coup_endgame_optimise(plateau, joueur, profondeur_endgame);
        printf("IA Adaptative (Endgame) joue en (%d,%d)\n", coup.ligne, coup.colonne);
    } else {
        /* Utiliser le même algorithme NegaScout que l'IA standard */
        coup = negascout_memoire_optimisee_tri_statique(plateau, joueur, profondeur_standard);
        printf("IA Adaptative (Standard) joue en (%d,%d)\n", coup.ligne, coup.colonne);
    }
    
    return coup;
}

/* Affiche le résultat final */
void afficher_resultat(Plateau *plateau, Joueur standard, Joueur adaptative) {
    int score_standard;
    int score_adaptative;
    int i;
    int j;
    
    score_standard = 0;
    score_adaptative = 0;
    
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            if (plateau->cases[i][j] == standard) {
                score_standard++;
            } else if (plateau->cases[i][j] == adaptative) {
                score_adaptative++;
            }
        }
    }
    
    printf("\n====== RÉSULTAT FINAL ======\n");
    printf("IA Standard (%c): %d pions\n", 
           (standard == NOIR) ? 'N' : 'B', score_standard);
    printf("IA Adaptative (%c): %d pions\n", 
           (adaptative == NOIR) ? 'N' : 'B', score_adaptative);
    
    if (score_standard > score_adaptative) {
        printf("L'IA Standard a gagné !\n");
    } else if (score_adaptative > score_standard) {
        printf("L'IA Adaptative a gagné !\n");
    } else {
        printf("Match nul !\n");
    }
    printf("===========================\n\n");
}

/* Fonction principale pour faire jouer les IAs l'une contre l'autre */
void jouer_ia_contre_ia(Config config) {
    Plateau plateau;
    Joueur joueur_standard;
    Joueur joueur_actuel;
    Joueur joueur_adaptative;
    int tour;
    int passe_consecutif;
    int i;
    int j;
    int coup_possible;
    Position coup,pos;
    int est_endgame;
    const char* phase_actuelle;
    
    /* Initialisation */
    initialiser_plateau(&plateau);
    passe_consecutif = 0;
    est_endgame = 0;
    
    /* Attribution des couleurs */
    if (config.ia_adaptative_commence) {
        joueur_adaptative = NOIR;
        joueur_standard = BLANC;
        tour = 0; /* 0 pour Adaptative, 1 pour Standard */
        printf("IA Adaptative joue les NOIRS et commence\n");
        printf("IA Standard joue les BLANCS\n\n");
    } else {
        joueur_standard = NOIR;
        joueur_adaptative = BLANC;
        tour = 1; /* 1 pour Standard, 0 pour Adaptative */
        printf("IA Standard joue les NOIRS et commence\n");
        printf("IA Adaptative joue les BLANCS\n\n");
    }
    
    printf("Les deux IAs utilisent l'algorithme NegaScout avec profondeur %d\n", 
           config.profondeur_standard);
    printf("L'IA Adaptative basculera vers l'algorithme Endgame avec profondeur %d\n",
           config.profondeur_endgame);
    printf("quand la phase endgame sera détectée\n\n");
    
    /* Mode graphique */
    if (config.mode_graphique) {
        initialiser_fenetre_graphique();
    }
    
    /* Boucle principale */
    while (!partie_terminee(&plateau) && passe_consecutif < 2) {
        /* Afficher le plateau selon le mode */
        if (config.mode_graphique) {
            afficher_plateau_graphique(&plateau, 0); /* 0 car ce n'est pas le tour d'un humain */
        } else {
            afficher_plateau_simple(&plateau);
        }
        
        /* Vérifier la phase actuelle de jeu */
        phase_actuelle = determiner_phase_partie(&plateau);
        printf("Phase actuelle: %s\n", phase_actuelle);
        
        /* Détection de la phase endgame */
        if (!est_endgame && strcmp(phase_actuelle, "endgame") == 0) {
            est_endgame = 1;
            printf("\n=== PHASE ENDGAME DÉTECTÉE ===\n");
            printf("L'IA Adaptative utilise maintenant l'algorithme Endgame de profondeur %d\n\n", 
                   config.profondeur_endgame);
        }
        
        /* Déterminer le joueur actuel */
        joueur_actuel = tour ? joueur_standard : joueur_adaptative;
        
        /* Vérifier s'il y a des coups possibles */
        coup_possible = 0;
        for (i = 0; i < TAILLE_PLATEAU && !coup_possible; i++) {
            for (j = 0; j < TAILLE_PLATEAU && !coup_possible; j++) {
                pos.ligne = i;
                pos.colonne = j;
                if (coup_valide(&plateau, pos, joueur_actuel)) {
                    coup_possible = 1;
                }
            }
        }
        
        /* Si aucun coup n'est possible, passer le tour */
        if (!coup_possible) {
            printf("%s doit passer son tour.\n", tour ? "IA Standard" : "IA Adaptative");
            passe_consecutif++;
            tour = !tour; /* Passer au joueur suivant */
        } else {
            passe_consecutif = 0;
            
            /* Faire jouer l'IA */
            if (tour) { /* Tour de Standard */
                printf("Tour de l'IA Standard (%c)...\n", (joueur_standard == NOIR) ? 'N' : 'B');
                coup = jouer_coup_standard(&plateau, joueur_standard, config.profondeur_standard);
            } else { /* Tour de Adaptative */
                printf("Tour de l'IA Adaptative (%c)...\n", 
                       (joueur_adaptative == NOIR) ? 'N' : 'B');
                coup = jouer_coup_adaptative(&plateau, joueur_adaptative, 
                                           config.profondeur_standard,
                                           config.profondeur_endgame,
                                           est_endgame);
            }
            
            /* Jouer le coup */
            jouer_coup(&plateau, coup, joueur_actuel);
            
            /* En mode graphique, afficher le coup */
            if (config.mode_graphique) {
                afficher_coup_ordinateur_graphique(coup);
            }
            
            /* Attendre le délai configuré */
            if (config.delai > 0) {
                printf("Attente de %d seconde(s)...\n", config.delai);
                sleep(config.delai);
            }
            
            tour = !tour; /* Passer au joueur suivant */
        }
    }
    
    /* Afficher le plateau final */
    if (config.mode_graphique) {
        afficher_plateau_graphique(&plateau, 0);
        afficher_resultat_graphique(&plateau);
    } else {
        afficher_plateau_simple(&plateau);
    }
    
    /* Afficher le résultat */
    afficher_resultat(&plateau, joueur_standard, joueur_adaptative);
    
    /* Fermer la fenêtre graphique si nécessaire */
    if (config.mode_graphique) {
        printf("Appuyez sur une touche pour fermer la fenêtre...\n");
        getchar();
        fermer_fenetre_graphique();
    }
}

/* Affiche l'aide */
void afficher_aide_TEST() {
    printf("Usage: ./test_ia_vs_ia [options]\n");
    printf("Options:\n");
    printf("  -h           Affiche cette aide\n");
    printf("  -p <prof>    Profondeur standard pour les deux IAs (défaut: 4)\n");
    printf("  -e <prof>    Profondeur pour l'IA adaptative en phase endgame (défaut: 10)\n");
    printf("  -d <délai>   Délai entre les coups en secondes (défaut: 1)\n");
    printf("  -a           L'IA adaptative commence (défaut: IA standard commence)\n");
    printf("  -g           Mode graphique\n");
}

/* Fonction principale */
int main(int argc, char *argv[]) {
    Config config;
    int opt;
    
    /* Valeurs par défaut */
    config.profondeur_standard = 4;
    config.profondeur_endgame = 10;
    config.delai = 1;
    config.ia_adaptative_commence = 0;
    config.mode_graphique = 0;
    
    /* Traitement des options avec getopt */
    while ((opt = getopt(argc, argv, "hp:e:d:ag")) != -1) {
        switch (opt) {
            case 'h':
                afficher_aide_TEST();
                return EXIT_SUCCESS;
            case 'p':
                config.profondeur_standard = atoi(optarg);
                break;
            case 'e':
                config.profondeur_endgame = atoi(optarg);
                break;
            case 'd':
                config.delai = atoi(optarg);
                break;
            case 'a':
                config.ia_adaptative_commence = 1;
                break;
            case 'g':
                config.mode_graphique = 1;
                break;
            default:
                afficher_aide_TEST();
                exit(EXIT_FAILURE);
        }
    }
    
    /* Valider les paramètres */
    if (config.profondeur_standard <= 0 || config.profondeur_endgame <= 0 || config.delai < 0) {
        printf("Erreur: Les valeurs de profondeur et délai doivent être positives.\n");
        exit(EXIT_FAILURE);
    }
    
    /* Afficher la configuration */
    printf("=== TEST IA Standard vs IA Adaptative ===\n");
    printf("Profondeur standard (deux IAs): %d\n", config.profondeur_standard);
    printf("Profondeur endgame (IA adaptative): %d\n", config.profondeur_endgame);
    printf("Délai entre coups: %d seconde(s)\n", config.delai);
    printf("Mode: %s\n", config.mode_graphique ? "Graphique" : "Console");
    printf("Commence: %s\n", config.ia_adaptative_commence ? 
           "IA Adaptative" : "IA Standard");
    printf("========================================\n\n");
    
    srand(time(NULL));
    
    /* Lancer le test */
    jouer_ia_contre_ia(config);
    
    exit(EXIT_SUCCESS);
}