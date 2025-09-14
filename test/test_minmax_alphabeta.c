/**
 * test_minimax_alphabeta.c - Test comparatif entre Minimax et Alpha-Beta
 * sur différentes phases de jeu (début, milieu, fin)
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <time.h>
 #include "../include/ia.h"
 #include "../include/reversi.h"
 #include "../include/evaluation.h"
 #include "../include/affichage.h"
 
 /* Constantes pour les phases de jeu */
 typedef enum {
     DEBUT,
     MILIEU,
     FIN
 } PhaseJeu;
 
/* Fonction pour enregistrer les résultats dans un fichier CSV */
void enregistrer_resultats_csv(PhaseJeu phase, int profondeur, int noeuds_minimax, int noeuds_alphabeta, 
                             double temps_minimax, double temps_alphabeta) {
    FILE *fichier;
    char nom_fichier[50];
    double reduction = 0.0;
    
    /* Calculer le pourcentage de réduction des nœuds */
    if (noeuds_minimax > 0) {
        reduction = 100.0 * (1.0 - (double)noeuds_alphabeta / noeuds_minimax);
    }
    
    /* Sélectionner le nom de fichier en fonction de la phase */
    switch(phase) {
        case DEBUT:
            strcpy(nom_fichier, "resultats_debut_MA.csv");
            break;
        case MILIEU:
            strcpy(nom_fichier, "resultats_milieu_MA.csv");
            break;
        case FIN:
            strcpy(nom_fichier, "resultats_fin_MA.csv");
            break;
    }
    
    fichier = fopen(nom_fichier, "a");
    if (fichier == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier %s.\n", nom_fichier);
        return;
    }
    fprintf(fichier, "%d,%d,%d,%f,%f,%.2f\n", profondeur, noeuds_minimax, noeuds_alphabeta, 
            temps_minimax, temps_alphabeta, reduction);
    fclose(fichier);
}

