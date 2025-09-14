#ifndef _IA_C_
#define _IA_C_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/ia.h"
#include "../include/evaluation.h"
/* compteur globaux : */

int compteur_noeuds_minimax = 0;
int compteur_noeuds_alphabeta = 0;
int compteur_noeuds_negascout = 0; 
size_t memoire_totale_arbre = 0;
size_t memoire_arbre_courant = 0;


void reinitialiser_memoire_arbre_courant() {
    memoire_arbre_courant = 0;
}
/* Fonction pour afficher la mémoire de l'arbre courant */
void afficher_memoire_arbre_courant() {
    printf("Mémoire utilisée par l'arbre courant: %lu octets (%.2f Ko)\n",
           (unsigned long)memoire_arbre_courant, memoire_arbre_courant / 1024.0);
}

/* Fonction pour créer un nœud de l'arbre */
arbre creer_noeud(Plateau *plateau, Position coup) {
    arbre noeud;
    int i, j;

    noeud = (arbre)malloc(sizeof(Noeud));
    if (noeud == NULL) {
        printf("Erreur : allocation mémoire échouée pour le nœud.\n");
        exit(EXIT_FAILURE);
    }
    /* Mise à jour des compteurs de mémoire */
    memoire_totale_arbre += sizeof(Noeud);
    memoire_arbre_courant += sizeof(Noeud);

    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            noeud->plateau.cases[i][j] = plateau->cases[i][j];
        }
    }

    noeud->plateau.joueur_humain = plateau->joueur_humain;
    noeud->plateau.ordinateur = plateau->ordinateur;
    noeud->coup = coup;
    noeud->enfants = NULL;
    noeud->nb_enfants = 0;
    noeud->evaluation = 0;

    return noeud;
}

/* Fonction pour libérer la mémoire allouée pour un arbre */
void liberer_arbre(arbre a) {
    int i;
    if (a == NULL) {
        return;
    }

    for (i = 0; i < a->nb_enfants; i++) {
        liberer_arbre(a->enfants[i]);
        a->enfants[i] = NULL; 
    }

    free(a->enfants);
    a->enfants = NULL;
    free(a);
    a = NULL; 
}

/* Fonction pour créer un arbre de jeu de profondeur donnée */
arbre creer_arbre_position(Plateau *plateau, Joueur joueur, int profondeur) {
    arbre racine;
    Position *coups_possibles;
    int nb_coups;
    int i;
    size_t taille_enfants;
    Plateau nouveau_plateau;
    Position coup_initial = {-1, -1};

    racine = creer_noeud(plateau, coup_initial); /* Racine sans coup spécifique */

    if (profondeur == 0) {
        return racine;
    }

    coups_possibles = generer_coups_possibles(plateau, joueur, &nb_coups);
    if (nb_coups == 0) {

        free(coups_possibles);
        coups_possibles = NULL; 
        return racine;
    }

    racine->enfants = (arbre *)malloc(nb_coups * sizeof(arbre));
    if (racine->enfants == NULL) {
        printf("Erreur : allocation mémoire échouée pour les enfants.\n");
        exit(EXIT_FAILURE);
    }

    racine->nb_enfants = nb_coups;

    /* Mise à jour des compteurs de mémoire */
    taille_enfants = nb_coups * sizeof(arbre);
    memoire_totale_arbre += taille_enfants;
    memoire_arbre_courant += taille_enfants;

    for (i = 0; i < nb_coups; i++) {
        nouveau_plateau = *plateau;
        jouer_coup(&nouveau_plateau, coups_possibles[i], joueur);
        racine->enfants[i] = creer_arbre_position(&nouveau_plateau, 
                                                  (joueur == NOIR) ? BLANC : NOIR, 
                                                  profondeur - 1);
        racine->enfants[i]->coup = coups_possibles[i];
    }

    free(coups_possibles);
    coups_possibles = NULL;

    return racine;
}

