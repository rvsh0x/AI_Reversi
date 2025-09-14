#ifndef _NEGA_MEMOIRE_C_
#define _NEGA_MEMOIRE_C_
#include <stdio.h>
#include <stdlib.h>
#include "../include/nega_memoire.h"

/* Compteurs pour les variantes NegaScout à mémoire optimisée */
int compteur_noeuds_negascout_optimise = 0;
int compteur_noeuds_negascout_tri_statique_optimise = 0;
int compteur_noeuds_negascout_tri_dynamique_optimise = 0;

/* Variables pour le suivi de la mémoire */
size_t memoire_totale_negascout_optimisee = 0;    /* Somme des mémoires maximales de chaque coup */
size_t memoire_courante_negascout_optimisee = 0;  /* Mémoire actuellement allouée */
size_t memoire_max_negascout_optimisee = 0;       /* Pic de mémoire pour le coup actuel */

/* Fonction pour mettre à jour les compteurs de mémoire */
void mettre_a_jour_memoire_negascout(size_t taille, int ajouter) {
    if (ajouter) {
        memoire_courante_negascout_optimisee += taille;
        
        /* Mettre à jour la mémoire maximale utilisée si nécessaire */
        if (memoire_courante_negascout_optimisee > memoire_max_negascout_optimisee) {
            memoire_max_negascout_optimisee = memoire_courante_negascout_optimisee;
        }
    } else {
        memoire_courante_negascout_optimisee -= taille;
    }
}
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

/* Ordonne les coups statiquement selon leur priorité */
void ordonner_coups_statique_optimise(Position *coups, int nb_coups) {
    int i, j;
    Position temp;
    int valeur_i, valeur_j;
    
    for (i = 0; i < nb_coups - 1; i++) {
        for (j = i + 1; j < nb_coups; j++) {
            valeur_i = priorite[coups[i].ligne][coups[i].colonne];
            valeur_j = priorite[coups[j].ligne][coups[j].colonne];
            
            if (valeur_j > valeur_i) {
                temp = coups[i];
                coups[i] = coups[j];
                coups[j] = temp;
            }
        }
    }
}


/* Ordonne les coups dynamiquement en fonction de leur évaluation */
void ordonner_coups_dynamique_optimise(Plateau *plateau, Position *coups, int nb_coups, Joueur joueur) {
    int i, j;
    Position temp;
    HistoriqueCoup historique;
    int eval_i, eval_j;
    
    for (i = 0; i < nb_coups - 1; i++) {
        for (j = i + 1; j < nb_coups; j++) {
            /* Jouer le coup i pour évaluation */
            jouer_coup_avec_historique(plateau, coups[i], joueur, &historique);
            eval_i = evaluation_ponderee7(plateau, joueur);
            annuler_coup_avec_historique(plateau, &historique);
            
            /* Jouer le coup j pour évaluation */
            jouer_coup_avec_historique(plateau, coups[j], joueur, &historique);
            eval_j = evaluation_ponderee7(plateau, joueur);
            annuler_coup_avec_historique(plateau, &historique);
            
            /* Trier par évaluation décroissante */
            if (eval_j > eval_i) {
                temp = coups[i];
                coups[i] = coups[j];
                coups[j] = temp;
            }
        }
    }
}

