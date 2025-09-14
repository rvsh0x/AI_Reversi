/**
 * test_memoire.c - Comparaison de la consommation mémoire entre Alpha-Beta avec arbre
 * et Alpha-Beta sans arbre explicite
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include "../include/ia.h"
#include "../include/reversi.h"
#include "../include/evaluation.h"
#include "../include/affichage.h"

/* Types pour les phases de jeu */
typedef enum {
    DEBUT,
    MILIEU,
    FIN
} PhaseJeu;

#define MIN_PROFONDEUR 1
#define MAX_PROFONDEUR 7
#define MAX_PLATEAUX 30
#define REPERTOIRE_PLATEAUX "plateaux"

/* Fonction pour enregistrer les résultats des phases spécifiques */
void enregistrer_resultats_phase_csv(PhaseJeu phase, int profondeur, 
                                   size_t memoire_arbre_courante, 
                                   size_t memoire_optimisee_max,
                                   float reduction_pourcent) {
    FILE *fichier;
    char nom_fichier[60];
    
    switch(phase) {
        case DEBUT: strcpy(nom_fichier, "resultats_memoire_debut.csv"); break;
        case MILIEU: strcpy(nom_fichier, "resultats_memoire_milieu.csv"); break;
        case FIN: strcpy(nom_fichier, "resultats_memoire_fin.csv"); break;
    }
    
    fichier = fopen(nom_fichier, "a");
    if (fichier == NULL) {
        printf("Erreur : Impossible d'ouvrir le fichier %s.\n", nom_fichier);
        return;
    }
    
    fprintf(fichier, "%d,%.2f,%.2f,%.2f\n", profondeur, 
            (double)memoire_arbre_courante / 1024.0, 
            (double)memoire_optimisee_max / 1024.0, 
            reduction_pourcent);
    
    fclose(fichier);
}

/* Fonction pour initialiser les fichiers CSV */
void initialiser_fichier_phase_csv(PhaseJeu phase) {
    FILE *fichier;
    char nom_fichier[60];
    
    switch(phase) {
        case DEBUT: strcpy(nom_fichier, "resultats_memoire_debut.csv"); break;
        case MILIEU: strcpy(nom_fichier, "resultats_memoire_milieu.csv"); break;
        case FIN: strcpy(nom_fichier, "resultats_memoire_fin.csv"); break;
    }
    
    fichier = fopen(nom_fichier, "w");
    if (fichier != NULL) {
        fprintf(fichier, "Profondeur,Memoire_Arbre_Ko,Memoire_Optimisee_Ko,Reduction_Pourcent\n");
        fclose(fichier);
    } else {
        printf("Erreur : Impossible de créer le fichier %s.\n", nom_fichier);
    }
}

/* Fonction pour initialiser le fichier CSV des plateaux */
void initialiser_fichier_partie_csv() {
    FILE *fichier = fopen("resultats_memoire_partie.csv", "w");
    if (fichier != NULL) {
        fprintf(fichier, "Profondeur,Memoire_Arbre_Moyenne_Ko,Memoire_Optimisee_Moyenne_Ko,"
               "Memoire_Arbre_Totale_Ko,Memoire_Optimisee_Totale_Ko,Reduction_Pourcent\n");
        fclose(fichier);
    } else {
        printf("Erreur : Impossible de créer le fichier CSV.\n");
    }
}

/* Fonction pour initialiser un plateau de début de partie */
void initialiser_plateau_debut(Plateau *plateau) {
    int i, j;
    
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            plateau->cases[i][j] = VIDE;
        }
    }
    
    plateau->cases[2][2] = BLANC;
    plateau->cases[2][3] = BLANC;
    plateau->cases[2][4] = BLANC;
    plateau->cases[3][2] = NOIR;
    plateau->cases[3][3] = NOIR;
    plateau->cases[3][4] = BLANC;
    plateau->cases[4][3] = NOIR;
    plateau->cases[4][4] = NOIR;
    plateau->cases[5][5] = NOIR;
    
    plateau->ordinateur = NOIR;
    plateau->joueur_humain = BLANC;
}

/* Fonction pour initialiser un plateau de milieu de partie */
void initialiser_plateau_milieu(Plateau *plateau) {
    int i, j;
    
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            plateau->cases[i][j] = VIDE;
        }
    }
    
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
    
    plateau->ordinateur = NOIR;
    plateau->joueur_humain = BLANC;
}