/* Fonction pour appliquer l'algorithme Minimax */
int minimax(arbre a, int profondeur, int est_max) {
    int meilleure_valeur;
    int i, valeur;
    Joueur joueur_ordi = a->plateau.ordinateur;
    /*Joueur joueur_actif = est_max ? joueur_ordi : ((joueur_ordi == NOIR) ? BLANC : NOIR);*/

    compteur_noeuds_minimax++;
    /* Condition d'arrêt : feuille ou profondeur 0 */
    if (a->nb_enfants == 0 || profondeur == 0) {
        return evaluation_plateau(&a->plateau, joueur_ordi); /* Évaluation du point de vue de l'ordinateur */
    }

    if (est_max) {
        meilleure_valeur = -1000000;
        for (i = 0; i < a->nb_enfants; i++) {
            valeur = minimax(a->enfants[i], profondeur - 1, 0);
            if (valeur > meilleure_valeur) {
                meilleure_valeur = valeur;
            }
        }
    } else {
        meilleure_valeur = 1000000;
        for (i = 0; i < a->nb_enfants; i++) {
            valeur = minimax(a->enfants[i], profondeur - 1, 1);
            if (valeur < meilleure_valeur) {
                meilleure_valeur = valeur;
            }
        }
    }
    return meilleure_valeur;
}

/* Fonction pour choisir le meilleur coup à jouer */
Position choisir_meilleur_coup(arbre a) {
    int meilleure_valeur;
    int valeur;
    Position meilleur_coup;
    int i;
    /*Joueur joueur_ordi = a->plateau.ordinateur;*/
    
    /* L'ordinateur est toujours MAX car on évalue du point de vue de l'ordinateur 
    int est_max = 1; */
    
    meilleure_valeur = -1000000;
    meilleur_coup.ligne = -1;
    meilleur_coup.colonne = -1;

    for (i = 0; i < a->nb_enfants; i++) {
        valeur = minimax(a->enfants[i], 1, 0); /* L'adversaire joue après notre coup */
        
        printf("Coup (%d, %d) : Valeur Minimax = %d\n", 
               a->enfants[i]->coup.ligne, 
               a->enfants[i]->coup.colonne, 
               valeur);
               
        if (valeur > meilleure_valeur) {
            meilleure_valeur = valeur;
            meilleur_coup = a->enfants[i]->coup;
        }
    }

    printf("Meilleur coup choisi : (%d, %d) avec valeur = %d\n", 
           meilleur_coup.ligne, 
           meilleur_coup.colonne, 
           meilleure_valeur);

    return meilleur_coup;
}

/* ****************************************************************************************
 * Fonctions de l'IA pour l'étape 4 du projet : 
 * IA : arbre avec plus grande profondeur et évaluation plus complexe 
***************************************************************************************** */
/* Fonction pour appliquer l'algorithme Minimax -> Ajustement pour l'étape 4 */
int minimax_etape4(arbre a, int profondeur, int est_max) {
    int meilleure_valeur;
    int i, valeur;
    Joueur joueur_ordi = a->plateau.ordinateur;
    /*Joueur joueur_actif = est_max ? joueur_ordi : ((joueur_ordi == NOIR) ? BLANC : NOIR);*/

    compteur_noeuds_minimax++;
    
    /* Condition d'arrêt : feuille ou profondeur 0 */
    if (a->nb_enfants == 0 || profondeur == 0) {
        return evaluation_etape4(&a->plateau, joueur_ordi); /* Évaluation du point de vue de l'ordinateur */
    }

    if (est_max) {
        meilleure_valeur = -1000000; /* Valeur très basse pour maximisation */
        for (i = 0; i < a->nb_enfants; i++) {
            valeur = minimax_etape4(a->enfants[i], profondeur - 1, 0); /* Appel récursif pour minimisation */
            if (valeur > meilleure_valeur) {
                meilleure_valeur = valeur;
            }
        }
    } else {
        meilleure_valeur = 1000000; /* Valeur très haute pour minimisation */
        for (i = 0; i < a->nb_enfants; i++) {
            valeur = minimax_etape4(a->enfants[i], profondeur - 1, 1); /* Appel récursif pour maximisation */
            if (valeur < meilleure_valeur) {
                meilleure_valeur = valeur;
            }
        }
    }
    
    return meilleure_valeur;
}

