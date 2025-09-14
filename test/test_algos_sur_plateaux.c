/* 
 * test_algos_optimises_sur_plateaux.c
 * Programme qui compare les performances des algorithmes optimisés en mémoire
 * sur les plateaux sauvegardés avec plusieurs profondeurs de recherche.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include "../include/reversi.h"
#include "../include/ia.h"
#include "../include/evaluation.h"
#include "../include/nega_memoire.h"

/* Constantes globales */
#define MIN_PROFONDEUR 1   /* Profondeur minimale de recherche utilisée pour les tests */
#define MAX_PROFONDEUR 8   /* Profondeur maximale de recherche utilisée pour les tests */
#define MAX_PLATEAUX 30    /* Nombre maximal de plateaux à tester */
#define REPERTOIRE_PLATEAUX "plateaux" /* Répertoire où sont stockés les plateaux */

/* Structure pour les résultats des tests */
typedef struct {
    int nb_noeuds;          /* Nombre de nœuds explorés */
    double temps_reflexion; /* Temps de réflexion utilisé */
    Position coup_choisi;   /* Le coup choisi par l'algorithme */
} ResultatAlgo;

/* Prototypes de fonctions */
void charger_plateau(const char *nom_fichier, Plateau *plateau);
void tester_plateau(const char *chemin_fichier, const char *nom_fichier, int numero_plateau);
void comparer_sur_plateau(Plateau *plateau, int numero_plateau, int profondeur);
void ecrire_resultats_csv(int numero_plateau, 
                        int profondeur,
                        ResultatAlgo res_alpha_beta, 
                        ResultatAlgo res_negascout_sans_tri,
                        ResultatAlgo res_negascout_tri_statique,
                        ResultatAlgo res_negascout_tri_dynamique);
void initialiser_csv();
void afficher_statistiques_globales(int profondeur);

/* Variables globales pour les statistiques cumulées (par profondeur) */
int total_noeuds_alpha_beta[MAX_PROFONDEUR+1] = {0};
int total_noeuds_negascout_sans_tri[MAX_PROFONDEUR+1] = {0};
int total_noeuds_negascout_tri_statique[MAX_PROFONDEUR+1] = {0};
int total_noeuds_negascout_tri_dynamique[MAX_PROFONDEUR+1] = {0};
double total_temps_alpha_beta[MAX_PROFONDEUR+1] = {0.0};
double total_temps_negascout_sans_tri[MAX_PROFONDEUR+1] = {0.0};
double total_temps_negascout_tri_statique[MAX_PROFONDEUR+1] = {0.0};
double total_temps_negascout_tri_dynamique[MAX_PROFONDEUR+1] = {0.0};
int nb_plateaux_testes[MAX_PROFONDEUR+1] = {0};

/* Variables pour vérifier la cohérence des coups choisis */
int total_coups_identiques_ns[MAX_PROFONDEUR+1] = {0};
int total_coups_identiques_nss[MAX_PROFONDEUR+1] = {0};
int total_coups_identiques_nsd[MAX_PROFONDEUR+1] = {0};

/*
 * Fonction principale
 */
