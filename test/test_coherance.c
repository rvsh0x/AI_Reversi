#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/ia.h"
#include "../include/reversi.h"
#include "../include/evaluation.h"
#include "../include/nega_memoire.h"

/* Fonction pour enregistrer les résultats dans un fichier CSV */
void enregistrer_resultats_csv(int profondeur, 
    int noeuds_negascout_tri, double temps_negascout_tri, Position coup_negascout_tri,
    int noeuds_negascout_sans_tri, double temps_negascout_sans_tri, Position coup_negascout_sans_tri,
    int noeuds_alphabeta, double temps_alphabeta, Position coup_alphabeta,
    int noeuds_negascout_dynamique, double temps_negascout_dynamique, Position coup_negascout_dynamique,
    int noeuds_alphabeta_optimise, double temps_alphabeta_optimise, Position coup_alphabeta_optimise) {
    
    FILE *fichier = fopen("resultats_tests2.csv", "a"); /* Ouvrir le fichier en mode ajout */
    if (fichier == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier.\n");
        return;
    }

    /* Format: profondeur, noeuds_algo, temps_algo, coup_ligne, coup_colonne (pour chaque algo) */
    fprintf(fichier, "%d,%d,%f,%d,%d,%d,%f,%d,%d,%d,%f,%d,%d,%d,%f,%d,%d,%d,%f,%d,%d\n", 
        profondeur, 
        noeuds_negascout_tri, temps_negascout_tri, coup_negascout_tri.ligne, coup_negascout_tri.colonne,
        noeuds_negascout_sans_tri, temps_negascout_sans_tri, coup_negascout_sans_tri.ligne, coup_negascout_sans_tri.colonne,
        noeuds_alphabeta, temps_alphabeta, coup_alphabeta.ligne, coup_alphabeta.colonne,
        noeuds_negascout_dynamique, temps_negascout_dynamique, coup_negascout_dynamique.ligne, coup_negascout_dynamique.colonne,
        noeuds_alphabeta_optimise, temps_alphabeta_optimise, coup_alphabeta_optimise.ligne, coup_alphabeta_optimise.colonne);

    fclose(fichier);
}