/* Fonction pour choisir le meilleur coup à jouer en utilisant l'arbre de l'étape 4 */
Position choisir_meilleur_coup_etape4(arbre a, int profondeur) {
    int valeur, i;
    Position meilleur_coup;
    /*Joueur joueur_ordi = a->plateau.ordinateur;*/
    
    /* L'ordinateur est toujours MAX car on évalue du point de vue de l'ordinateur 
    int est_max = 1; */
    int meilleure_valeur = -1000000; /* Toujours chercher à maximiser */
    
    /* Initialiser le meilleur coup */
    meilleur_coup.ligne = -1;
    meilleur_coup.colonne = -1;

    /* Parcourir tous les enfants de la racine */
    for (i = 0; i < a->nb_enfants; i++) {
        /* Appliquer Minimax sur chaque enfant */
        valeur = minimax_etape4(a->enfants[i], profondeur - 1, 0); /* L'adversaire joue, donc 0 */

        printf("Coup (%d, %d) : Valeur Minimax = %d\n", 
               a->enfants[i]->coup.ligne, 
               a->enfants[i]->coup.colonne, 
               valeur);

        /* Chercher toujours le maximum puisqu'on évalue du point de vue de l'ordinateur */
        if (valeur > meilleure_valeur) {
            meilleure_valeur = valeur;
            meilleur_coup = a->enfants[i]->coup;
        }
    }

    printf("Meilleur coup choisi : (%d, %d) avec valeur = %d\n", 
           meilleur_coup.ligne,
           meilleur_coup.colonne, 
           meilleure_valeur);

    return meilleur_coup;
}
/******************************************************************************************
 * Fonctions de l'IA pour l'étape 5 du projet : 
 * IA : arbre avec optimisation alpha-bêta et évaluation plus complexe
******************************************************************************************/
/* Fonction pour appliquer l'algorithme Alpha-Bêta avec minimax pur */
int alpha_beta(arbre a, int profondeur, int alpha, int beta, int est_max) {
    int meilleure_valeur;
    int i, valeur,score;
    Joueur joueur_ordi = a->plateau.ordinateur;
    /*Joueur joueur_actif = est_max ? joueur_ordi : ((joueur_ordi == NOIR) ? BLANC : NOIR);*/

    compteur_noeuds_alphabeta++;

    /* Condition d'arrêt : feuille ou profondeur 0 */
    if (a->nb_enfants == 0 || profondeur == 0) {
        score = evaluation_etape5(&a->plateau, joueur_ordi);
        return score;
    }

    if (est_max) {
        meilleure_valeur = -1000000;
        for (i = 0; i < a->nb_enfants; i++) {
            valeur = alpha_beta(a->enfants[i], profondeur - 1, alpha, beta, 0);
            if (valeur > meilleure_valeur) {
                meilleure_valeur = valeur;
            }
            if (meilleure_valeur > alpha) {
                alpha = meilleure_valeur;
            }
            if (alpha >= beta) {
                return meilleure_valeur;
            }
        }
    } else {
        meilleure_valeur = 1000000;
        for (i = 0; i < a->nb_enfants; i++) {
            valeur = alpha_beta(a->enfants[i], profondeur - 1, alpha, beta, 1);
            if (valeur < meilleure_valeur) {
                meilleure_valeur = valeur;
            }
            if (meilleure_valeur < beta) {
                beta = meilleure_valeur;
            }
            if (alpha >= beta) {
                return meilleure_valeur;
            }
        }
    }

    return meilleure_valeur;
}

