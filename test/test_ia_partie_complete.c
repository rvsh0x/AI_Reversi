/**
 * Programme qui simule une partie complète entre deux IA et enregistre
 * les états du plateau pour des tests ultérieurs.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/reversi.h"
#include "../include/ia.h"
#include "../include/affichage.h"
#include "../include/nega_memoire.h" 

/* Constantes globales */
#define MAX_COUPS 60    
#define PROFONDEUR_IA 4   
#define TAILLE_PLATEAU 8  

/* Prototypes de fonctions */
void sauvegarder_plateau(Plateau *plateau, int numero_coup);
void simuler_partie_complete();
int jouer_un_tour(Plateau *plateau, Joueur joueur, int profondeur);
int est_partie_terminee(Plateau *plateau); /* Renommé pour éviter le conflit */
void ecrire_statistiques(int nb_coups_joues, int score_noir, int score_blanc);

/* 
 * Fonction principale
 */
int main() {
    srand(time(NULL));
    
    printf("=== SIMULATION DE PARTIE COMPLÈTE POUR GÉNÉRATION DE PLATEAUX DE TEST ===\n\n");
    
    /* Lancer la simulation et enregistrer les plateaux */
    simuler_partie_complete();
    
    printf("\n=== FIN DE LA SIMULATION ===\n");
    
    return EXIT_SUCCESS;
}

/*
 * Simule une partie complète entre deux IA et enregistre les plateaux
 */
void simuler_partie_complete() {
    Plateau plateau;
    int nb_coups_joues = 0;
    int tours_sans_coup = 0;
    Joueur joueur_actuel;
    int coup_joue;
    int score_noir, score_blanc;
    
    /* Initialiser le plateau de jeu */
    initialiser_plateau(&plateau);

    /* NOIR commence toujours */
    joueur_actuel = NOIR;
    
    printf("Début de la partie...\n\n");
    
    /* Boucle principale de la partie */
    while (!est_partie_terminee(&plateau) && tours_sans_coup < 2) {
        /* Afficher le plateau actuel */
        printf("Tour #%d - Joueur: %s\n", nb_coups_joues + 1, 
               (joueur_actuel == NOIR) ? "NOIR" : "BLANC");
        afficher_plateau(&plateau);
        
        /* Essayer de jouer un coup */
        coup_joue = jouer_un_tour(&plateau, joueur_actuel, PROFONDEUR_IA);
        
        /* Si un coup a été joué */
        if (coup_joue) {
            tours_sans_coup = 0;
            nb_coups_joues++;
            
            /* Si c'est BLANC qui vient de jouer, sauvegarder le plateau */
            if (joueur_actuel == BLANC) {
                printf("Sauvegarde du plateau après le coup #%d (BLANC)\n", nb_coups_joues);
                sauvegarder_plateau(&plateau, nb_coups_joues / 2);
            }
        } 
        /* Si aucun coup n'a pu être joué */
        else {
            printf("Le joueur %s ne peut pas jouer. Tour passé.\n", 
                  (joueur_actuel == NOIR) ? "NOIR" : "BLANC");
            tours_sans_coup++;
        }
        
        /* Passer au joueur suivant */
        joueur_actuel = (joueur_actuel == NOIR) ? BLANC : NOIR;
        
        printf("\n");
    }
    
    /* Afficher le résultat final */
    printf("Partie terminée après %d coups!\n", nb_coups_joues);
    afficher_plateau(&plateau);
    
    /* Compter les pions finaux */
    compter_pions(&plateau, &score_noir, &score_blanc);
    printf("Score final: NOIR %d - %d BLANC\n", score_noir, score_blanc);
    
    /* Déterminer le vainqueur */
    if (score_noir > score_blanc) {
        printf("Vainqueur: NOIR\n");
    } else if (score_blanc > score_noir) {
        printf("Vainqueur: BLANC\n");
    } else {
        printf("Match nul!\n");
    }
    
    /* Sauvegarder les statistiques de la partie */
    ecrire_statistiques(nb_coups_joues, score_noir, score_blanc);
}

