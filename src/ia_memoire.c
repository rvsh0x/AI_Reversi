#ifndef _IA_MEMOIRE_C_
#define _IA_MEMOIRE_C_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/ia.h"
#include "../include/evaluation.h"

/* Compteur de nœuds explorés pour l'algorithme à mémoire optimisée */
int compteur_noeuds_memoire = 0;

/* Variables pour le suivi de la mémoire */
size_t memoire_totale_optimisee = 0;    /* Somme des mémoires maximales de chaque coup */
size_t memoire_courante_optimisee = 0;  /* Mémoire actuellement allouée */
size_t memoire_max_optimisee = 0;       /* MAX de mémoire pour le coup actuel */

/* Fonction pour afficher la mémoire courante de l'approche optimisée */
void afficher_memoire_optimisee_courante() {
    printf("Mémoire utilisée par l'approche optimisée ACTUELLEMENT : %lu octets (%.2f Ko)\n",
           (unsigned long)memoire_courante_optimisee, memoire_courante_optimisee / 1024.0);
}

void jouer_coup_avec_historique(Plateau *plateau, Position pos, Joueur joueur, 
                               HistoriqueCoup *historique) {
    int i, j, k;
    Position tmp;
    int continuer_parcours;
    int directions[8][2] = {
        {-1, -1}, {-1, 0}, {-1, 1},
        { 0, -1},          { 0, 1},
        { 1, -1}, { 1, 0}, { 1, 1}
    };
    
    /* Initialiser l'historique */
    historique->position = pos;
    historique->nb_pions_retournes = 0;
    
    /* Placer le pion du joueur */
    plateau->cases[pos.ligne][pos.colonne] = joueur;
    
    /* Vérifier dans toutes les directions */
    for (k = 0; k < 8; k++) {
        i = pos.ligne + directions[k][0];
        j = pos.colonne + directions[k][1];
        
        tmp.ligne = i;
        tmp.colonne = j;
        /* Continuer dans cette direction tant qu'on trouve des pions adverses */
        if (est_dans_plateau(tmp) && 
            plateau->cases[i][j] != VIDE && 
            plateau->cases[i][j] != joueur) {
            
            /* Avancer dans cette direction */
            continuer_parcours = 1;
            while (continuer_parcours) {
                i += directions[k][0];
                j += directions[k][1];
                
                tmp.ligne = i;
                tmp.colonne = j;
                
                if (!est_dans_plateau(tmp) || 
                    plateau->cases[i][j] == VIDE || 
                    plateau->cases[i][j] == joueur) {
                    continuer_parcours = 0;
                }
            }
            
            /* Si on a trouvé un pion du joueur, retourner tous les pions entre les deux */
            if (est_dans_plateau(tmp) && plateau->cases[i][j] == joueur) {
                i = pos.ligne + directions[k][0];
                j = pos.colonne + directions[k][1];
                
                while (plateau->cases[i][j] != joueur) {
                    /* Enregistrer le pion et son état avant de le retourner */
                    historique->pions_retournes[historique->nb_pions_retournes][0] = i;
                    historique->pions_retournes[historique->nb_pions_retournes][1] = j;
                    historique->nb_pions_retournes++;
                    
                    /* Retourner le pion */
                    plateau->cases[i][j] = joueur;
                    
                    /* Avancer dans cette direction */
                    i += directions[k][0];
                    j += directions[k][1];
                }
            }
        }
    }
}

/* Fonction pour annuler un coup en utilisant l'historique */
void annuler_coup_avec_historique(Plateau *plateau, HistoriqueCoup *historique) {
    int i;
    int ligne, colonne;
    Joueur joueur = plateau->cases[historique->position.ligne][historique->position.colonne];
    Joueur adversaire = (joueur == NOIR) ? BLANC : NOIR;
    
    /* Annuler les retournements de pions */
    for (i = 0; i < historique->nb_pions_retournes; i++) {
        ligne = historique->pions_retournes[i][0];
        colonne = historique->pions_retournes[i][1];
        plateau->cases[ligne][colonne] = adversaire;
    }
    
    /* Remettre la case d'origine à VIDE */
    plateau->cases[historique->position.ligne][historique->position.colonne] = VIDE;
}

/* Fonction pour mettre à jour les compteurs de mémoire */
void mettre_a_jour_memoire(size_t taille, int ajouter) {
    if (ajouter) {
        memoire_courante_optimisee += taille;
        
        /* Mettre à jour la mémoire maximale utilisée si nécessaire */
        if (memoire_courante_optimisee > memoire_max_optimisee) {
            memoire_max_optimisee = memoire_courante_optimisee;
        }
    } else {
        memoire_courante_optimisee -= taille;
    }
}