/* Fonction pour comparer les versions de Nega-Scout et Alpha-Bêta */
void comparer_algorithmes() {
    Plateau plateau;
    clock_t start, end;
    double temps_avec_tri, temps_sans_tri, temps_alpha_beta, temps_dynamique;
    double temps_alphabeta_optimise;
    double temps_negascout_sans_tri_optimise, temps_negascout_tri_statique_optimise;
    double temps_negascout_tri_dynamique_optimise;
    int profondeur;
    arbre arbre_jeu;
    int noeuds_avec_tri, noeuds_sans_tri, noeuds_alpha_beta, noeuds_dynamique;
    int noeuds_alphabeta_optimise;
    Position coup_avec_tri, coup_sans_tri, coup_alpha_beta, coup_dynamique;
    Position coup_alphabeta_optimise;
    Position coup_negascout_sans_tri_optimise, coup_negascout_tri_statique_optimise;
    Position coup_negascout_tri_dynamique_optimise;
    Joueur joueur_test = BLANC; 

    /* Initialiser un plateau de test avec une configuration spécifique */
    initialiser_plateau(&plateau);
    plateau.ordinateur = BLANC;  
    plateau.joueur_humain = NOIR;  
    
    plateau.cases[2][2] = BLANC;
    plateau.cases[2][3] = BLANC;
    plateau.cases[2][4] = BLANC;
    plateau.cases[3][2] = NOIR;
    plateau.cases[3][3] = NOIR;
    plateau.cases[3][4] = BLANC;
    plateau.cases[4][3] = NOIR;
    plateau.cases[4][4] = NOIR;
    plateau.cases[5][5] = NOIR;

    /* Tester pour les profondeurs de 1 à 5 (pour éviter des temps trop longs) */
    for (profondeur = 1; profondeur <= 7; profondeur++) {
        printf("\n--- Comparaison pour profondeur = %d ---\n", profondeur);

        /* Test avec tri dynamique des coups (Nega-Scout) */
        compteur_noeuds_negascout = 0;
        arbre_jeu = creer_arbre_position(&plateau, joueur_test, profondeur);
        start = clock();
        coup_dynamique = choisir_meilleur_coup_dynamique(arbre_jeu, profondeur);
        end = clock();
        temps_dynamique = (double)(end - start) / CLOCKS_PER_SEC;
        noeuds_dynamique = compteur_noeuds_negascout;
        printf("Avec ordonnancement dynamique (Nega-Scout) :\n");
        printf("Meilleur coup : (%d,%d)\n", coup_dynamique.ligne, coup_dynamique.colonne);
        printf("Nombre de nœuds explorés : %d\n", noeuds_dynamique);
        printf("Temps d'exécution : %f secondes\n", temps_dynamique);
        liberer_arbre(arbre_jeu);

        /* Test avec tri statique des coups (Nega-Scout) */
        compteur_noeuds_negascout = 0;
        arbre_jeu = creer_arbre_position(&plateau, joueur_test, profondeur);
        start = clock();
        coup_avec_tri = choisir_meilleur_coup_avec_tri(arbre_jeu, profondeur);
        end = clock();
        temps_avec_tri = (double)(end - start) / CLOCKS_PER_SEC;
        noeuds_avec_tri = compteur_noeuds_negascout;
        printf("Avec tri statique des coups (Nega-Scout) :\n");
        printf("Meilleur coup : (%d,%d)\n", coup_avec_tri.ligne, coup_avec_tri.colonne);
        printf("Nombre de nœuds explorés : %d\n", noeuds_avec_tri);
        printf("Temps d'exécution : %f secondes\n", temps_avec_tri);
        liberer_arbre(arbre_jeu);

        /* Test sans tri des coups (Nega-Scout) */
        compteur_noeuds_negascout = 0;
        arbre_jeu = creer_arbre_position(&plateau, joueur_test, profondeur);
        start = clock();
        coup_sans_tri = choisir_meilleur_coup_sans_tri(arbre_jeu, profondeur);
        end = clock();
        temps_sans_tri = (double)(end - start) / CLOCKS_PER_SEC;
        noeuds_sans_tri = compteur_noeuds_negascout;
        printf("Sans tri des coups (Nega-Scout) :\n");
        printf("Meilleur coup : (%d,%d)\n", coup_sans_tri.ligne, coup_sans_tri.colonne);
        printf("Nombre de nœuds explorés : %d\n", noeuds_sans_tri);
        printf("Temps d'exécution : %f secondes\n", temps_sans_tri);
        liberer_arbre(arbre_jeu);

        /* Test avec Alpha-Bêta classique */
        compteur_noeuds_alphabeta = 0;
        arbre_jeu = creer_arbre_position(&plateau, joueur_test, profondeur);
        start = clock();
        coup_alpha_beta = choisir_meilleur_coup_alphabeta(arbre_jeu, profondeur);
        end = clock();
        temps_alpha_beta = (double)(end - start) / CLOCKS_PER_SEC;
        noeuds_alpha_beta = compteur_noeuds_alphabeta;
        printf("Avec Alpha-Bêta classique :\n");
        printf("Meilleur coup : (%d,%d)\n", coup_alpha_beta.ligne, coup_alpha_beta.colonne);
        printf("Nombre de nœuds explorés : %d\n", noeuds_alpha_beta);
        printf("Temps d'exécution : %f secondes\n", temps_alpha_beta);
        liberer_arbre(arbre_jeu);
        
        /* Test avec Alpha-Beta optimisé (mémoire) */
        compteur_noeuds_memoire = 0;
        start = clock();
        coup_alphabeta_optimise = alpha_beta_memoire_optimisee(&plateau, joueur_test, profondeur);
        end = clock();
        temps_alphabeta_optimise = (double)(end - start) / CLOCKS_PER_SEC;
        noeuds_alphabeta_optimise = compteur_noeuds_memoire;
        printf("Avec Alpha-Bêta optimisé mémoire :\n");
        printf("Meilleur coup : (%d,%d)\n", coup_alphabeta_optimise.ligne, coup_alphabeta_optimise.colonne);
        printf("Nombre de nœuds explorés : %d\n", noeuds_alphabeta_optimise);
        printf("Temps d'exécution : %f secondes\n", temps_alphabeta_optimise);

        /* Test avec NegaScout optimisé sans tri */
        compteur_noeuds_negascout_optimise = 0;
        start = clock();
        coup_negascout_sans_tri_optimise = negascout_memoire_optimisee_sans_tri(&plateau, joueur_test, profondeur);
        end = clock();
        temps_negascout_sans_tri_optimise = (double)(end - start) / CLOCKS_PER_SEC;
        printf("Avec NegaScout optimisé sans tri :\n");
        printf("Meilleur coup : (%d,%d)\n", coup_negascout_sans_tri_optimise.ligne, coup_negascout_sans_tri_optimise.colonne);
        printf("Nombre de nœuds explorés : %d\n", compteur_noeuds_negascout_optimise);
        printf("Temps d'exécution : %f secondes\n", temps_negascout_sans_tri_optimise);

        /* Test avec NegaScout optimisé avec tri statique */
        compteur_noeuds_negascout_tri_statique_optimise = 0;
        start = clock();
        coup_negascout_tri_statique_optimise = negascout_memoire_optimisee_tri_statique(&plateau, joueur_test, profondeur);
        end = clock();
        temps_negascout_tri_statique_optimise = (double)(end - start) / CLOCKS_PER_SEC;
        printf("Avec NegaScout optimisé avec tri statique :\n");
        printf("Meilleur coup : (%d,%d)\n", coup_negascout_tri_statique_optimise.ligne, coup_negascout_tri_statique_optimise.colonne);
        printf("Nombre de nœuds explorés : %d\n", compteur_noeuds_negascout_tri_statique_optimise);
        printf("Temps d'exécution : %f secondes\n", temps_negascout_tri_statique_optimise);

        /* Test avec NegaScout optimisé avec tri dynamique */
        compteur_noeuds_negascout_tri_dynamique_optimise = 0;
        start = clock();
        coup_negascout_tri_dynamique_optimise = negascout_memoire_optimisee_tri_dynamique(&plateau, joueur_test, profondeur);
        end = clock();
        temps_negascout_tri_dynamique_optimise = (double)(end - start) / CLOCKS_PER_SEC;
        printf("Avec NegaScout optimisé avec tri dynamique :\n");
        printf("Meilleur coup : (%d,%d)\n", coup_negascout_tri_dynamique_optimise.ligne, coup_negascout_tri_dynamique_optimise.colonne);
        printf("Nombre de nœuds explorés : %d\n", compteur_noeuds_negascout_tri_dynamique_optimise);
        printf("Temps d'exécution : %f secondes\n", temps_negascout_tri_dynamique_optimise);
        
        /* Comparer les coups choisis */
        printf("\nRésultats - Meilleurs coups:\n");
        printf("Alpha-Beta standard  : (%d,%d)\n", coup_alpha_beta.ligne, coup_alpha_beta.colonne);
        printf("Alpha-Beta optimisé  : (%d,%d)\n", coup_alphabeta_optimise.ligne, coup_alphabeta_optimise.colonne);
        printf("NegaScout sans tri   : (%d,%d)\n", coup_sans_tri.ligne, coup_sans_tri.colonne);
        printf("NegaScout tri stat   : (%d,%d)\n", coup_avec_tri.ligne, coup_avec_tri.colonne);
        printf("NegaScout tri dyn    : (%d,%d)\n", coup_dynamique.ligne, coup_dynamique.colonne);
        printf("NegaScout opt sans tri: (%d,%d)\n", coup_negascout_sans_tri_optimise.ligne, coup_negascout_sans_tri_optimise.colonne);
        printf("NegaScout opt tri stat: (%d,%d)\n", coup_negascout_tri_statique_optimise.ligne, coup_negascout_tri_statique_optimise.colonne);
        printf("NegaScout opt tri dyn : (%d,%d)\n", coup_negascout_tri_dynamique_optimise.ligne, coup_negascout_tri_dynamique_optimise.colonne);
    }
}

int main() {
    printf("=== Début des tests de comparaison des algorithmes ===\n");

    /* Comparer les versions de Nega-Scout et Alpha-Bêta */
    comparer_algorithmes();

    printf("\n=== Fin des tests de comparaison des algorithmes ===\n");
    return 0;
}