/*
 * Joue un tour pour le joueur spécifié
 * Retourne 1 si un coup a été joué, 0 sinon
 */
int jouer_un_tour(Plateau *plateau, Joueur joueur, int profondeur) {
    Position *coups_possibles;
    int nb_coups;
    Position coup_choisi;
    
    /* Générer les coups possibles pour ce joueur */
    coups_possibles = generer_coups_possibles(plateau, joueur, &nb_coups);
    
    /* Si aucun coup n'est possible, retourner 0 */
    if (nb_coups == 0) {
        free(coups_possibles);
        return 0;
    }
    
    /* Utiliser NegaScout avec tri statique pour les deux joueurs */
    coup_choisi = negascout_memoire_optimisee_tri_statique(plateau, joueur, profondeur);
    
    /* Jouer le coup choisi */
    jouer_coup(plateau, coup_choisi, joueur);
    printf("Coup joué: (%d,%d)\n", coup_choisi.ligne, coup_choisi.colonne);
    
    free(coups_possibles);
    return 1;
}

/*
 * Vérifie si la partie est terminée (aucun coup possible pour les deux joueurs)
 */
int est_partie_terminee(Plateau *plateau) {
    int nb_coups_noir, nb_coups_blanc;
    Position *coups_noir, *coups_blanc;
    int terminee;
    
    /* Générer les coups possibles pour chaque joueur */
    coups_noir = generer_coups_possibles(plateau, NOIR, &nb_coups_noir);
    coups_blanc = generer_coups_possibles(plateau, BLANC, &nb_coups_blanc);
    
    /* La partie est terminée si aucun joueur ne peut jouer */
    terminee = (nb_coups_noir == 0 && nb_coups_blanc == 0);
    
    /* Libérer la mémoire */
    free(coups_noir);
    free(coups_blanc);
    
    return terminee;
}

/*
 * Sauvegarde l'état actuel du plateau dans un fichier
 */
void sauvegarder_plateau(Plateau *plateau, int numero_coup) {
    FILE *fichier;
    char nom_fichier[100];
    int i, j;
    
    /* Créer le nom du fichier */
    sprintf(nom_fichier, "plateau_test_%02d.txt", numero_coup);
    
    /* Ouvrir le fichier en écriture */
    fichier = fopen(nom_fichier, "w");
    if (fichier == NULL) {
        printf("Erreur: Impossible de créer le fichier %s\n", nom_fichier);
        return;
    }
    
    /* Écrire le plateau dans le fichier */
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            char c;
            switch (plateau->cases[i][j]) {
                case NOIR:  c = 'N'; break;
                case BLANC: c = 'B'; break;
                default:    c = '.'; break;
            }
            fprintf(fichier, "%c", c);
        }
        fprintf(fichier, "\n");
    }
    
    /* Fermer le fichier */
    fclose(fichier);
}

/*
 * Écrit les statistiques de la partie dans un fichier
 */
void ecrire_statistiques(int nb_coups_joues, int score_noir, int score_blanc) {
    FILE *fichier;
    
    /* Ouvrir le fichier en écriture */
    fichier = fopen("statistiques_partie.txt", "w");
    if (fichier == NULL) {
        printf("Erreur: Impossible de créer le fichier statistiques_partie.txt\n");
        return;
    }
    
    /* Écrire les statistiques */
    fprintf(fichier, "Nombre total de coups joués: %d\n", nb_coups_joues);
    fprintf(fichier, "Score final: NOIR %d - %d BLANC\n", score_noir, score_blanc);
    if (score_noir > score_blanc) {
        fprintf(fichier, "Vainqueur: NOIR\n");
    } else if (score_blanc > score_noir) {
        fprintf(fichier, "Vainqueur: BLANC\n");
    } else {
        fprintf(fichier, "Match nul\n");
    }
    
    /* Fermer le fichier */
    fclose(fichier);
}