/* Algorithme NegaScout avec optimisation mémoire - Sans tri */
int negascout_optimise_sans_tri(Plateau *plateau, int profondeur, int alpha, int beta,
                               Joueur joueur_actif, Joueur joueur_ordi, 
                               HistoriqueCoup *historique, int *compteur) {
    int meilleure_valeur, valeur, valeur_fenetre;
    int nb_coups;
    Position *coups_possibles;
    int i;
    int est_max = (joueur_actif == joueur_ordi);
    HistoriqueCoup historique_local;
    size_t taille_coups_possibles, taille_historique;
    
    /* Incrémenter le compteur de nœuds explorés */
    (*compteur)++;
    
    /* Condition d'arrêt : profondeur atteinte ou partie terminée */
    if (profondeur == 0 || partie_terminee(plateau)) {
        return evaluation_ponderee7(plateau, joueur_ordi);
    }
    
    /* Générer tous les coups possibles */
    coups_possibles = generer_coups_possibles(plateau, joueur_actif, &nb_coups);
    
    /* Mettre à jour les compteurs de mémoire pour les coups possibles */
    taille_coups_possibles = sizeof(Position) * nb_coups;
    mettre_a_jour_memoire_negascout(taille_coups_possibles, 1);
    
    /* Si aucun coup possible, passer le tour */
    if (nb_coups == 0) {
        /* Libérer la mémoire et ajuster le compteur */
        free(coups_possibles);
        mettre_a_jour_memoire_negascout(taille_coups_possibles, 0);
        
        /* Changer de joueur et continuer l'exploration */
        return negascout_optimise_sans_tri(plateau, profondeur - 1, alpha, beta, 
                                         (joueur_actif == NOIR) ? BLANC : NOIR, 
                                         joueur_ordi, historique, compteur);
    }
    
    /* Initialiser la meilleure valeur selon MAX ou MIN */
    if (est_max) {
        meilleure_valeur = -1000000;
    } else {
        meilleure_valeur = 1000000;
    }
    
    /* Explorer tous les coups possibles */
    for (i = 0; i < nb_coups; i++) {
        /* Jouer le coup et enregistrer l'historique */
        jouer_coup_avec_historique(plateau, coups_possibles[i], joueur_actif, &historique_local);
        
        /* Mettre à jour les compteurs de mémoire pour l'historique */
        taille_historique = sizeof(HistoriqueCoup);
        mettre_a_jour_memoire_negascout(taille_historique, 1);
        
        /* Premier enfant: recherche avec fenêtre complète */
        if (i == 0) {
            valeur = negascout_optimise_sans_tri(plateau, profondeur - 1, alpha, beta,
                                              (joueur_actif == NOIR) ? BLANC : NOIR,
                                              joueur_ordi, historique, compteur);
        } else {
            /* Autres enfants: d'abord recherche avec fenêtre nulle */
            if (est_max) {
                valeur_fenetre = negascout_optimise_sans_tri(plateau, profondeur - 1, alpha, alpha + 1,
                                                         (joueur_actif == NOIR) ? BLANC : NOIR,
                                                         joueur_ordi, historique, compteur);
            } else {
                valeur_fenetre = negascout_optimise_sans_tri(plateau, profondeur - 1, beta - 1, beta,
                                                         (joueur_actif == NOIR) ? BLANC : NOIR,
                                                         joueur_ordi, historique, compteur);
            }
            
            /* Si la valeur est dans la fenêtre, refaire la recherche avec fenêtre complète */
            if ((est_max && valeur_fenetre > alpha && valeur_fenetre < beta) ||
                (!est_max && valeur_fenetre > alpha && valeur_fenetre < beta)) {
                valeur = negascout_optimise_sans_tri(plateau, profondeur - 1, alpha, beta,
                                                  (joueur_actif == NOIR) ? BLANC : NOIR,
                                                  joueur_ordi, historique, compteur);
            } else {
                valeur = valeur_fenetre;
            }
        }
        
        /* Annuler le coup */
        annuler_coup_avec_historique(plateau, &historique_local);
        
        /* Réduire la mémoire courante après avoir libéré l'historique */
        mettre_a_jour_memoire_negascout(taille_historique, 0);
        
        /* Mettre à jour la meilleure valeur et les bornes alpha/beta */
        if (est_max) {
            if (valeur > meilleure_valeur) {
                meilleure_valeur = valeur;
            }
            if (meilleure_valeur > alpha) {
                alpha = meilleure_valeur;
            }
        } else {
            if (valeur < meilleure_valeur) {
                meilleure_valeur = valeur;
            }
            if (meilleure_valeur < beta) {
                beta = meilleure_valeur;
            }
        }
        
        /* Coupure alpha-bêta */
        if (alpha >= beta) {
            /* Libérer la mémoire et ajuster le compteur */
            free(coups_possibles);
            mettre_a_jour_memoire_negascout(taille_coups_possibles, 0);
            return meilleure_valeur;
        }
    }
    
    /* Libérer la mémoire des coups possibles et ajuster le compteur */
    free(coups_possibles);
    mettre_a_jour_memoire_negascout(taille_coups_possibles, 0);
    
    return meilleure_valeur;
}