/* Fonction pour initialiser un plateau de fin de partie */
void initialiser_plateau_fin(Plateau *plateau) {
    int i, j;
    
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            plateau->cases[i][j] = VIDE;
        }
    }
    
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
    
    plateau->ordinateur = NOIR;
    plateau->joueur_humain = BLANC;
}

/* Fonction qui renvoie le nom de la phase pour l'affichage */
const char* nom_phase(PhaseJeu phase) {
    switch(phase) {
        case DEBUT: return "début";
        case MILIEU: return "milieu";
        case FIN: return "fin";
        default: return "inconnue";
    }
}

/* Fonction pour tester une phase spécifique */
void tester_phase_specifique(PhaseJeu phase) {
    Plateau plateau;
    arbre arbre_jeu;
    Position coup_arbre, coup_optimise;
    int profondeur;
    float reduction_pourcent;
    int profondeur_max = 7;
    size_t memoire_arbre_courante, memoire_optimisee_max;
    
    initialiser_fichier_phase_csv(phase);
    
    printf("\n=== Test de consommation mémoire pour la phase de %s ===\n", nom_phase(phase));
    
    for (profondeur = 1; profondeur <= profondeur_max; profondeur++) {
        printf("\n--- Profondeur = %d ---\n", profondeur);
        
        /* Initialiser le plateau selon la phase */
        switch(phase) {
            case DEBUT: initialiser_plateau_debut(&plateau); break;
            case MILIEU: initialiser_plateau_milieu(&plateau); break;
            case FIN: initialiser_plateau_fin(&plateau); break;
        }
        
        /* Test avec Alpha-Beta avec arbre */
        printf("Test Alpha-Beta avec arbre...\n");
        reinitialiser_memoire_arbre_courant();
        
        arbre_jeu = creer_arbre_position(&plateau, NOIR, profondeur);
        memoire_arbre_courante = memoire_arbre_courant;
        
        coup_arbre = choisir_meilleur_coup_alphabeta(arbre_jeu, profondeur);
        printf("Meilleur coup avec arbre: (%d,%d)\n", coup_arbre.ligne, coup_arbre.colonne);
        
        liberer_arbre(arbre_jeu);
        
        /* Test avec Alpha-Beta optimisé en mémoire */
        printf("Test Alpha-Beta avec optimisation mémoire...\n");
        
        switch(phase) {
            case DEBUT: initialiser_plateau_debut(&plateau); break;
            case MILIEU: initialiser_plateau_milieu(&plateau); break;
            case FIN: initialiser_plateau_fin(&plateau); break;
        }
        
        /* Réinitialiser les compteurs de mémoire pour l'approche optimisée */
        memoire_courante_optimisee = 0;
        memoire_max_optimisee = 0;
        memoire_totale_optimisee = 0;
        
        /* Utiliser la fonction qui choisit le meilleur coup */
        coup_optimise = alpha_beta_memoire_optimisee(&plateau, NOIR, profondeur);
        printf("Meilleur coup optimisé: (%d,%d)\n", coup_optimise.ligne, coup_optimise.colonne);
        
        /* Récupérer la mémoire maximale utilisée */
        memoire_optimisee_max = memoire_max_optimisee;
        
        printf("Mémoire utilisée avec arbre: %lu octets (%.2f Ko)\n", 
               (unsigned long)memoire_arbre_courante, (double)memoire_arbre_courante / 1024.0);
        printf("Mémoire utilisée avec optimisation: %lu octets (%.2f Ko)\n", 
               (unsigned long)memoire_optimisee_max, (double)memoire_optimisee_max / 1024.0);
        
        reduction_pourcent = 100.0f * (1.0f - (float)memoire_optimisee_max / (float)memoire_arbre_courante);
        printf("Réduction de mémoire: %.2f%%\n", reduction_pourcent);
        
        if (coup_arbre.ligne == coup_optimise.ligne && coup_arbre.colonne == coup_optimise.colonne) {
            printf("Les deux approches ont donné le même coup.\n");
        } else {
            printf("ATTENTION: Coups différents!\n");
        }
        
        enregistrer_resultats_phase_csv(phase, profondeur, memoire_arbre_courante, 
                                      memoire_optimisee_max, reduction_pourcent);
    }
}