/* Fonction pour choisir le meilleur coup en utilisant Alpha-Bêta */
Position choisir_meilleur_coup_alphabeta(arbre a, int profondeur) {
    int valeur, i;
    Position meilleur_coup;
    int alpha, beta;
    /*Joueur joueur_ordi = a->plateau.ordinateur;*/
    
    /*int est_max = 1;*/
    int meilleure_valeur = -1000000; /* Toujours chercher à maximiser */
    
    /* Initialiser les bornes alpha et beta */
    alpha = -1000000;
    beta = 1000000;

    /* Initialiser le meilleur coup */
    meilleur_coup.ligne = -1;
    meilleur_coup.colonne = -1;

    /* Parcourir tous les enfants du nœud racine */
    for (i = 0; i < a->nb_enfants; i++) {
        /* Appliquer Alpha-Bêta sur chaque enfant */
        valeur = alpha_beta(a->enfants[i], profondeur - 1, alpha, beta, 0); /* L'adversaire joue, donc 0 */

        /* Affichage du score pour chaque coup */
        printf("Coup (%d, %d) : Valeur Alpha-Beta = %d\n", 
               a->enfants[i]->coup.ligne, 
               a->enfants[i]->coup.colonne, 
               valeur);

        /* Chercher toujours le maximum puisqu'on évalue du point de vue de l'ordinateur */
        if (valeur > meilleure_valeur) {
            meilleure_valeur = valeur;
            meilleur_coup = a->enfants[i]->coup;
            
            /* Mettre à jour alpha */
            if (meilleure_valeur > alpha) {
                alpha = meilleure_valeur;
            }
        }
    }

    /* Affichage du coup choisi */
    printf("Meilleur coup choisi : (%d, %d) avec valeur = %d\n", 
           meilleur_coup.ligne, 
           meilleur_coup.colonne, 
           meilleure_valeur);
    /* Afficher la mémoire utilisée par l'arbre courant -> ON affiche pour debug 
    afficher_memoire_arbre_courant(); */
    return meilleur_coup;
}
/******************************************************************************************
 * Fonctions de l'IA pour l'étape 7 du projet : 
 * IA : avec optimisations et initatives personnelles
******************************************************************************************/
/* Tri : */
/* Fonction de comparaison pour qsort (tri décroissant) */
int comparer_enfants(const void *a, const void *b) {
    Noeud *enfantA = *(Noeud **)a;
    Noeud *enfantB = *(Noeud **)b;
    return enfantB->evaluation - enfantA->evaluation; 
}

/* Fonction pour ordonner statiquement les coups */
void ordonner_coups_statique(arbre noeud) {
    int i;
    Position p;
    /* Tableau de priorité pour les positions */
    int priorite[8][8] = {
        {100, -10,  8,  6,  6,  8, -10, 100},
        {-10, -20, -4, -4, -4, -4, -20, -10},
        {  8,  -4,  6,  4,  4,  6,  -4,   8},
        {  6,  -4,  4,  0,  0,  4,  -4,   6},
        {  6,  -4,  4,  0,  0,  4,  -4,   6},
        {  8,  -4,  6,  4,  4,  6,  -4,   8},
        {-10, -20, -4, -4, -4, -4, -20, -10},
        {100, -10,  8,  6,  6,  8, -10, 100}
    };
    
    /* Évaluer chaque enfant selon la table de priorité */
    for (i = 0; i < noeud->nb_enfants; i++) {
        p = noeud->enfants[i]->coup;
        noeud->enfants[i]->evaluation = priorite[p.ligne][p.colonne];
    }
    
    /* Trier les enfants */
    qsort(noeud->enfants, noeud->nb_enfants, sizeof(Noeud *), comparer_enfants);
}