/* Algorithme NegaScout avec tri statique et optimisation mémoire */
int negascout_optimise_tri_statique(Plateau *plateau, int profondeur, int alpha, int beta,
                                   Joueur joueur_actif, Joueur joueur_ordi, 
                                   HistoriqueCoup *historique, int *compteur) {
    int meilleure_valeur, valeur, valeur_fenetre;
    int nb_coups;
    Position *coups_possibles;
    int i;
    int est_max = (joueur_actif == joueur_ordi);
    HistoriqueCoup historique_local;
    size_t taille_coups_possibles, taille_historique;
    
    /* Incrémenter le compteur de nœuds explorés */
    (*compteur)++;
    
    /* Condition d'arrêt : profondeur atteinte ou partie terminée */
    if (profondeur == 0 || partie_terminee(plateau)) {
        return evaluation_ponderee7(plateau, joueur_ordi);
    }
    
    /* Générer tous les coups possibles */
    coups_possibles = generer_coups_possibles(plateau, joueur_actif, &nb_coups);
    
    /* Trier les coups statiquement */
    ordonner_coups_statique_optimise(coups_possibles, nb_coups);
    
    /* Mettre à jour les compteurs de mémoire pour les coups possibles */
    taille_coups_possibles = sizeof(Position) * nb_coups;
    mettre_a_jour_memoire_negascout(taille_coups_possibles, 1);
    
    /* Si aucun coup possible, passer le tour */
    if (nb_coups == 0) {
        /* Libérer la mémoire et ajuster le compteur */
        free(coups_possibles);
        mettre_a_jour_memoire_negascout(taille_coups_possibles, 0);
        
        /* Changer de joueur et continuer l'exploration */
        return negascout_optimise_tri_statique(plateau, profondeur - 1, alpha, beta,
                                             (joueur_actif == NOIR) ? BLANC : NOIR,
                                             joueur_ordi, historique, compteur);
    }
    
    /* Initialiser la meilleure valeur selon MAX ou MIN */
    if (est_max) {
        meilleure_valeur = -1000000;
    } else {
        meilleure_valeur = 1000000;
    }
    
    /* Explorer tous les coups possibles */
    for (i = 0; i < nb_coups; i++) {
        /* Jouer le coup et enregistrer l'historique */
        jouer_coup_avec_historique(plateau, coups_possibles[i], joueur_actif, &historique_local);
        
        /* Mettre à jour les compteurs de mémoire pour l'historique */
        taille_historique = sizeof(HistoriqueCoup);
        mettre_a_jour_memoire_negascout(taille_historique, 1);
        
        /* Premier enfant: recherche avec fenêtre complète */
        if (i == 0) {
            valeur = negascout_optimise_tri_statique(plateau, profondeur - 1, alpha, beta,
                                                  (joueur_actif == NOIR) ? BLANC : NOIR,
                                                  joueur_ordi, historique, compteur);
        } else {
            /* Autres enfants: d'abord recherche avec fenêtre nulle */
            if (est_max) {
                valeur_fenetre = negascout_optimise_tri_statique(plateau, profondeur - 1, alpha, alpha + 1,
                                                             (joueur_actif == NOIR) ? BLANC : NOIR,
                                                             joueur_ordi, historique, compteur);
            } else {
                valeur_fenetre = negascout_optimise_tri_statique(plateau, profondeur - 1, beta - 1, beta,
                                                             (joueur_actif == NOIR) ? BLANC : NOIR,
                                                             joueur_ordi, historique, compteur);
            }
            
            /* Si la valeur est dans la fenêtre, refaire la recherche avec fenêtre complète */
            if ((est_max && valeur_fenetre > alpha && valeur_fenetre < beta) ||
                (!est_max && valeur_fenetre > alpha && valeur_fenetre < beta)) {
                valeur = negascout_optimise_tri_statique(plateau, profondeur - 1, alpha, beta,
                                                      (joueur_actif == NOIR) ? BLANC : NOIR,
                                                      joueur_ordi, historique, compteur);
            } else {
                valeur = valeur_fenetre;
            }
        }
        
        /* Annuler le coup */
        annuler_coup_avec_historique(plateau, &historique_local);
        
        /* Réduire la mémoire courante après avoir libéré l'historique */
        mettre_a_jour_memoire_negascout(taille_historique, 0);
        
        /* Mettre à jour la meilleure valeur et les bornes alpha/beta */
        if (est_max) {
            if (valeur > meilleure_valeur) {
                meilleure_valeur = valeur;
            }
            if (meilleure_valeur > alpha) {
                alpha = meilleure_valeur;
            }
        } else {
            if (valeur < meilleure_valeur) {
                meilleure_valeur = valeur;
            }
            if (meilleure_valeur < beta) {
                beta = meilleure_valeur;
            }
        }
        
        /* Coupure alpha-bêta */
        if (alpha >= beta) {
            /* Libérer la mémoire et ajuster le compteur */
            free(coups_possibles);
            mettre_a_jour_memoire_negascout(taille_coups_possibles, 0);
            return meilleure_valeur;
        }
    }
    
    /* Libérer la mémoire des coups possibles et ajuster le compteur */
    free(coups_possibles);
    mettre_a_jour_memoire_negascout(taille_coups_possibles, 0);
    
    return meilleure_valeur;
}