/* Fonction pour initialiser les fichiers CSV pour une phase donnée */
void initialiser_fichier_csv(PhaseJeu phase) {
    FILE *fichier;
    char nom_fichier[50];
    
    /* Sélectionner le nom de fichier en fonction de la phase */
    switch(phase) {
        case DEBUT:
            strcpy(nom_fichier, "resultats_debut_MA.csv");
            break;
        case MILIEU:
            strcpy(nom_fichier, "resultats_milieu_MA.csv");
            break;
        case FIN:
            strcpy(nom_fichier, "resultats_fin_MA.csv");
            break;
    }
    
    fichier = fopen(nom_fichier, "w");
    if (fichier != NULL) {
        fprintf(fichier, "Profondeur,Noeuds_Minimax,Noeuds_AlphaBeta,Temps_Minimax,Temps_AlphaBeta,Reduction_Pourcent\n");
        fclose(fichier);
    } else {
        printf("Erreur : Impossible de créer le fichier %s.\n", nom_fichier);
    }
}
 
 /* Fonction pour initialiser un plateau de début de partie */
 void initialiser_plateau_debut(Plateau *plateau) {
     int i, j;
     
     /* Initialiser le plateau vide */
     for (i = 0; i < TAILLE_PLATEAU; i++) {
         for (j = 0; j < TAILLE_PLATEAU; j++) {
             plateau->cases[i][j] = VIDE;
         }
     }
     
     /* Configuration de test pour début de partie */
     plateau->cases[2][2] = BLANC;
     plateau->cases[2][3] = BLANC;
     plateau->cases[2][4] = BLANC;
     plateau->cases[3][2] = NOIR;
     plateau->cases[3][3] = NOIR;
     plateau->cases[3][4] = BLANC;
     plateau->cases[4][3] = NOIR;
     plateau->cases[4][4] = NOIR;
     plateau->cases[5][5] = NOIR;
     
     /* Configurer le joueur ordinateur et humain */
     plateau->ordinateur = NOIR;
     plateau->joueur_humain = BLANC;
 }
 
 /* Fonction pour initialiser un plateau de milieu de partie */
 void initialiser_plateau_milieu(Plateau *plateau) {
     int i, j;
     
     /* Initialiser le plateau vide */
     for (i = 0; i < TAILLE_PLATEAU; i++) {
         for (j = 0; j < TAILLE_PLATEAU; j++) {
             plateau->cases[i][j] = VIDE;
         }
     }
     
     /* Configuration de test pour milieu de partie */
     plateau->cases[0][1] = NOIR;
     plateau->cases[0][2] = NOIR;
     plateau->cases[0][3] = NOIR;
     plateau->cases[1][3] = BLANC;
     plateau->cases[2][2] = BLANC;
     plateau->cases[2][3] = BLANC;
     plateau->cases[2][4] = BLANC;
     plateau->cases[3][1] = BLANC;
     plateau->cases[3][2] = BLANC;
     plateau->cases[3][3] = NOIR;
     plateau->cases[3][4] = NOIR;
     plateau->cases[4][1] = BLANC;
     plateau->cases[4][2] = NOIR;
     plateau->cases[4][3] = NOIR;
     plateau->cases[4][4] = NOIR;
     plateau->cases[5][2] = NOIR;
     plateau->cases[5][3] = BLANC;
     plateau->cases[5][4] = NOIR;
     plateau->cases[6][1] = NOIR;
     plateau->cases[6][2] = BLANC;
     
     /* Configurer le joueur ordinateur et humain */
     plateau->ordinateur = NOIR;
     plateau->joueur_humain = BLANC;
 }
 
 /* Fonction pour initialiser un plateau de fin de partie */
 void initialiser_plateau_fin(Plateau *plateau) {
     int i, j;
     
     /* Initialiser le plateau vide */
     for (i = 0; i < TAILLE_PLATEAU; i++) {
         for (j = 0; j < TAILLE_PLATEAU; j++) {
             plateau->cases[i][j] = VIDE;
         }
     }
     
     /* Configuration de test pour fin de partie */
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
     
     /* Configurer le joueur ordinateur et humain */
     plateau->ordinateur = NOIR;
     plateau->joueur_humain = BLANC;
 }
 
 /* Fonction pour afficher l'arbre de jeu avec sa structure */
 void afficher_arbre_simple(arbre a, int niveau, int max_niveau) {
     int i;
     char indent[100];
     
     /* Initialiser l'indentation */
     indent[0] = '\0';
     
     /* Construire l'indentation */
     for (i = 0; i < niveau; i++) {
         strcat(indent, "  ");
     }
     
     /* Afficher le nœud */
     if (niveau == 0) {
         printf("%sRacine (évaluation: %d)\n", indent, a->evaluation);
     } else {
         printf("%sCoup (%d,%d) - évaluation: %d\n", 
                indent, a->coup.ligne, a->coup.colonne, a->evaluation);
     }
     
     /* Si on a atteint la profondeur maximale ou si le nœud est une feuille, on s'arrête */
     if (niveau >= max_niveau || a->nb_enfants == 0) {
         return;
     }
     
     /* Afficher les enfants */
     for (i = 0; i < a->nb_enfants; i++) {
         afficher_arbre_simple(a->enfants[i], niveau + 1, max_niveau);
     }
 }
 
 
 /* Fonction pour obtenir le nom de la phase */
 const char* nom_phase(PhaseJeu phase) {
     switch(phase) {
         case DEBUT: return "début";
         case MILIEU: return "milieu";
         case FIN: return "fin";
         default: return "inconnue";
     }
 }
 