/* Fonction NegaScout avec tri des coups (approche Minimax pure) */
int nega_scout_arbre_avec_tri(arbre noeud, int profondeur, int alpha, int beta, int est_max) {
    int meilleure_valeur, valeur, i;
    Joueur joueur_ordi = noeud->plateau.ordinateur;
    /*Joueur joueur_actif = est_max ? joueur_ordi : ((joueur_ordi == NOIR) ? BLANC : NOIR);*/
    
    compteur_noeuds_negascout++;
    
    /* Si profondeur maximale ou feuille, évaluer directement */
    if (profondeur == 0 || noeud == NULL || noeud->nb_enfants == 0) {
        return evaluation_ponderee7(&noeud->plateau, joueur_ordi); /* Évaluation du point de vue de l'ordinateur */
    }
    
    /* Trier les enfants avant d'explorer */
    ordonner_coups_statique(noeud);
    
    if (est_max) {
        /* Maximisation pour le joueur ordinateur */
        meilleure_valeur = -1000000;
        
        for (i = 0; i < noeud->nb_enfants; i++) {
            /* Premier enfant: recherche avec fenêtre complète */
            if (i == 0) {
                valeur = nega_scout_arbre_avec_tri(noeud->enfants[i], profondeur - 1, alpha, beta, 0);
            } 
            /* Autres enfants: d'abord recherche avec fenêtre nulle */
            else {
                valeur = nega_scout_arbre_avec_tri(noeud->enfants[i], profondeur - 1, alpha, alpha + 1, 0);
                
                /* Si la valeur est dans la fenêtre, refaire la recherche avec fenêtre complète */
                if (valeur > alpha && valeur < beta) {
                    valeur = nega_scout_arbre_avec_tri(noeud->enfants[i], profondeur - 1, alpha, beta, 0);
                }
            }
            
            /* Mettre à jour la meilleure valeur */
            if (valeur > meilleure_valeur) {
                meilleure_valeur = valeur;
            }
            
            /* Mettre à jour alpha */
            if (meilleure_valeur > alpha) {
                alpha = meilleure_valeur;
            }
            
            /* Coupure alpha-bêta */
            if (alpha >= beta) {
                return meilleure_valeur;
            }
        }
    } else {
        /* Minimisation pour l'adversaire */
        meilleure_valeur = 1000000;
        
        for (i = 0; i < noeud->nb_enfants; i++) {
            /* Premier enfant: recherche avec fenêtre complète */
            if (i == 0) {
                valeur = nega_scout_arbre_avec_tri(noeud->enfants[i], profondeur - 1, alpha, beta, 1);
            } 
            /* Autres enfants: d'abord recherche avec fenêtre nulle */
            else {
                valeur = nega_scout_arbre_avec_tri(noeud->enfants[i], profondeur - 1, beta - 1, beta, 1);
                
                /* Si la valeur est dans la fenêtre, refaire la recherche avec fenêtre complète */
                if (valeur > alpha && valeur < beta) {
                    valeur = nega_scout_arbre_avec_tri(noeud->enfants[i], profondeur - 1, alpha, beta, 1);
                }
            }
            
            /* Mettre à jour la meilleure valeur */
            if (valeur < meilleure_valeur) {
                meilleure_valeur = valeur;
            }
            
            /* Mettre à jour beta */
            if (meilleure_valeur < beta) {
                beta = meilleure_valeur;
            }
            
            /* Coupure alpha-bêta */
            if (alpha >= beta) {
                return meilleure_valeur;
            }
        }
    }
    
    return meilleure_valeur;
}

