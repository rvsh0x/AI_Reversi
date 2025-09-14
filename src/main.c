#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include "../include/jeu.h"

int main(int argc, char *argv[]) {
    Plateau plateau;
    ConfigurationJeu config;
    char choix = '\0';  /*Valeur par défaut pour la couleur */
    int opt;
    int choix_scanf;
    
    /* Initialisation des paramètres par défaut */
    config.niveau = 1;        /* IA aléatoire par défaut */
    config.profondeur = 3;    /* Profondeur par défaut */
    config.mode_graphique = 0; /* Mode terminal par défaut */
    config.profondeur_endgame = 6; /* Augmentation de profondeur pour l'endgame par défaut */
    
    /* Analyse des arguments de ligne de commande avec getopt */
    while ((opt = getopt(argc, argv, "tgn:p:c:e:h")) != -1) {
        switch (opt) {
            case 't': /* Mode terminal */
                config.mode_graphique = 0;
                break;
            case 'g': /* Mode graphique */
                config.mode_graphique = 1;
                break;
            case 'n': /* Niveau de l'IA */
                config.niveau = atoi(optarg);
                if (config.niveau < 1 || config.niveau > 6) {
                    printf("Niveau %d invalide. Utilisation du niveau 1 (aléatoire).\n", config.niveau);
                    config.niveau = 1;
                }
                break;
            case 'p': /* Profondeur de l'arbre */
                config.profondeur = atoi(optarg);
                if (config.profondeur <= 0) {
                    fprintf(stderr, "Profondeur %d invalide. Utilisation de la profondeur par défaut (3).\n", 
                            config.profondeur);
                    config.profondeur = 3;
                }
                break;
            case 'e': /* Profondeur supplémentaire pour endgame */
                config.profondeur_endgame = atoi(optarg);
                if (config.profondeur_endgame < 0) {
                    fprintf(stderr, "Profondeur endgame %d invalide. Utilisation de la valeur par défaut (6).\n", 
                            config.profondeur_endgame);
                    config.profondeur_endgame = 6;
                }
                break;
            case 'c': /* Couleur du joueur */
                choix = optarg[0];
                if (choix != 'B' && choix != 'N' && choix != 'b' && choix != 'n') {
                    printf("Couleur %c invalide. Veuillez choisir B (Blanc) ou N (Noir).\n", choix);
                    choix = '\0'; 
                }
                break;
            case 'h': /* Aide */
                afficher_aide(argv[0]);
                exit(EXIT_SUCCESS);
            default: /* Option non reconnue */
                afficher_aide(argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    
    /* Afficher la configuration active */
    printf("=== REVERSI - Configuration ===\n");
    printf("Mode : %s\n", config.mode_graphique ? "Graphique" : "Terminal");
    printf("Niveau d'IA : %d ", config.niveau);
    switch (config.niveau) {
        case 1: printf("(Aléatoire)\n"); break;
        case 2: printf("(Arbre fixe de profondeur 2)\n"); break;
        case 3: printf("(Arbre de profondeur variable)\n"); break;
        case 4: printf("(Alpha-Beta)\n"); break;
        case 5: printf("(Optimisation mémoire)\n"); break;
        case 6: printf("(IA avancée avec adaptation phase de jeu (ENDGAME))\n"); break;
    }
    if (config.niveau >= 3) {
        printf("Profondeur : %d\n", config.profondeur);
        if (config.niveau == 6) {
            printf("Profondeur supplémentaire en endgame : +%d\n", config.profondeur_endgame);
        }
    }
    printf("\n");
    srand(time(NULL));
    afficher_bienvenue();

    /* Demander la couleur si elle n'a pas été spécifiée en ligne de commande */
    if (choix == '\0') {
        do {
            printf("Voulez-vous jouer les Blancs (B) ou les Noirs (N) ? ");
            choix_scanf = scanf(" %c", &choix);
        } while (choix_scanf != 1 || (choix != 'B' && choix != 'N' && choix != 'b' && choix != 'n'));
    } else {
        printf("Vous avez choisi de jouer les %s.\n", (choix == 'B' || choix == 'b') ? "Blancs" : "Noirs");
    }

    /* Initialisation du plateau */
    initialiser_plateau(&plateau);
    plateau.joueur_humain = (choix == 'B' || choix == 'b') ? BLANC : NOIR;
    plateau.ordinateur = (plateau.joueur_humain == BLANC) ? NOIR : BLANC;

    /* Lancer la partie avec la configuration choisie */
    jouer_partie_unifiee(&plateau, &config);

    exit(EXIT_SUCCESS);
}