int main() {
    DIR *repertoire;
    struct dirent *entree;
    char **noms_fichiers;
    char chemin_fichier[256], tmp[100];
    int nb_fichiers = 0;
    int i, profondeur;
    
    printf("=== TEST DES ALGORITHMES OPTIMISÉS SUR PLATEAUX SAUVEGARDÉS (PROFONDEURS %d à %d) ===\n\n",
           MIN_PROFONDEUR, MAX_PROFONDEUR);
    
    /* Initialiser le fichier CSV des résultats */
    initialiser_csv();
    
    /* Allouer de la mémoire pour les noms de fichiers */
    noms_fichiers = (char**)malloc(MAX_PLATEAUX * sizeof(char*));
    for (i = 0; i < MAX_PLATEAUX; i++) {
        noms_fichiers[i] = (char*)malloc(100 * sizeof(char));
    }
    
    /* Trouver tous les fichiers plateau_test_*.txt dans le répertoire spécifié */
    repertoire = opendir(REPERTOIRE_PLATEAUX);
    if (repertoire != NULL) {
        while ((entree = readdir(repertoire)) != NULL && nb_fichiers < MAX_PLATEAUX) {
            if (strstr(entree->d_name, "plateau_test_") && 
                strstr(entree->d_name, ".txt")) {
                strcpy(noms_fichiers[nb_fichiers], entree->d_name);
                nb_fichiers++;
            }
        }
        closedir(repertoire);
    } else {
        printf("Erreur: Impossible d'ouvrir le répertoire %s\n", REPERTOIRE_PLATEAUX);
        return EXIT_FAILURE;
    }
    
    /* Si aucun fichier trouvé, quitter */
    if (nb_fichiers == 0) {
        printf("Aucun fichier de plateau trouvé dans %s\n", REPERTOIRE_PLATEAUX);
        
        /* Libérer la mémoire allouée */
        for (i = 0; i < MAX_PLATEAUX; i++) {
            free(noms_fichiers[i]);
        }
        free(noms_fichiers);
        
        return EXIT_FAILURE;
    }
    
    printf("Nombre de plateaux trouvés: %d\n\n", nb_fichiers);
    
    /* Trier les noms de fichiers pour les traiter dans l'ordre */
    for (i = 0; i < nb_fichiers - 1; i++) {
        int j;
        for (j = 0; j < nb_fichiers - i - 1; j++) {
            if (strcmp(noms_fichiers[j], noms_fichiers[j + 1]) > 0) {
                strcpy(tmp, noms_fichiers[j]);
                strcpy(noms_fichiers[j], noms_fichiers[j + 1]);
                strcpy(noms_fichiers[j + 1], tmp);
            }
        }
    }
    
    /* Tester chaque plateau */
    for (i = 0; i < nb_fichiers; i++) {
        snprintf(chemin_fichier, sizeof(chemin_fichier), "%s/%s", 
                REPERTOIRE_PLATEAUX, noms_fichiers[i]);
        tester_plateau(chemin_fichier, noms_fichiers[i], i);
    }
    
    /* Afficher les statistiques globales pour chaque profondeur */
    for (profondeur = MIN_PROFONDEUR; profondeur <= MAX_PROFONDEUR; profondeur++) {
        afficher_statistiques_globales(profondeur);
    }
    
    /* Libérer la mémoire */
    for (i = 0; i < MAX_PLATEAUX; i++) {
        free(noms_fichiers[i]);
    }
    free(noms_fichiers);
    
    printf("\n=== FIN DES TESTS ===\n");
    
    return EXIT_SUCCESS;
}

/*
 * Charge un plateau à partir d'un fichier
 */
void charger_plateau(const char *nom_fichier, Plateau *plateau) {
    /* Déclaration des variables */
    FILE *fichier;
    int i, j;
    char c;
    
    /* Initialiser le plateau à vide */
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            plateau->cases[i][j] = VIDE;
        }
    }
    
    /* Définir les valeurs par défaut des joueurs */
    plateau->ordinateur = NOIR;
    plateau->joueur_humain = BLANC;
    
    /* Ouvrir le fichier */
    fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        printf("Erreur: Impossible d'ouvrir le fichier %s\n", nom_fichier);
        return;
    }
    
    /* Lire le plateau */
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            c = fgetc(fichier);
            switch (c) {
                case 'N': plateau->cases[i][j] = NOIR; break;
                case 'B': plateau->cases[i][j] = BLANC; break;
                default:  plateau->cases[i][j] = VIDE; break;
            }
        }
        /* Ignorer la fin de ligne */
        while ((c = fgetc(fichier)) != '\n' && c != EOF);
    }
    
    /* Fermer le fichier */
    fclose(fichier);
}

/*
 * Teste les algorithmes sur un plateau donné
 */
void tester_plateau(const char *chemin_fichier, const char *nom_fichier, int numero_plateau) {
    /* Déclaration des variables */
    Plateau plateau;
    int profondeur;
    
    /* Charger le plateau */
    charger_plateau(chemin_fichier, &plateau);
    
    /* Informations sur le test */
    printf("Test #%d - Plateau: %s\n", numero_plateau + 1, nom_fichier);
    
    /* Tester toutes les profondeurs demandées */
    for (profondeur = MIN_PROFONDEUR; profondeur <= MAX_PROFONDEUR; profondeur++) {
        printf("  Profondeur %d...\n", profondeur);
        /* Comparer les algorithmes sur ce plateau avec cette profondeur */
        comparer_sur_plateau(&plateau, numero_plateau + 1, profondeur);
    }
    
    printf("\n");
}