/* Algorithme NegaScout avec tri dynamique et optimisation mémoire */
int negascout_optimise_tri_dynamique(Plateau *plateau, int profondeur, int alpha, int beta,
                                    Joueur joueur_actif, Joueur joueur_ordi, 
                                    HistoriqueCoup *historique, int *compteur) {
    int meilleure_valeur, valeur, valeur_fenetre;
    int nb_coups;
    Position *coups_possibles;
    int i;
    int est_max = (joueur_actif == joueur_ordi);
    HistoriqueCoup historique_local;
    size_t taille_coups_possibles, taille_historique;
    
    /* Incrémenter le compteur de nœuds explorés */
    (*compteur)++;
    
    /* Condition d'arrêt : profondeur atteinte ou partie terminée */
    if (profondeur == 0 || partie_terminee(plateau)) {
        return evaluation_ponderee7(plateau, joueur_ordi);
    }
    
    /* Générer tous les coups possibles */
    coups_possibles = generer_coups_possibles(plateau, joueur_actif, &nb_coups);
    
    /* Mettre à jour les compteurs de mémoire pour les coups possibles */
    taille_coups_possibles = sizeof(Position) * nb_coups;
    mettre_a_jour_memoire_negascout(taille_coups_possibles, 1);
    
    /* Si aucun coup possible, passer le tour */
    if (nb_coups == 0) {
        /* Libérer la mémoire et ajuster le compteur */
        free(coups_possibles);
        mettre_a_jour_memoire_negascout(taille_coups_possibles, 0);
        
        /* Changer de joueur et continuer l'exploration */
        return negascout_optimise_tri_dynamique(plateau, profondeur - 1, alpha, beta,
                                              (joueur_actif == NOIR) ? BLANC : NOIR,
                                              joueur_ordi, historique, compteur);
    }
    
    /* Trier les coups dynamiquement */
    ordonner_coups_dynamique_optimise(plateau, coups_possibles, nb_coups, joueur_actif);
    
    /* Initialiser la meilleure valeur selon MAX ou MIN */
    if (est_max) {
        meilleure_valeur = -1000000;
    } else {
        meilleure_valeur = 1000000;
    }
    
    /* Explorer tous les coups possibles */
    for (i = 0; i < nb_coups; i++) {
        /* Jouer le coup et enregistrer l'historique */
        jouer_coup_avec_historique(plateau, coups_possibles[i], joueur_actif, &historique_local);
        
        /* Mettre à jour les compteurs de mémoire pour l'historique */
        taille_historique = sizeof(HistoriqueCoup);
        mettre_a_jour_memoire_negascout(taille_historique, 1);
        
        /* Premier enfant: recherche avec fenêtre complète */
        if (i == 0) {
            valeur = negascout_optimise_tri_dynamique(plateau, profondeur - 1, alpha, beta,
                                                   (joueur_actif == NOIR) ? BLANC : NOIR,
                                                   joueur_ordi, historique, compteur);
        } else {
            /* Autres enfants: d'abord recherche avec fenêtre nulle */
            if (est_max) {
                valeur_fenetre = negascout_optimise_tri_dynamique(plateau, profondeur - 1, alpha, alpha + 1,
                                                              (joueur_actif == NOIR) ? BLANC : NOIR,
                                                              joueur_ordi, historique, compteur);
            } else {
                valeur_fenetre = negascout_optimise_tri_dynamique(plateau, profondeur - 1, beta - 1, beta,
                                                              (joueur_actif == NOIR) ? BLANC : NOIR,
                                                              joueur_ordi, historique, compteur);
            }
            
            /* Si la valeur est dans la fenêtre, refaire la recherche avec fenêtre complète */
            if ((est_max && valeur_fenetre > alpha && valeur_fenetre < beta) ||
                (!est_max && valeur_fenetre > alpha && valeur_fenetre < beta)) {
                valeur = negascout_optimise_tri_dynamique(plateau, profondeur - 1, alpha, beta,
                                                       (joueur_actif == NOIR) ? BLANC : NOIR,
                                                       joueur_ordi, historique, compteur);
            } else {
                valeur = valeur_fenetre;
            }
        }
        
        /* Annuler le coup */
        annuler_coup_avec_historique(plateau, &historique_local);
        
        /* Réduire la mémoire courante après avoir libéré l'historique */
        mettre_a_jour_memoire_negascout(taille_historique, 0);
        
        /* Mettre à jour la meilleure valeur et les bornes alpha/beta */
        if (est_max) {
            if (valeur > meilleure_valeur) {
                meilleure_valeur = valeur;
            }
            if (meilleure_valeur > alpha) {
                alpha = meilleure_valeur;
            }
        } else {
            if (valeur < meilleure_valeur) {
                meilleure_valeur = valeur;
            }
            if (meilleure_valeur < beta) {
                beta = meilleure_valeur;
            }
        }
        
        /* Coupure alpha-bêta */
        if (alpha >= beta) {
            /* Libérer la mémoire et ajuster le compteur */
            free(coups_possibles);
            mettre_a_jour_memoire_negascout(taille_coups_possibles, 0);
            return meilleure_valeur;
        }
    }
    
    /* Libérer la mémoire des coups possibles et ajuster le compteur */
    free(coups_possibles);
    mettre_a_jour_memoire_negascout(taille_coups_possibles, 0);
    
    return meilleure_valeur;
}