/* Fonction Nega-Scout sans tri des coups */
int nega_scout_arbre_sans_tri(arbre noeud, int profondeur, int alpha, int beta, int est_max) {
    int meilleure_valeur, valeur, i;
    Joueur joueur_ordi = noeud->plateau.ordinateur;
    /*Joueur joueur_actif = est_max ? joueur_ordi : ((joueur_ordi == NOIR) ? BLANC : NOIR);*/
    
    compteur_noeuds_negascout++;
    
    /* Si profondeur maximale ou feuille, évaluer directement */
    if (profondeur == 0 || noeud == NULL || noeud->nb_enfants == 0) {
        return evaluation_ponderee7(&noeud->plateau, joueur_ordi); /* Évaluation du point de vue de l'ordinateur */
    }
    
    if (est_max) {
        /* Maximisation pour le joueur ordinateur */
        meilleure_valeur = -1000000;
        
        for (i = 0; i < noeud->nb_enfants; i++) {
            /* Premier enfant: recherche avec fenêtre complète */
            if (i == 0) {
                valeur = nega_scout_arbre_sans_tri(noeud->enfants[i], profondeur - 1, alpha, beta, 0);
            } 
            /* Autres enfants: d'abord recherche avec fenêtre nulle */
            else {
                valeur = nega_scout_arbre_sans_tri(noeud->enfants[i], profondeur - 1, alpha, alpha + 1, 0);
                
                /* Si la valeur est dans la fenêtre, refaire la recherche avec fenêtre complète */
                if (valeur > alpha && valeur < beta) {
                    valeur = nega_scout_arbre_sans_tri(noeud->enfants[i], profondeur - 1, alpha, beta, 0);
                }
            }
            
            /* Mettre à jour la meilleure valeur */
            if (valeur > meilleure_valeur) {
                meilleure_valeur = valeur;
            }
            
            /* Mettre à jour alpha */
            if (meilleure_valeur > alpha) {
                alpha = meilleure_valeur;
            }
            
            /* Coupure alpha-bêta */
            if (alpha >= beta) {
                return meilleure_valeur;
            }
        }
    } else {
        /* Minimisation pour l'adversaire */
        meilleure_valeur = 1000000;
        
        for (i = 0; i < noeud->nb_enfants; i++) {
            /* Premier enfant: recherche avec fenêtre complète */
            if (i == 0) {
                valeur = nega_scout_arbre_sans_tri(noeud->enfants[i], profondeur - 1, alpha, beta, 1);
            } 
            /* Autres enfants: d'abord recherche avec fenêtre nulle */
            else {
                valeur = nega_scout_arbre_sans_tri(noeud->enfants[i], profondeur - 1, beta - 1, beta, 1);
                
                /* Si la valeur est dans la fenêtre, refaire la recherche avec fenêtre complète */
                if (valeur > alpha && valeur < beta) {
                    valeur = nega_scout_arbre_sans_tri(noeud->enfants[i], profondeur - 1, alpha, beta, 1);
                }
            }
            
            /* Mettre à jour la meilleure valeur */
            if (valeur < meilleure_valeur) {
                meilleure_valeur = valeur;
            }
            
            /* Mettre à jour beta */
            if (meilleure_valeur < beta) {
                beta = meilleure_valeur;
            }
            
            /* Coupure alpha-bêta */
            if (alpha >= beta) {
                return meilleure_valeur;
            }
        }
    }
    
    return meilleure_valeur;
}
/* Fonction pour ordonner dynamiquement les coups en fonction de leur évaluation réelle */
void ordonner_coups_dynamique(arbre noeud, Joueur joueur) {
    int i;
    /* Évaluer chaque enfant directement avec son plateau */
    for (i = 0; i < noeud->nb_enfants; i++) {
        /* Utiliser le plateau de l'enfant qui contient déjà le coup joué */
        noeud->enfants[i]->evaluation = evaluation_ponderee7(&noeud->enfants[i]->plateau, joueur);
    }
    
    /* Trier les enfants par ordre décroissant d'évaluation 
    pourquoi qsort() ? -> elle est de complexité O(n log n) et est plus rapide que certains tris */
    qsort(noeud->enfants, noeud->nb_enfants, sizeof(Noeud *), comparer_enfants);

    /* Debug : afficher l'ordre des coups */
    /*
    printf("Ordre des coups après tri dynamique:\n");
    for (i = 0; i < noeud->nb_enfants; i++) {
        printf("  Coup (%d,%d) - Score: %d\n", 
               noeud->enfants[i]->coup.ligne, 
               noeud->enfants[i]->coup.colonne, 
               noeud->enfants[i]->evaluation);
    }
    */
}