/*
 * Compare les performances des algorithmes sur un plateau avec une profondeur donnée
 */
void comparer_sur_plateau(Plateau *plateau, int numero_plateau, int profondeur) {
    /* Déclaration des variables */
    ResultatAlgo res_alpha_beta;
    ResultatAlgo res_negascout_sans_tri;
    ResultatAlgo res_negascout_tri_statique;
    ResultatAlgo res_negascout_tri_dynamique;
    clock_t debut, fin;
    Plateau plateau_copie;
    
    /* Faire une copie du plateau pour chaque algorithme */
    memcpy(&plateau_copie, plateau, sizeof(Plateau));
    
    /******* Alpha-Beta Optimisé *******/
    compteur_noeuds_memoire = 0;
    debut = clock();
    
    /* Choisir le meilleur coup */
    res_alpha_beta.coup_choisi = alpha_beta_memoire_optimisee(&plateau_copie, NOIR, profondeur);
    
    fin = clock();
    res_alpha_beta.nb_noeuds = compteur_noeuds_memoire;
    res_alpha_beta.temps_reflexion = ((double)(fin - debut)) / CLOCKS_PER_SEC;
    
    /* Faire une copie du plateau pour chaque algorithme */
    memcpy(&plateau_copie, plateau, sizeof(Plateau));
    
    /******* NegaScout Sans Tri *******/
    debut = clock();
    
    /* Choisir le meilleur coup */
    res_negascout_sans_tri.coup_choisi = negascout_memoire_optimisee_sans_tri(&plateau_copie, NOIR, profondeur);
    
    fin = clock();
    res_negascout_sans_tri.nb_noeuds = compteur_noeuds_negascout_optimise;
    res_negascout_sans_tri.temps_reflexion = ((double)(fin - debut)) / CLOCKS_PER_SEC;
    
    /* Faire une copie du plateau pour chaque algorithme */
    memcpy(&plateau_copie, plateau, sizeof(Plateau));
    
    /******* NegaScout avec Tri Statique *******/
    debut = clock();
    
    /* Choisir le meilleur coup */
    res_negascout_tri_statique.coup_choisi = negascout_memoire_optimisee_tri_statique(&plateau_copie, NOIR, profondeur);
    
    fin = clock();
    res_negascout_tri_statique.nb_noeuds = compteur_noeuds_negascout_tri_statique_optimise;
    res_negascout_tri_statique.temps_reflexion = ((double)(fin - debut)) / CLOCKS_PER_SEC;
    
    /* Faire une copie du plateau pour chaque algorithme */
    memcpy(&plateau_copie, plateau, sizeof(Plateau));
    
    /******* NegaScout avec Tri Dynamique *******/
    debut = clock();
    
    /* Choisir le meilleur coup */
    res_negascout_tri_dynamique.coup_choisi = negascout_memoire_optimisee_tri_dynamique(&plateau_copie, NOIR, profondeur);
    
    fin = clock();
    res_negascout_tri_dynamique.nb_noeuds = compteur_noeuds_negascout_tri_dynamique_optimise;
    res_negascout_tri_dynamique.temps_reflexion = ((double)(fin - debut)) / CLOCKS_PER_SEC;
    
    /* Vérifier la cohérence des coups choisis */
    if (res_alpha_beta.coup_choisi.ligne == res_negascout_sans_tri.coup_choisi.ligne && 
        res_alpha_beta.coup_choisi.colonne == res_negascout_sans_tri.coup_choisi.colonne) {
        total_coups_identiques_ns[profondeur]++;
    }
    
    if (res_alpha_beta.coup_choisi.ligne == res_negascout_tri_statique.coup_choisi.ligne && 
        res_alpha_beta.coup_choisi.colonne == res_negascout_tri_statique.coup_choisi.colonne) {
        total_coups_identiques_nss[profondeur]++;
    }
    
    if (res_alpha_beta.coup_choisi.ligne == res_negascout_tri_dynamique.coup_choisi.ligne && 
        res_alpha_beta.coup_choisi.colonne == res_negascout_tri_dynamique.coup_choisi.colonne) {
        total_coups_identiques_nsd[profondeur]++;
    }
    
    /* Mettre à jour les statistiques globales */
    total_noeuds_alpha_beta[profondeur] += res_alpha_beta.nb_noeuds;
    total_noeuds_negascout_sans_tri[profondeur] += res_negascout_sans_tri.nb_noeuds;
    total_noeuds_negascout_tri_statique[profondeur] += res_negascout_tri_statique.nb_noeuds;
    total_noeuds_negascout_tri_dynamique[profondeur] += res_negascout_tri_dynamique.nb_noeuds;
    
    total_temps_alpha_beta[profondeur] += res_alpha_beta.temps_reflexion;
    total_temps_negascout_sans_tri[profondeur] += res_negascout_sans_tri.temps_reflexion;
    total_temps_negascout_tri_statique[profondeur] += res_negascout_tri_statique.temps_reflexion;
    total_temps_negascout_tri_dynamique[profondeur] += res_negascout_tri_dynamique.temps_reflexion;
    
    nb_plateaux_testes[profondeur]++;
    
    /* Afficher les résultats */
    printf("    Alpha-Beta: %d nœuds, %.6f sec, coup: (%d,%d)\n",
           res_alpha_beta.nb_noeuds,
           res_alpha_beta.temps_reflexion,
           res_alpha_beta.coup_choisi.ligne,
           res_alpha_beta.coup_choisi.colonne);
    
    printf("    NegaScout sans tri: %d nœuds, %.6f sec, coup: (%d,%d)\n",
           res_negascout_sans_tri.nb_noeuds,
           res_negascout_sans_tri.temps_reflexion,
           res_negascout_sans_tri.coup_choisi.ligne,
           res_negascout_sans_tri.coup_choisi.colonne);
    
    printf("    NegaScout tri statique: %d nœuds, %.6f sec, coup: (%d,%d)\n",
           res_negascout_tri_statique.nb_noeuds,
           res_negascout_tri_statique.temps_reflexion,
           res_negascout_tri_statique.coup_choisi.ligne,
           res_negascout_tri_statique.coup_choisi.colonne);
    
    printf("    NegaScout tri dynamique: %d nœuds, %.6f sec, coup: (%d,%d)\n",
           res_negascout_tri_dynamique.nb_noeuds,
           res_negascout_tri_dynamique.temps_reflexion,
           res_negascout_tri_dynamique.coup_choisi.ligne,
           res_negascout_tri_dynamique.coup_choisi.colonne);
    
    /* Écrire les résultats dans le fichier CSV */
    ecrire_resultats_csv(numero_plateau, 
                        profondeur,
                        res_alpha_beta, 
                        res_negascout_sans_tri,
                        res_negascout_tri_statique,
                        res_negascout_tri_dynamique);
}