/* Fonction principale pour trouver le meilleur coup avec NegaScout sans tri */
Position negascout_memoire_optimisee_sans_tri(Plateau *plateau, Joueur joueur_actif, int profondeur) {
    Position *coups_possibles;
    int nb_coups;
    int i, valeur;
    Position meilleur_coup;
    int alpha = -1000000;
    int beta = 1000000;
    int meilleure_valeur = -1000000;
    HistoriqueCoup historique_local;
    Joueur joueur_ordi = joueur_actif;
    size_t taille_plateau, taille_coups, taille_historique;
    int compteur_noeuds = 0;

    /* Réinitialiser les compteurs au début de chaque appel */
    memoire_courante_negascout_optimisee = 0;
    memoire_max_negascout_optimisee = 0;
    
    /* Calcul de la mémoire pour le plateau */
    taille_plateau = sizeof(Plateau);
    mettre_a_jour_memoire_negascout(taille_plateau, 1);

    coups_possibles = generer_coups_possibles(plateau, joueur_actif, &nb_coups);
    /* Calcul de la mémoire pour les coups possibles */
    taille_coups = nb_coups * sizeof(Position);
    mettre_a_jour_memoire_negascout(taille_coups, 1);
    /* Si aucun coup possible, retourner une position invalide */
    if (nb_coups == 0) {
        mettre_a_jour_memoire_negascout(taille_coups, 0);
        free(coups_possibles);
        meilleur_coup.ligne = -1;
        meilleur_coup.colonne = -1;
        return meilleur_coup;
    }
    /* Initialiser le meilleur coup au premier coup possible */
    meilleur_coup = coups_possibles[0];
    /* Explorer tous les coups possibles */
    for (i = 0; i < nb_coups; i++) {
        /* Jouer le coup et enregistrer l'historique */
        jouer_coup_avec_historique(plateau, coups_possibles[i], joueur_actif, &historique_local);
        /* Calcul de la mémoire pour l'historique */
        taille_historique = sizeof(HistoriqueCoup);
        mettre_a_jour_memoire_negascout(taille_historique, 1);
        /* Appel à NegaScout avec le joueur adversaire */
        valeur = negascout_optimise_sans_tri(plateau, profondeur - 1, alpha, beta,
                                           (joueur_actif == NOIR) ? BLANC : NOIR,
                                           joueur_ordi, &historique_local, &compteur_noeuds);
        
        /* Annuler le coup */
        annuler_coup_avec_historique(plateau, &historique_local);
        
        /* Réduire la mémoire courante après avoir libéré l'historique */
        mettre_a_jour_memoire_negascout(taille_historique, 0);
        
        /* Afficher le score pour chaque coup */
        printf("Coup (%d, %d) : Valeur NegaScout Sans Tri Optimisé = %d\n", 
               coups_possibles[i].ligne, 
               coups_possibles[i].colonne, 
               valeur);
        
        /* Mettre à jour le meilleur coup */
        if (valeur > meilleure_valeur) {
            meilleure_valeur = valeur;
            meilleur_coup = coups_possibles[i];
            
            /* Mettre à jour alpha */
            if (meilleure_valeur > alpha) {
                alpha = meilleure_valeur;
            }
        }
    }
    
    compteur_noeuds_negascout_optimise = compteur_noeuds;
    
    /* Afficher le coup choisi */
    printf("Meilleur coup choisi : (%d, %d) avec valeur = %d\n", 
           meilleur_coup.ligne, 
           meilleur_coup.colonne, 
           meilleure_valeur);
    
    
    /* Ajouter la mémoire maximale de ce coup au compteur total */
    memoire_totale_negascout_optimisee += memoire_max_negascout_optimisee;
    
    /* Afficher les statistiques de mémoire ... on affiche pour debugger .. comme tjrs *
    printf("Mémoire utilisée par l'approche optimisée: %lu octets (%.2f Ko)\n",
           (unsigned long)memoire_courante_negascout_optimisee / 1024.0);
    printf("Mémoire maximale pour ce coup: %lu octets (%.2f Ko)\n",
           (unsigned long)memoire_max_negascout_optimisee, memoire_max_negascout_optimisee / 1024.0);
    printf("Mémoire totale cumulée: %lu octets (%.2f Ko)\n",
           (unsigned long)memoire_totale_negascout_optimisee, memoire_totale_negascout_optimisee / 1024.0);
    */
    /* Libérer la mémoire des coups possibles et ajuster le compteur */
    mettre_a_jour_memoire_negascout(taille_coups, 0);
    free(coups_possibles);
    mettre_a_jour_memoire_negascout(taille_plateau, 0);
    
    return meilleur_coup;
}