/* Fonction pour tester et comparer Minimax et Alpha-Beta pour une phase donnée */
void test_comparaison_minimax_alphabeta(PhaseJeu phase) {
    Plateau plateau;
    arbre arbre_jeu;
    clock_t start, end;
    double temps_minimax, temps_alphabeta;
    int profondeur;
    double reduction;
    Position coup_minimax;
    Position coup_alphabeta;
    int profondeur_max = 7;
    
    /* Initialiser le fichier CSV */
    initialiser_fichier_csv(phase);

    printf("\n=== Tests pour la phase de %s ===\n", nom_phase(phase));
    
    /* Tester pour différentes profondeurs */
    for (profondeur = 1; profondeur <= profondeur_max; profondeur++) {
        printf("\n--- Tests pour profondeur = %d ---\n", profondeur);
        
        /* Initialiser le plateau selon la phase */
        switch(phase) {
            case DEBUT:
                initialiser_plateau_debut(&plateau);
                break;
            case MILIEU:
                initialiser_plateau_milieu(&plateau);
                break;
            case FIN:
                initialiser_plateau_fin(&plateau);
                break;
        }
        
        /* Afficher le plateau */
        printf("Plateau de test (%s):\n", nom_phase(phase));
        afficher_plateau(&plateau);
        
        /* Créer l'arbre de jeu */
        printf("Création de l'arbre...\n");
        arbre_jeu = creer_arbre_position(&plateau, NOIR, profondeur);
        
        /* Test avec Minimax - directement sur choisir_meilleur_coup_etape4 */
        compteur_noeuds_minimax = 0; /* Réinitialiser le compteur */
        start = clock();
        coup_minimax = choisir_meilleur_coup_etape4(arbre_jeu, profondeur);
        end = clock();
        temps_minimax = (double)(end - start) / CLOCKS_PER_SEC;
        
        printf("Test avec Minimax:\n");
        printf("- Nombre de noeuds explorés: %d\n", compteur_noeuds_minimax);
        printf("- Temps d'exécution: %.6f secondes\n", temps_minimax);
        printf("- Meilleur coup: (%d,%d)\n", coup_minimax.ligne, coup_minimax.colonne);
        
        /* Recréer l'arbre pour le test Alpha-Beta pour éviter les interférences */
        liberer_arbre(arbre_jeu);
        arbre_jeu = creer_arbre_position(&plateau, NOIR, profondeur);
        
        /* Test avec Alpha-Beta - directement sur choisir_meilleur_coup_alphabeta */
        compteur_noeuds_alphabeta = 0; /* Réinitialiser le compteur */
        start = clock();
        coup_alphabeta = choisir_meilleur_coup_alphabeta(arbre_jeu, profondeur);
        end = clock();
        temps_alphabeta = (double)(end - start) / CLOCKS_PER_SEC;
        
        printf("Test avec Alpha-Beta:\n");
        printf("- Nombre de noeuds explorés: %d\n", compteur_noeuds_alphabeta);
        printf("- Temps d'exécution: %.6f secondes\n", temps_alphabeta);
        printf("- Meilleur coup: (%d,%d)\n", coup_alphabeta.ligne, coup_alphabeta.colonne);
        
        /* Calculer le taux de réduction des nœuds */
        if (compteur_noeuds_minimax > 0) {
            reduction = 100.0 * (1.0 - (double)compteur_noeuds_alphabeta / compteur_noeuds_minimax);
            printf("Alpha-Beta a réduit le nombre de noeuds de %.2f%%\n", reduction);
        } else {
            printf("Impossible de calculer la réduction (division par zéro)\n");
        }
        
        /* Enregistrer les résultats */
        enregistrer_resultats_csv(phase, profondeur, compteur_noeuds_minimax, compteur_noeuds_alphabeta, 
                               temps_minimax, temps_alphabeta);
        
        /* Libérer l'arbre */
        liberer_arbre(arbre_jeu);
    }
}
 /* Fonction pour visualiser la structure de l'arbre */
 void visualiser_arbre(PhaseJeu phase) {
     Plateau plateau;
     arbre arbre_jeu;
     int profondeur_max = 2; /* Profondeur limitée pour un affichage lisible */
     Position meilleur_coup;
     
     printf("\n=== Visualisation de la structure de l'arbre (phase %s) ===\n", nom_phase(phase));
     
     /* Initialiser le plateau selon la phase */
     switch(phase) {
         case DEBUT:
             initialiser_plateau_debut(&plateau);
             break;
         case MILIEU:
             initialiser_plateau_milieu(&plateau);
             break;
         case FIN:
             initialiser_plateau_fin(&plateau);
             break;
     }
     
     /* Afficher le plateau */
     printf("Plateau de test (%s):\n", nom_phase(phase));
     afficher_plateau(&plateau);
     
     /* Créer l'arbre de jeu */
     printf("Création de l'arbre...\n");
     arbre_jeu = creer_arbre_position(&plateau, NOIR, profondeur_max);
     
     /* Afficher la structure de l'arbre */
     printf("\nStructure de l'arbre (limité à 2 niveaux):\n");
     afficher_arbre_simple(arbre_jeu, 0, 2);
     
     /* Trouver le meilleur coup */
     meilleur_coup = choisir_meilleur_coup_etape4(arbre_jeu, profondeur_max);
     printf("\nMeilleur coup identifié par Minimax: (%d,%d)\n", 
            meilleur_coup.ligne, meilleur_coup.colonne);
     
     /* Libérer l'arbre */
     liberer_arbre(arbre_jeu);
 }
 
 /* Fonction principale */
 int main() {
     int choix, phase;
     int scanf_result;
     
     printf("=== Test des algorithmes Minimax et Alpha-Beta pour Reversi ===\n");
     
     do {
         printf("\nMenu principal:\n");
         printf("1. Comparer les performances sur une phase de jeu\n");
         printf("2. Visualiser la structure de l'arbre pour une phase de jeu\n");
         printf("3. Exécuter tous les tests (toutes phases)\n");
         printf("0. Quitter\n");
         printf("Votre choix: ");
         scanf_result = scanf("%d", &choix);
         
         if (scanf_result != 1) {
             /* Gestion d'erreur de saisie */
             printf("Erreur de saisie. Veuillez entrer un nombre.\n");
             while (getchar() != '\n'); /* Vider le buffer */
             choix = -1;
         }
         else {
             switch (choix) {
                 case 1:
                     /* Sous-menu pour choisir la phase */
                     printf("\nChoisir une phase de jeu:\n");
                     printf("1. Début de partie\n");
                     printf("2. Milieu de partie\n");
                     printf("3. Fin de partie\n");
                     printf("Votre choix: ");
                     scanf_result = scanf("%d", &phase);
                     
                     if (scanf_result != 1 || phase < 1 || phase > 3) {
                         printf("Choix invalide! Retour au menu principal.\n");
                     } else {
                         test_comparaison_minimax_alphabeta(phase - 1);
                     }
                     break;
                     
                 case 2:
                     /* Sous-menu pour choisir la phase */
                     printf("\nChoisir une phase de jeu:\n");
                     printf("1. Début de partie\n");
                     printf("2. Milieu de partie\n");
                     printf("3. Fin de partie\n");
                     printf("Votre choix: ");
                     scanf_result = scanf("%d", &phase);
                     
                     if (scanf_result != 1 || phase < 1 || phase > 3) {
                         printf("Choix invalide! Retour au menu principal.\n");
                     } else {
                         visualiser_arbre(phase - 1);
                     }
                     break;
                     
                 case 3:
                     /* Exécuter tous les tests */
                     printf("\n=== Exécution de tous les tests ===\n");
                     test_comparaison_minimax_alphabeta(DEBUT);
                     test_comparaison_minimax_alphabeta(MILIEU);
                     test_comparaison_minimax_alphabeta(FIN);
                     printf("\n=== Tous les tests terminés ===\n");
                     break;
                     
                 case 0:
                     printf("Au revoir!\n");
                     break;
                     
                 default:
                     printf("Choix invalide!\n");
             }
         }
     } while (choix != 0);
     
     exit(EXIT_SUCCESS);
 }