/* Algorithme Alpha-Beta avec optimisation mémoire - CORRIGÉ */
int alpha_beta_optimise(Plateau *plateau, int profondeur, int alpha, int beta, 
                        Joueur joueur_actif, Joueur joueur_ordi, 
                        HistoriqueCoup *historique, int *compteur) {
    int meilleure_valeur, valeur;
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
        return evaluation_etape5(plateau, joueur_ordi);
    }
    
    /* Générer tous les coups possibles */
    coups_possibles = generer_coups_possibles(plateau, joueur_actif, &nb_coups);
    
    /* Mettre à jour les compteurs de mémoire pour les coups possibles */
    taille_coups_possibles = sizeof(Position) * nb_coups;
    mettre_a_jour_memoire(taille_coups_possibles, 1);
    
    /* Si aucun coup possible, passer le tour */
    if (nb_coups == 0) {
        /* Libérer la mémoire et ajuster le compteur */
        free(coups_possibles);
        mettre_a_jour_memoire(taille_coups_possibles, 0);
        
        /* Changer de joueur et continuer l'exploration */
        return alpha_beta_optimise(plateau, profondeur - 1, alpha, beta, 
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
        mettre_a_jour_memoire(taille_historique, 1);
        
        /* Appel récursif avec le joueur adversaire */
        valeur = alpha_beta_optimise(plateau, profondeur - 1, alpha, beta,
                                    (joueur_actif == NOIR) ? BLANC : NOIR,
                                    joueur_ordi, historique, compteur);
        
        /* Annuler le coup */
        annuler_coup_avec_historique(plateau, &historique_local);
        
        /* Réduire la mémoire courante après avoir libéré l'historique */
        mettre_a_jour_memoire(taille_historique, 0);
        
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
            mettre_a_jour_memoire(taille_coups_possibles, 0);
            return meilleure_valeur;
        }
    }
    
    /* Libérer la mémoire des coups possibles et ajuster le compteur */
    free(coups_possibles);
    mettre_a_jour_memoire(taille_coups_possibles, 0);
    
    return meilleure_valeur;
}

/* Fonction principale pour trouver le meilleur coup avec optimisation mémoire */
Position alpha_beta_memoire_optimisee(Plateau *plateau, Joueur joueur_actif, int profondeur) {
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
    
    /* Réinitialiser les compteurs au début de chaque appel */
    compteur_noeuds_memoire = 0;
    memoire_courante_optimisee = 0;
    memoire_max_optimisee = 0;
    
    /* Calcul de la mémoire pour le plateau */
    taille_plateau = sizeof(Plateau);
    mettre_a_jour_memoire(taille_plateau, 1);
    
    /* Générer tous les coups possibles pour le joueur actif */
    coups_possibles = generer_coups_possibles(plateau, joueur_actif, &nb_coups);
    
    /* Calcul de la mémoire pour les coups possibles */
    taille_coups = nb_coups * sizeof(Position);
    mettre_a_jour_memoire(taille_coups, 1);
    
    /* Si aucun coup possible, retourner une position invalide */
    if (nb_coups == 0) {
        free(coups_possibles);
        mettre_a_jour_memoire(taille_coups, 0);
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
        mettre_a_jour_memoire(taille_historique, 1);
        
        /* Appel à Alpha-Beta avec le joueur adversaire */
        valeur = alpha_beta_optimise(plateau, profondeur - 1, alpha, beta,
                                    (joueur_actif == NOIR) ? BLANC : NOIR,
                                    joueur_ordi, &historique_local, &compteur_noeuds_memoire);
        
        /* Annuler le coup */
        annuler_coup_avec_historique(plateau, &historique_local);
        
        /* Réduire la mémoire courante après avoir libéré l'historique */
        mettre_a_jour_memoire(taille_historique, 0);
        
        /* Afficher le score pour chaque coup */
        printf("Coup (%d, %d) : Valeur Alpha-Beta Optimisé = %d\n", 
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
    
    /* Afficher le nombre de nœuds explorés -> AFFICHER POUR DEBUGER 
    printf("Nœuds explorés avec Alpha-Beta Optimisé: %d\n", compteur_noeuds_memoire); */
    
    /* Afficher le coup choisi */
    printf("Meilleur coup choisi : (%d, %d) avec valeur = %d\n", 
           meilleur_coup.ligne, 
           meilleur_coup.colonne, 
           meilleure_valeur);
    
    /* Ajouter la mémoire maximale de ce coup au compteur total */
    memoire_totale_optimisee += memoire_max_optimisee;
    
    /* Afficher les statistiques de mémoire ... pour etre sur ..  A decommenter au besoin
    printf("Mémoire utilisée par l'approche optimisée: %lu octets (%.2f Ko)\n",
           (unsigned long)memoire_courante_optimisee, memoire_courante_optimisee / 1024.0);
    printf("Mémoire maximale pour ce coup: %lu octets (%.2f Ko)\n",
           (unsigned long)memoire_max_optimisee, memoire_max_optimisee / 1024.0);
    printf("Mémoire totale cumulée: %lu octets (%.2f Ko)\n",
           (unsigned long)memoire_totale_optimisee, memoire_totale_optimisee / 1024.0);
    */
    /* Libérer la mémoire des coups possibles et ajuster le compteur */
    free(coups_possibles);
    mettre_a_jour_memoire(taille_coups, 0);
    mettre_a_jour_memoire(taille_plateau, 0);
    
    return meilleur_coup;
}

/* Fonction pour afficher les statistiques finales après la partie */
void afficher_statistiques_finales() {
    printf("\n===== STATISTIQUES DE MÉMOIRE =====\n");
    if (memoire_totale_arbre > 0) {
        printf("Mémoire totale utilisée avec arbre: %lu octets (%.2f Mo)\n", 
               (unsigned long)memoire_totale_arbre, memoire_totale_arbre / (1024.0 * 1024.0));
    }
    if (memoire_totale_optimisee > 0) {
        printf("Mémoire totale utilisée avec optimisation: %lu octets (%.2f Mo)\n", 
               (unsigned long)memoire_totale_optimisee, memoire_totale_optimisee / (1024.0 * 1024.0));
    }
    if (memoire_totale_arbre > 0 && memoire_totale_optimisee > 0) {
        printf("Rapport d'utilisation mémoire (arbre/optimisé): %.2f\n", 
               (float)memoire_totale_arbre / (float)memoire_totale_optimisee);
        printf("Réduction mémoire: %.2f%%\n", 
               100.0 * (1.0 - (float)memoire_totale_optimisee / (float)memoire_totale_arbre));
    }
    printf("=================================\n");
}

#endif /* _IA_MEMOIRE_C_ */