/* Fonction principale pour trouver le meilleur coup avec NegaScout et tri statique */
Position negascout_memoire_optimisee_tri_statique(Plateau *plateau, Joueur joueur_actif, int profondeur) {
    Position *coups_possibles;
    int nb_coups;
    int i, valeur;
    Position meilleur_coup;
    int alpha = -1000000;
    int beta = 1000000;
    int meilleure_valeur = -1000000;
    HistoriqueCoup historique_local;
    Joueur joueur_ordi = joueur_actif;
    size_t taille_plateau, taille_coups, taille_historique;
    int compteur_noeuds = 0;

    /* Réinitialiser les compteurs au début de chaque appel */
    memoire_courante_negascout_optimisee = 0;
    memoire_max_negascout_optimisee = 0;
    
    /* Calcul de la mémoire pour le plateau */
    taille_plateau = sizeof(Plateau);
    mettre_a_jour_memoire_negascout(taille_plateau, 1);

    coups_possibles = generer_coups_possibles(plateau, joueur_actif, &nb_coups);
    /* Calcul de la mémoire pour les coups possibles */
    taille_coups = nb_coups * sizeof(Position);
    mettre_a_jour_memoire_negascout(taille_coups, 1);
    /* Si aucun coup possible, retourner une position invalide */
    if (nb_coups == 0) {
        mettre_a_jour_memoire_negascout(taille_coups, 0);
        free(coups_possibles);
        meilleur_coup.ligne = -1;
        meilleur_coup.colonne = -1;
        return meilleur_coup;
    }
    /* Initialiser le meilleur coup au premier coup possible */
    meilleur_coup = coups_possibles[0];
    /* Explorer tous les coups possibles */
    for (i = 0; i < nb_coups; i++) {
        /* Jouer le coup et enregistrer l'historique */
        jouer_coup_avec_historique(plateau, coups_possibles[i], joueur_actif, &historique_local);
        /* Calcul de la mémoire pour l'historique */
        taille_historique = sizeof(HistoriqueCoup);
        mettre_a_jour_memoire_negascout(taille_historique, 1);
        /* Appel à NegaScout avec le joueur adversaire */
        valeur = negascout_optimise_tri_statique(plateau, profondeur - 1, alpha, beta,
                                              (joueur_actif == NOIR) ? BLANC : NOIR,
                                              joueur_ordi, &historique_local, &compteur_noeuds);
        /* Annuler le coup */
        annuler_coup_avec_historique(plateau, &historique_local);
        /* Réduire la mémoire courante après avoir libéré l'historique */
        mettre_a_jour_memoire_negascout(taille_historique, 0);
        /* Afficher le score pour chaque coup */
        printf("Coup (%d, %d) : Valeur NegaScout Tri Statique Optimisé = %d\n", 
               coups_possibles[i].ligne, 
               coups_possibles[i].colonne, 
               valeur);
        
        /* Mettre à jour le meilleur coup */
        if (valeur > meilleure_valeur) {
            meilleure_valeur = valeur;
            meilleur_coup = coups_possibles[i];
            
            /* Mettre à jour alpha */
            if (meilleure_valeur > alpha) {
                alpha = meilleure_valeur;
            }
        }
    }
    
    compteur_noeuds_negascout_tri_statique_optimise = compteur_noeuds;

    /* Afficher le coup choisi */
    printf("Meilleur coup choisi : (%d, %d) avec valeur = %d\n", 
           meilleur_coup.ligne, 
           meilleur_coup.colonne, 
           meilleure_valeur);
    
    /* Ajouter la mémoire maximale de ce coup au compteur total */
    memoire_totale_negascout_optimisee += memoire_max_negascout_optimisee;
    /* Afficher les statistiques de mémoire ... on affiche pour debugger .. comme tjrs 
    printf("Mémoire utilisée par l'approche optimisée: %lu octets (%.2f Ko)\n",
           (unsigned long)memoire_courante_negascout_optimisee, memoire_courante_negascout_optimisee / 1024.0);
    printf("Mémoire maximale pour ce coup: %lu octets (%.2f Ko)\n",
           (unsigned long)memoire_max_negascout_optimisee, memoire_max_negascout_optimisee / 1024.0);
    printf("Mémoire totale cumulée: %lu octets (%.2f Ko)\n",
           (unsigned long)memoire_totale_negascout_optimisee, memoire_totale_negascout_optimisee / 1024.0);
    */
    /* Libérer la mémoire des coups possibles et ajuster le compteur */
    mettre_a_jour_memoire_negascout(taille_coups, 0);
    free(coups_possibles);
    mettre_a_jour_memoire_negascout(taille_plateau, 0);
    
    return meilleur_coup;
}