/*
 * Initialise le fichier CSV des résultats
 */
void initialiser_csv() {
    /* Déclaration des variables */
    FILE *fichier;
    
    /* Créer ou écraser le fichier */
    fichier = fopen("resultats_tests_algos_optimises.csv", "w");
    if (fichier == NULL) {
        printf("Erreur: Impossible de créer le fichier CSV\n");
        return;
    }
    
    /* Écrire l'en-tête */
    fprintf(fichier, "Plateau,Profondeur,AB_Noeuds,AB_Temps,NS_Noeuds,NS_Temps,NSS_Noeuds,NSS_Temps,NSD_Noeuds,NSD_Temps\n");
    
    /* Fermer le fichier */
    fclose(fichier);
}

/*
 * Écrit les résultats d'un test dans le fichier CSV
 */
void ecrire_resultats_csv(int numero_plateau, 
                        int profondeur,
                        ResultatAlgo res_alpha_beta, 
                        ResultatAlgo res_negascout_sans_tri,
                        ResultatAlgo res_negascout_tri_statique,
                        ResultatAlgo res_negascout_tri_dynamique) {
    /* Déclaration des variables */
    FILE *fichier;
    
    /* Ouvrir le fichier en mode ajout */
    fichier = fopen("resultats_tests_algos_optimises.csv", "a");
    if (fichier == NULL) {
        printf("Erreur: Impossible d'ouvrir le fichier CSV\n");
        return;
    }
    
    /* Écrire une ligne de résultats sans les coups */
    fprintf(fichier, "%d,%d,%d,%.6f,%d,%.6f,%d,%.6f,%d,%.6f\n",
            numero_plateau,
            profondeur,
            res_alpha_beta.nb_noeuds,
            res_alpha_beta.temps_reflexion,
            res_negascout_sans_tri.nb_noeuds,
            res_negascout_sans_tri.temps_reflexion,
            res_negascout_tri_statique.nb_noeuds,
            res_negascout_tri_statique.temps_reflexion,
            res_negascout_tri_dynamique.nb_noeuds,
            res_negascout_tri_dynamique.temps_reflexion);
    
    /* Fermer le fichier */
    fclose(fichier);
}