/* Fonction pour charger un plateau à partir d'un fichier */
void charger_plateau(const char *nom_fichier, Plateau *plateau) {
    FILE *fichier;
    int i, j;
    char c;
    
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            plateau->cases[i][j] = VIDE;
        }
    }
    
    plateau->ordinateur = NOIR;
    plateau->joueur_humain = BLANC;
    
    fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        printf("Erreur: Impossible d'ouvrir le fichier %s\n", nom_fichier);
        return;
    }
    
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            c = fgetc(fichier);
            switch (c) {
                case 'N': plateau->cases[i][j] = NOIR; break;
                case 'B': plateau->cases[i][j] = BLANC; break;
                default:  plateau->cases[i][j] = VIDE; break;
            }
        }
        while ((c = fgetc(fichier)) != '\n' && c != EOF);
    }
    
    fclose(fichier);
}

/* Fonction pour tester sur une partie complète (30 plateaux) */
void tester_partie_complete() {
    DIR *repertoire;
    struct dirent *entree;
    char **noms_fichiers;
    char chemin_fichier[256];
    int nb_fichiers = 0;
    int i, profondeur;
    size_t memoire_arbre_total[MAX_PROFONDEUR+1];
    size_t memoire_optimisee_total[MAX_PROFONDEUR+1];
    int nb_plateaux[MAX_PROFONDEUR+1];
    double mem_arbre_moy, mem_optimisee_moy, mem_arbre_totale, mem_optimisee_totale, reduction;
    Plateau plateau;
    arbre arbre_jeu;
    size_t memoire_arbre_courante, memoire_optimisee_max;
    Position coup_optimise;
    FILE *fichier;
    
    /* Initialiser les tableaux */
    for (i = 0; i <= MAX_PROFONDEUR; i++) {
        memoire_arbre_total[i] = 0;
        memoire_optimisee_total[i] = 0;
        nb_plateaux[i] = 0;
    }
    
    printf("\n=== Test sur une partie complète (30 plateaux max) ===\n");
    initialiser_fichier_partie_csv();
    
    noms_fichiers = (char**)malloc(MAX_PLATEAUX * sizeof(char*));
    for (i = 0; i < MAX_PLATEAUX; i++) {
        noms_fichiers[i] = (char*)malloc(100 * sizeof(char));
    }
    
    repertoire = opendir(REPERTOIRE_PLATEAUX);
    if (repertoire != NULL) {
        while ((entree = readdir(repertoire)) != NULL && nb_fichiers < MAX_PLATEAUX) {
            if (strstr(entree->d_name, "plateau_test_") && strstr(entree->d_name, ".txt")) {
                strcpy(noms_fichiers[nb_fichiers], entree->d_name);
                nb_fichiers++;
            }
        }
        closedir(repertoire);
    } else {
        printf("Impossible d'ouvrir le répertoire %s\n", REPERTOIRE_PLATEAUX);
        for (i = 0; i < MAX_PLATEAUX; i++) {
            free(noms_fichiers[i]);
        }
        free(noms_fichiers);
        return;
    }
    
    if (nb_fichiers == 0) {
        printf("Aucun plateau trouvé\n");
        for (i = 0; i < MAX_PLATEAUX; i++) {
            free(noms_fichiers[i]);
        }
        free(noms_fichiers);
        return;
    }
    
    printf("Nombre de plateaux trouvés: %d\n", nb_fichiers);
    
    /* Trier les noms de fichiers */
    for (i = 0; i < nb_fichiers - 1; i++) {
        int j;
        for (j = 0; j < nb_fichiers - i - 1; j++) {
            if (strcmp(noms_fichiers[j], noms_fichiers[j + 1]) > 0) {
                char temp[100];
                strcpy(temp, noms_fichiers[j]);
                strcpy(noms_fichiers[j], noms_fichiers[j + 1]);
                strcpy(noms_fichiers[j + 1], temp);
            }
        }
    }
    
    /* Pour chaque profondeur, tester tous les plateaux */
    for (profondeur = MIN_PROFONDEUR; profondeur <= MAX_PROFONDEUR; profondeur++) {
        printf("\n--- Profondeur %d ---\n", profondeur);
        
        for (i = 0; i < nb_fichiers; i++) {
            snprintf(chemin_fichier, sizeof(chemin_fichier), "%s/%s", 
                     REPERTOIRE_PLATEAUX, noms_fichiers[i]);
            
            charger_plateau(chemin_fichier, &plateau);
            printf("Plateau %d/%d: %s\n", i+1, nb_fichiers, noms_fichiers[i]);
            
            /* Test avec arbre */
            reinitialiser_memoire_arbre_courant();
            arbre_jeu = creer_arbre_position(&plateau, NOIR, profondeur);
            memoire_arbre_courante = memoire_arbre_courant;
            choisir_meilleur_coup_alphabeta(arbre_jeu, profondeur);
            liberer_arbre(arbre_jeu);
            
            /* Test sans arbre */
            memoire_courante_optimisee = 0;
            memoire_max_optimisee = 0;
            memoire_totale_optimisee = 0;
            
            coup_optimise = alpha_beta_memoire_optimisee(&plateau, NOIR, profondeur);
            printf("Meilleur coup optimisé: (%d,%d)\n", coup_optimise.ligne, coup_optimise.colonne);
            memoire_optimisee_max = memoire_max_optimisee;
            
            printf("  Mémoire arbre: %lu octets, Mémoire optimisée: %lu octets\n", 
                   (unsigned long)memoire_arbre_courante, (unsigned long)memoire_optimisee_max);
            
            /* Accumuler les totaux */
            memoire_arbre_total[profondeur] += memoire_arbre_courante;
            memoire_optimisee_total[profondeur] += memoire_optimisee_max;
            nb_plateaux[profondeur]++;
        }
        
        /* Calculer les moyennes */
        mem_arbre_moy = (double)memoire_arbre_total[profondeur] / nb_plateaux[profondeur];
        mem_optimisee_moy = (double)memoire_optimisee_total[profondeur] / nb_plateaux[profondeur];
        mem_arbre_totale = (double)memoire_arbre_total[profondeur];
        mem_optimisee_totale = (double)memoire_optimisee_total[profondeur];
        reduction = 100.0 * (1.0 - mem_optimisee_moy / mem_arbre_moy);
        
        printf("\nRésultats pour profondeur %d:\n", profondeur);
        printf("  Mémoire arbre moyenne: %lu octets (%.2f Ko)\n", 
               (unsigned long)mem_arbre_moy, mem_arbre_moy / 1024.0);
        printf("  Mémoire optimisée moyenne: %lu octets (%.2f Ko)\n", 
               (unsigned long)mem_optimisee_moy, mem_optimisee_moy / 1024.0);
        printf("  Mémoire arbre totale: %lu octets (%.2f Ko)\n", 
               (unsigned long)memoire_arbre_total[profondeur], mem_arbre_totale / 1024.0);
        printf("  Mémoire optimisée totale: %lu octets (%.2f Ko)\n", 
               (unsigned long)memoire_optimisee_total[profondeur], mem_optimisee_totale / 1024.0);
        printf("  Réduction moyenne: %.2f%%\n", reduction);
        
        /* Enregistrer dans le CSV */
        fichier = fopen("resultats_memoire_partie.csv", "a");
        if (fichier != NULL) {
            fprintf(fichier, "%d,%.2f,%.2f,%.2f,%.2f,%.2f\n", 
                    profondeur, 
                    mem_arbre_moy / 1024.0, 
                    mem_optimisee_moy / 1024.0,
                    mem_arbre_totale / 1024.0, 
                    mem_optimisee_totale / 1024.0, 
                    reduction);
            fclose(fichier);
        }
    }
    
    /* Libérer la mémoire */
    for (i = 0; i < MAX_PLATEAUX; i++) {
        free(noms_fichiers[i]);
    }
    free(noms_fichiers);
}

/* Fonction principale */
int main() {
    int choix;
    int scanf_result;
    
    printf("=== Comparaison de la consommation mémoire Alpha-Beta avec/sans arbre ===\n");
    
    do {
        printf("\nMenu de tests:\n");
        printf("1. Tester les phases spécifiques (début, milieu, fin)\n");
        printf("2. Tester sur une partie complète (30 plateaux)\n");
        printf("0. Quitter\n");
        printf("Votre choix: ");
        
        scanf_result = scanf("%d", &choix);
        if (scanf_result != 1) {
            printf("Saisie invalide\n");
            while (getchar() != '\n'); /* Vider le buffer d'entrée */
            choix = -1;
            continue;
        }
        
        switch (choix) {
            case 1:
                printf("\nTest sur les 3 phases du jeu...\n");
                tester_phase_specifique(DEBUT);
                tester_phase_specifique(MILIEU);
                tester_phase_specifique(FIN);
                break;
                
            case 2:
                tester_partie_complete();
                break;
                
            case 0:
                printf("Au revoir!\n");
                break;
                
            default:
                printf("Choix invalide\n");
        }
    } while (choix != 0);
    
    exit(EXIT_SUCCESS);
}