/* Fonction NegaScout avec ordonnancement dynamique des coups */
int nega_scout_arbre_dynamique(arbre noeud, int profondeur, int alpha, int beta, int est_max) {
    int meilleure_valeur, valeur, i;
    Joueur joueur_ordi = noeud->plateau.ordinateur;
    Joueur joueur_actif = est_max ? joueur_ordi : ((joueur_ordi == NOIR) ? BLANC : NOIR);
    
    compteur_noeuds_negascout++;
    
    /* Si profondeur maximale ou feuille, évaluer directement */
    if (profondeur == 0 || noeud == NULL || noeud->nb_enfants == 0) {
        return evaluation_ponderee7(&noeud->plateau, joueur_ordi); /* Évaluation du point de vue de l'ordinateur */
    }
    
    /* Trier les enfants avant d'explorer, en utilisant l'ordonnancement dynamique */
    ordonner_coups_dynamique(noeud, joueur_actif);
    
    if (est_max) {
        /* Maximisation pour le joueur ordinateur */
        meilleure_valeur = -1000000;
        
        for (i = 0; i < noeud->nb_enfants; i++) {
            /* Premier enfant: recherche avec fenêtre complète */
            if (i == 0) {
                valeur = nega_scout_arbre_dynamique(noeud->enfants[i], profondeur - 1, alpha, beta, 0);
            } 
            /* Autres enfants: d'abord recherche avec fenêtre nulle */
            else {
                valeur = nega_scout_arbre_dynamique(noeud->enfants[i], profondeur - 1, alpha, alpha + 1, 0);
                
                /* Si la valeur est dans la fenêtre, refaire la recherche avec fenêtre complète */
                if (valeur > alpha && valeur < beta) {
                    valeur = nega_scout_arbre_dynamique(noeud->enfants[i], profondeur - 1, alpha, beta, 0);
                }
            }
            
            /* Mettre à jour la meilleure valeur */
            if (valeur > meilleure_valeur) {
                meilleure_valeur = valeur;
            }
            
            /* Mettre à jour alpha */
            if (meilleure_valeur > alpha) {
                alpha = meilleure_valeur;
            }
            
            /* Coupure alpha-bêta */
            if (alpha >= beta) {
                return meilleure_valeur;
            }
        }
    } else {
        /* Minimisation pour l'adversaire */
        meilleure_valeur = 1000000;
        
        for (i = 0; i < noeud->nb_enfants; i++) {
            /* Premier enfant: recherche avec fenêtre complète */
            if (i == 0) {
                valeur = nega_scout_arbre_dynamique(noeud->enfants[i], profondeur - 1, alpha, beta, 1);
            } 
            /* Autres enfants: d'abord recherche avec fenêtre nulle */
            else {
                valeur = nega_scout_arbre_dynamique(noeud->enfants[i], profondeur - 1, beta - 1, beta, 1);
                
                /* Si la valeur est dans la fenêtre, refaire la recherche avec fenêtre complète */
                if (valeur > alpha && valeur < beta) {
                    valeur = nega_scout_arbre_dynamique(noeud->enfants[i], profondeur - 1, alpha, beta, 1);
                }
            }
            
            /* Mettre à jour la meilleure valeur */
            if (valeur < meilleure_valeur) {
                meilleure_valeur = valeur;
            }
            
            /* Mettre à jour beta */
            if (meilleure_valeur < beta) {
                beta = meilleure_valeur;
            }
            
            /* Coupure alpha-bêta */
            if (alpha >= beta) {
                return meilleure_valeur;
            }
        }
    }
    
    return meilleure_valeur;
}