/*
 * Affiche les statistiques globales des tests pour une profondeur donnée
 */
void afficher_statistiques_globales(int profondeur) {
    /* Déclaration des variables */
    double noeuds_moyenne_ab, noeuds_moyenne_ns, noeuds_moyenne_nss, noeuds_moyenne_nsd;
    double temps_moyen_ab, temps_moyen_ns, temps_moyen_nss, temps_moyen_nsd;
    double ratio_noeuds_ns, ratio_noeuds_nss, ratio_noeuds_nsd;
    double speedup_ns, speedup_nss, speedup_nsd;
    double pct_coups_identiques_ns, pct_coups_identiques_nss, pct_coups_identiques_nsd;
    FILE *fichier;
    char nom_fichier[100];
    
    /* Calculer les moyennes */
    if (nb_plateaux_testes[profondeur] > 0) {
        noeuds_moyenne_ab = (double)total_noeuds_alpha_beta[profondeur] / nb_plateaux_testes[profondeur];
        noeuds_moyenne_ns = (double)total_noeuds_negascout_sans_tri[profondeur] / nb_plateaux_testes[profondeur];
        noeuds_moyenne_nss = (double)total_noeuds_negascout_tri_statique[profondeur] / nb_plateaux_testes[profondeur];
        noeuds_moyenne_nsd = (double)total_noeuds_negascout_tri_dynamique[profondeur] / nb_plateaux_testes[profondeur];
        
        temps_moyen_ab = total_temps_alpha_beta[profondeur] / nb_plateaux_testes[profondeur];
        temps_moyen_ns = total_temps_negascout_sans_tri[profondeur] / nb_plateaux_testes[profondeur];
        temps_moyen_nss = total_temps_negascout_tri_statique[profondeur] / nb_plateaux_testes[profondeur];
        temps_moyen_nsd = total_temps_negascout_tri_dynamique[profondeur] / nb_plateaux_testes[profondeur];
        
        /* Calculer les ratios de noeuds */
        ratio_noeuds_ns = noeuds_moyenne_ab > 0 ? (noeuds_moyenne_ns / noeuds_moyenne_ab) * 100 : 0;
        ratio_noeuds_nss = noeuds_moyenne_ab > 0 ? (noeuds_moyenne_nss / noeuds_moyenne_ab) * 100 : 0;
        ratio_noeuds_nsd = noeuds_moyenne_ab > 0 ? (noeuds_moyenne_nsd / noeuds_moyenne_ab) * 100 : 0;
        
        /* Calculer les speedups */
        speedup_ns = temps_moyen_ab > 0 ? temps_moyen_ab / temps_moyen_ns : 0;
        speedup_nss = temps_moyen_ab > 0 ? temps_moyen_ab / temps_moyen_nss : 0;
        speedup_nsd = temps_moyen_ab > 0 ? temps_moyen_ab / temps_moyen_nsd : 0;
        
        /* Calculer les pourcentages de coups identiques */
        pct_coups_identiques_ns = (double)total_coups_identiques_ns[profondeur] / nb_plateaux_testes[profondeur] * 100;
        pct_coups_identiques_nss = (double)total_coups_identiques_nss[profondeur] / nb_plateaux_testes[profondeur] * 100;
        pct_coups_identiques_nsd = (double)total_coups_identiques_nsd[profondeur] / nb_plateaux_testes[profondeur] * 100;
    } else {
        /* Cas où aucun plateau n'a été testé pour cette profondeur */
        noeuds_moyenne_ab = noeuds_moyenne_ns = noeuds_moyenne_nss = noeuds_moyenne_nsd = 0.0;
        temps_moyen_ab = temps_moyen_ns = temps_moyen_nss = temps_moyen_nsd = 0.0;
        ratio_noeuds_ns = ratio_noeuds_nss = ratio_noeuds_nsd = 0.0;
        speedup_ns = speedup_nss = speedup_nsd = 0.0;
        pct_coups_identiques_ns = pct_coups_identiques_nss = pct_coups_identiques_nsd = 0.0;
    }
    
    /* Afficher les statistiques pour cette profondeur */
    printf("\n=== STATISTIQUES POUR PROFONDEUR %d (%d plateaux) ===\n", 
           profondeur, nb_plateaux_testes[profondeur]);
    
    printf("Alpha-Beta Optimisé:\n");
    printf("  Nœuds moyen: %.1f\n", noeuds_moyenne_ab);
    printf("  Temps de réflexion moyen: %.6f secondes\n", temps_moyen_ab);
    
    printf("\nNegaScout sans tri optimisé:\n");
    printf("  Nœuds moyen: %.1f (%.2f%% d'Alpha-Beta)\n", 
           noeuds_moyenne_ns, ratio_noeuds_ns);
    printf("  Temps de réflexion moyen: %.6f secondes (speedup: %.2fx)\n", 
           temps_moyen_ns, speedup_ns);
    printf("  Coups identiques à Alpha-Beta: %d/%d (%.1f%%)\n",
           total_coups_identiques_ns[profondeur], nb_plateaux_testes[profondeur],
           pct_coups_identiques_ns);
    
    printf("\nNegaScout tri statique optimisé:\n");
    printf("  Nœuds moyen: %.1f (%.2f%% d'Alpha-Beta)\n", 
           noeuds_moyenne_nss, ratio_noeuds_nss);
    printf("  Temps de réflexion moyen: %.6f secondes (speedup: %.2fx)\n", 
           temps_moyen_nss, speedup_nss);
    printf("  Coups identiques à Alpha-Beta: %d/%d (%.1f%%)\n",
           total_coups_identiques_nss[profondeur], nb_plateaux_testes[profondeur],
           pct_coups_identiques_nss);
    
    printf("\nNegaScout tri dynamique optimisé:\n");
    printf("  Nœuds moyen: %.1f (%.2f%% d'Alpha-Beta)\n", 
           noeuds_moyenne_nsd, ratio_noeuds_nsd);
    printf("  Temps de réflexion moyen: %.6f secondes (speedup: %.2fx)\n", 
           temps_moyen_nsd, speedup_nsd);
    printf("  Coups identiques à Alpha-Beta: %d/%d (%.1f%%)\n",
           total_coups_identiques_nsd[profondeur], nb_plateaux_testes[profondeur],
           pct_coups_identiques_nsd);
    
    /* Créer le fichier CSV de résumé pour cette profondeur */
    sprintf(nom_fichier, "resume_tests_algos_optimises_prof_%d.csv", profondeur);
    fichier = fopen(nom_fichier, "w");
    if (fichier != NULL) {
        fprintf(fichier, "Algorithme,Noeuds_Moyen,Temps_Moyen,Ratio_Noeuds,Speedup,Coups_Identiques\n");
        fprintf(fichier, "Alpha-Beta,%.1f,%.6f,100.00,1.00,100.00\n", 
                noeuds_moyenne_ab, temps_moyen_ab);
        
        fprintf(fichier, "NegaScout_sans_tri,%.1f,%.6f,%.2f,%.2f,%.1f\n", 
                noeuds_moyenne_ns, temps_moyen_ns, ratio_noeuds_ns, speedup_ns, pct_coups_identiques_ns);
        
        fprintf(fichier, "NegaScout_tri_statique,%.1f,%.6f,%.2f,%.2f,%.1f\n", 
                noeuds_moyenne_nss, temps_moyen_nss, ratio_noeuds_nss, speedup_nss, pct_coups_identiques_nss);
        
        fprintf(fichier, "NegaScout_tri_dynamique,%.1f,%.6f,%.2f,%.2f,%.1f\n", 
                noeuds_moyenne_nsd, temps_moyen_nsd, ratio_noeuds_nsd, speedup_nsd, pct_coups_identiques_nsd);
        
        fclose(fichier);
        printf("\nRésumé enregistré dans %s\n", nom_fichier);
    }
}