/* Fonction principale pour trouver le meilleur coup avec NegaScout et tri dynamique */
Position negascout_memoire_optimisee_tri_dynamique(Plateau *plateau, Joueur joueur_actif, int profondeur) {
    Position *coups_possibles;
    int nb_coups;
    int i, valeur;
    Position meilleur_coup;
    int alpha = -1000000;
    int beta = 1000000;
    int meilleure_valeur = -1000000;
    HistoriqueCoup historique_local;
    Joueur joueur_ordi = joueur_actif;
    size_t taille_plateau, taille_coups, taille_historique;
    int compteur_noeuds = 0;

    /* Réinitialiser les compteurs au début de chaque appel */
    memoire_courante_negascout_optimisee = 0;
    memoire_max_negascout_optimisee = 0;
    
    /* Calcul de la mémoire pour le plateau */
    taille_plateau = sizeof(Plateau);
    mettre_a_jour_memoire_negascout(taille_plateau, 1);

    coups_possibles = generer_coups_possibles(plateau, joueur_actif, &nb_coups);
    /* Calcul de la mémoire pour les coups possibles */
    taille_coups = nb_coups * sizeof(Position);
    mettre_a_jour_memoire_negascout(taille_coups, 1);
    
    /* Si aucun coup possible, retourner une position invalide */
    if (nb_coups == 0) {
        mettre_a_jour_memoire_negascout(taille_coups, 0);
        free(coups_possibles);
        meilleur_coup.ligne = -1;
        meilleur_coup.colonne = -1;
        return meilleur_coup;
    }
    /* Initialiser le meilleur coup au premier coup possible */
    meilleur_coup = coups_possibles[0];
    /* Explorer tous les coups possibles */
    for (i = 0; i < nb_coups; i++) {
        /* Jouer le coup et enregistrer l'historique */
        jouer_coup_avec_historique(plateau, coups_possibles[i], joueur_actif, &historique_local);
        /* Calcul de la mémoire pour l'historique */
        taille_historique = sizeof(HistoriqueCoup);
        mettre_a_jour_memoire_negascout(taille_historique, 1);
        /* Appel à NegaScout avec le joueur adversaire */
        valeur = negascout_optimise_tri_dynamique(plateau, profondeur - 1, alpha, beta,
                                               (joueur_actif == NOIR) ? BLANC : NOIR,
                                               joueur_ordi, &historique_local, &compteur_noeuds);
        /* Annuler le coup */
        annuler_coup_avec_historique(plateau, &historique_local);
        
        /* Réduire la mémoire courante après avoir libéré l'historique */
        mettre_a_jour_memoire_negascout(taille_historique, 0);
        
        /* Afficher le score pour chaque coup */
        printf("Coup (%d, %d) : Valeur NegaScout Tri Dynamique Optimisé = %d\n", 
               coups_possibles[i].ligne, 
               coups_possibles[i].colonne, 
               valeur);
        
        /* Mettre à jour le meilleur coup */
        if (valeur > meilleure_valeur) {
            meilleure_valeur = valeur;
            meilleur_coup = coups_possibles[i];
            
            /* Mettre à jour alpha */
            if (meilleure_valeur > alpha) {
                alpha = meilleure_valeur;
            }
        }
    }
    
    compteur_noeuds_negascout_tri_dynamique_optimise = compteur_noeuds;

    /* Afficher le coup choisi */
    printf("Meilleur coup choisi : (%d, %d) avec valeur = %d\n", 
           meilleur_coup.ligne, 
           meilleur_coup.colonne, 
           meilleure_valeur);
    
    /* Ajouter la mémoire maximale de ce coup au compteur total */
    memoire_totale_negascout_optimisee += memoire_max_negascout_optimisee;
    
    /* Afficher les statistiques de mémoire ... on affiche pour debugger .. comme tjrs
    printf("Mémoire utilisée par l'approche optimisée: %lu octets (%.2f Ko)\n",
           memoire_courante_negascout_optimisee, memoire_courante_negascout_optimisee / 1024.0);
    printf("Mémoire maximale pour ce coup: %lu octets (%.2f Ko)\n",
           memoire_max_negascout_optimisee, memoire_max_negascout_optimisee / 1024.0);
    printf("Mémoire totale cumulée: %lu octets (%.2f Ko)\n",
           memoire_totale_negascout_optimisee, memoire_totale_negascout_optimisee / 1024.0);
    */
    /* Libérer la mémoire des coups possibles et ajuster le compteur */
    mettre_a_jour_memoire_negascout(taille_coups, 0);
    free(coups_possibles);
    mettre_a_jour_memoire_negascout(taille_plateau, 0);
    
    return meilleur_coup;
}

#endif /* _NEGA_MEMOIRE_C_ */