/* Fonction pour choisir le meilleur coup avec NegaScout et tri statique (version corrigée) */
Position choisir_meilleur_coup_avec_tri(arbre arbre_jeu, int profondeur) {
    Position meilleur_coup;
    int i, valeur;
    int alpha = -1000000;
    int beta = 1000000;
    int meilleure_valeur = -1000000; /* Toujours maximiser du point de vue de l'ordinateur */
    /*Joueur joueur_ordi = arbre_jeu->plateau.ordinateur;*/
    
    /*int est_max = 1;*/
    
    /* Initialisation du meilleur coup */
    meilleur_coup.ligne = -1;
    meilleur_coup.colonne = -1;
    
    /* Parcourir tous les enfants de l'arbre */
    for (i = 0; i < arbre_jeu->nb_enfants; i++) {
        /* Appliquer NegaScout sur chaque enfant */
        valeur = nega_scout_arbre_avec_tri(
            arbre_jeu->enfants[i], 
            profondeur - 1, 
            alpha, 
            beta, 
            0  /* L'adversaire joue après notre coup (toujours MIN) */
        );
        
        /* Affichage du score pour chaque coup */
        printf("Coup (%d, %d) : Valeur NegaScout avec Tri = %d\n", 
               arbre_jeu->enfants[i]->coup.ligne, 
               arbre_jeu->enfants[i]->coup.colonne, 
               valeur);
        
        /* Chercher toujours le maximum puisqu'on évalue du point de vue de l'ordinateur */
        if (valeur > meilleure_valeur) {
            meilleure_valeur = valeur;
            meilleur_coup = arbre_jeu->enfants[i]->coup;
            
            /* Mettre à jour alpha */
            if (meilleure_valeur > alpha) {
                alpha = meilleure_valeur;
            }
        }
    }
    
    /* Affichage du coup choisi */
    printf("Meilleur coup choisi : (%d, %d) avec valeur = %d\n", 
           meilleur_coup.ligne, 
           meilleur_coup.colonne, 
           meilleure_valeur);
           
    return meilleur_coup;
}
/* Fonction pour choisir le meilleur coup avec NegaScout et tri dynamique - Version corrigée */
Position choisir_meilleur_coup_dynamique(arbre arbre_jeu, int profondeur) {
    Position meilleur_coup;
    int i, valeur;
    int alpha = -1000000;
    int beta = 1000000;
    int meilleure_valeur = -1000000; /* Toujours maximiser du point de vue de l'ordinateur */
    /*Joueur joueur_ordi = arbre_jeu->plateau.ordinateur;*/
    
    /*int est_max = 1;*/
    
    /* Initialisation du meilleur coup */
    meilleur_coup.ligne = -1;
    meilleur_coup.colonne = -1;
    
    /* Parcourir tous les enfants de l'arbre */
    for (i = 0; i < arbre_jeu->nb_enfants; i++) {
        /* Appliquer NegaScout sur chaque enfant */
        valeur = nega_scout_arbre_dynamique(
            arbre_jeu->enfants[i], 
            profondeur - 1, 
            alpha, 
            beta, 
            0  /* L'adversaire joue après notre coup (toujours MIN) */
        );
        
        /* Affichage du score pour chaque coup */
        printf("Coup (%d, %d) : Valeur NegaScout Dynamique = %d\n", 
               arbre_jeu->enfants[i]->coup.ligne, 
               arbre_jeu->enfants[i]->coup.colonne, 
               valeur);
        
        /* Chercher toujours le maximum puisqu'on évalue du point de vue de l'ordinateur */
        if (valeur > meilleure_valeur) {
            meilleure_valeur = valeur;
            meilleur_coup = arbre_jeu->enfants[i]->coup;
            
            /* Mettre à jour alpha */
            if (meilleure_valeur > alpha) {
                alpha = meilleure_valeur;
            }
        }
    }
    
    /* Affichage du coup choisi */
    printf("Meilleur coup choisi : (%d, %d) avec valeur = %d\n", 
           meilleur_coup.ligne, 
           meilleur_coup.colonne, 
           meilleure_valeur);
           
    return meilleur_coup;
}

/* Fonction pour choisir le meilleur coup avec NegaScout sans tri - Version corrigée */
Position choisir_meilleur_coup_sans_tri(arbre arbre_jeu, int profondeur) {
    Position meilleur_coup;
    int i, valeur;
    int alpha = -1000000;
    int beta = 1000000;
    int meilleure_valeur = -1000000; /* Toujours maximiser du point de vue de l'ordinateur */
    /*Joueur joueur_ordi = arbre_jeu->plateau.ordinateur;*/
    
    /*int est_max = 1;*/
    
    /* Initialisation du meilleur coup */
    meilleur_coup.ligne = -1;
    meilleur_coup.colonne = -1;
    
    /* Parcourir tous les enfants de l'arbre */
    for (i = 0; i < arbre_jeu->nb_enfants; i++) {
        /* Appliquer NegaScout sur chaque enfant */
        valeur = nega_scout_arbre_sans_tri(
            arbre_jeu->enfants[i], 
            profondeur - 1, 
            alpha, 
            beta, 
            0  /* L'adversaire joue après notre coup (toujours MIN) */
        );
        
        /* Affichage du score pour chaque coup */
        printf("Coup (%d, %d) : Valeur NegaScout Sans Tri = %d\n", 
               arbre_jeu->enfants[i]->coup.ligne, 
               arbre_jeu->enfants[i]->coup.colonne, 
               valeur);
        
        /* Chercher toujours le maximum puisqu'on évalue du point de vue de l'ordinateur */
        if (valeur > meilleure_valeur) {
            meilleure_valeur = valeur;
            meilleur_coup = arbre_jeu->enfants[i]->coup;
            
            /* Mettre à jour alpha */
            if (meilleure_valeur > alpha) {
                alpha = meilleure_valeur;
            }
        }
    }
    
    /* Affichage du coup choisi */
    printf("Meilleur coup choisi : (%d, %d) avec valeur = %d\n", 
           meilleur_coup.ligne, 
           meilleur_coup.colonne, 
           meilleure_valeur);
           
    return meilleur_coup;
}

#endif /* _IA_C_ */