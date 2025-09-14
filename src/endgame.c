#ifndef _ENDGAME_C_
#define _ENDGAME_C_
#include <stdio.h>
#include <stdlib.h>

#include "../include/endgame.h"

int compteur_noeuds_endgame = 0; /* Compteur de nœuds explorés dans l'endgame */

/* Compteurs pour la version optimisée d'Endgame */
int compteur_noeuds_endgame_optimise = 0;
size_t memoire_totale_endgame_optimisee = 0;
size_t memoire_courante_endgame_optimisee = 0;
size_t memoire_max_endgame_optimisee = 0;
size_t memoire_totale_partie = 0;


/* Fonction pour vérifier si un coup est un coin du plateau */
int est_coin(Position coup) {
    return ((coup.ligne == 0 || coup.ligne == TAILLE_PLATEAU - 1) && 
            (coup.colonne == 0 || coup.colonne == TAILLE_PLATEAU - 1));
}

/* Fonction pour ordonner les coups selon l'heuristique Fastest-First pour l'endgame */
void ordonner_coups_endgame(arbre noeud, Joueur joueur) {
    int i, j;
    int *scores;
    Joueur adversaire;
    Plateau plateau_tmp;
    int score_tmp;
    int mobilite_adversaire, stabilite_joueur;
    arbre noeud_tmp;

    
    /* Vérification des paramètres */
    if (noeud == NULL || noeud->nb_enfants == 0) {
        return;
    }
    
    /* Initialisation des variables */
    adversaire = (joueur == NOIR) ? BLANC : NOIR;
    scores = (int*)malloc(noeud->nb_enfants * sizeof(int));
    
    if (scores == NULL) {
        printf("Erreur : allocation mémoire échouée pour les scores.\n");
        return;
    }
    
    /* Calculer le score de chaque coup basé sur:
       1. Mobilité de l'adversaire (moins il a de coups, mieux c'est) 
       2. Capture de coins (priorité absolue) 
       3. Stabilité des pions */
    for (i = 0; i < noeud->nb_enfants; i++) {
        /* Priorité maximale si le coup est un coin */
        if (est_coin(noeud->enfants[i]->coup)) {
            scores[i] = 10000;
        } else {
            /* Copier le plateau pour calculer la mobilité adverse */
            plateau_tmp = noeud->enfants[i]->plateau;
            /* Score basé sur la mobilité de l'adversaire (inversé) */
            mobilite_adversaire = calculer_mobilite_immediate(&plateau_tmp, adversaire);
            /* Score basé sur la stabilité des pions du joueur */
            stabilite_joueur = evaluation_stabilite_etape7(&plateau_tmp, joueur);
            /* Combiner les deux critères avec une forte pondération pour la mobilité */
            scores[i] = -mobilite_adversaire * 10 + stabilite_joueur;
        }
    }
    
    /* Trier les enfants par score décroissant (tri à bulle) 
        -> A la fin de la partie , on a pas trop de coups qui restent 
        donc le tri à bulle est suffisant */
    for (i = 0; i < noeud->nb_enfants - 1; i++) {
        for (j = 0; j < noeud->nb_enfants - i - 1; j++) {
            if (scores[j] < scores[j + 1]) {
                /* Échanger les scores */
                score_tmp = scores[j];
                scores[j] = scores[j + 1];
                scores[j + 1] = score_tmp;
                
                /* Échanger les nœuds */
                noeud_tmp = noeud->enfants[j];
                noeud->enfants[j] = noeud->enfants[j + 1];
                noeud->enfants[j + 1] = noeud_tmp;
            }
        }
    }
    free(scores);
}

arbre creer_arbre_position_endgame(Plateau *plateau, Joueur joueur, int profondeur) {
    arbre racine;
    Position *coups_possibles;
    Position *coups_adversaire;
    int nb_coups;
    int nb_coups_adversaire;
    int i;
    Joueur adversaire;
    Plateau nouveau_plateau;
    Position coup_initial = {-1, -1}; /* Coup fictif pour la racine */

    racine = creer_noeud(plateau, coup_initial);

    /* Condition d'arrêt : profondeur 0 */
    if (profondeur == 0) {
        return racine;
    }

    /* Générer les coups possibles pour le joueur actif */
    coups_possibles = generer_coups_possibles(plateau, joueur, &nb_coups);
    
    /* Si aucun coup n'est possible, passer le tour (spécifique à l'endgame) */
    if (nb_coups == 0) {
        /* Vérifier si la partie est terminée (les deux joueurs ne peuvent pas jouer) */
        adversaire = (joueur == NOIR) ? BLANC : NOIR;
        
        coups_adversaire = generer_coups_possibles(plateau, adversaire, &nb_coups_adversaire);
        free(coups_adversaire);
        
        if (nb_coups_adversaire == 0) {
            /* Partie terminée, retourner simplement la racine */
            free(coups_possibles);
            return racine;
        } else {
            /* Passer le tour, créer un arbre pour l'adversaire */
            racine = creer_arbre_position_endgame(plateau, adversaire, profondeur - 1);
            free(coups_possibles);
            return racine;
        }
    }

    /* Allouer de la mémoire pour les enfants */
    racine->enfants = (arbre *)malloc(nb_coups * sizeof(arbre));
    if (racine->enfants == NULL) {
        printf("Erreur : allocation mémoire échouée pour les enfants.\n");
        exit(EXIT_FAILURE);
    }

    racine->nb_enfants = nb_coups;
    adversaire = (joueur == NOIR) ? BLANC : NOIR;

    /* Créer les enfants pour chaque coup possible */
    for (i = 0; i < nb_coups; i++) {
        nouveau_plateau = *plateau;
        jouer_coup(&nouveau_plateau, coups_possibles[i], joueur);
        
        racine->enfants[i] = creer_arbre_position_endgame(
            &nouveau_plateau,
            adversaire,
            profondeur - 1
        );
        racine->enfants[i]->coup = coups_possibles[i];
    }

    free(coups_possibles);
    return racine;
}

/* Fonction NegaScout optimisée pour l'Endgame - Version harmonisée */
int nega_scout_endgame(arbre noeud, int profondeur, int alpha, int beta, int est_max) {
    int meilleure_valeur, valeur, i;
    Joueur joueur_ordi = noeud->plateau.ordinateur;
    Joueur joueur_actif = est_max ? joueur_ordi : ((joueur_ordi == NOIR) ? BLANC : NOIR);
    
    compteur_noeuds_endgame++;
    
    /* Si la partie est terminée, évaluer directement avec le score exact */
    if (partie_terminee(&noeud->plateau)) {
        return evaluation_plateau(&noeud->plateau, joueur_ordi); /* Évaluation du point de vue de l'ordinateur */
    }
    
    /* Si profondeur maximale ou feuille, évaluer avec l'heuristique standard */
    if (profondeur == 0 || noeud == NULL || noeud->nb_enfants == 0) {
        return evaluation_ponderee7(&noeud->plateau, joueur_ordi); /* Évaluation du point de vue de l'ordinateur */
    }
    
    /* Trier les enfants avec l'heuristique Fastest-First spécifique à l'endgame */
    ordonner_coups_endgame(noeud, joueur_actif);
    
    if (est_max) {
        /* Maximisation pour le joueur ordinateur */
        meilleure_valeur = -1000000;
        
        for (i = 0; i < noeud->nb_enfants; i++) {
            /* Premier enfant: recherche avec fenêtre complète */
            if (i == 0) {
                valeur = nega_scout_endgame(noeud->enfants[i], profondeur - 1, alpha, beta, 0);
            }
            /* Autres enfants: d'abord recherche avec fenêtre nulle */
            else {
                valeur = nega_scout_endgame(noeud->enfants[i], profondeur - 1, alpha, alpha + 1, 0);
                
                /* Si la valeur est dans la fenêtre, refaire la recherche avec fenêtre complète */
                if (valeur > alpha && valeur < beta) {
                    valeur = nega_scout_endgame(noeud->enfants[i], profondeur - 1, alpha, beta, 0);
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
                valeur = nega_scout_endgame(noeud->enfants[i], profondeur - 1, alpha, beta, 1);
            }
            /* Autres enfants: d'abord recherche avec fenêtre nulle */
            else {
                valeur = nega_scout_endgame(noeud->enfants[i], profondeur - 1, beta - 1, beta, 1);
                
                /* Si la valeur est dans la fenêtre, refaire la recherche avec fenêtre complète */
                if (valeur > alpha && valeur < beta) {
                    valeur = nega_scout_endgame(noeud->enfants[i], profondeur - 1, alpha, beta, 1);
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

/* Fonction pour choisir le meilleur coup en utilisant l'algorithme d'Endgame - Version harmonisée */
Position choisir_meilleur_coup_endgame(arbre racine, int profondeur) {
    Position meilleur_coup;
    int i, valeur;
    int alpha = -1000000;
    int beta = 1000000;
    Joueur joueur_ordi = racine->plateau.ordinateur;
    int meilleure_valeur = -1000000; /* Toujours chercher à maximiser */
    /*int est_max = 1; L'ordinateur est toujours MAX car on évalue de son point de vue */
    
    /* Initialisation du meilleur coup */
    meilleur_coup.ligne = -1;
    meilleur_coup.colonne = -1;
    /* Vérifications de sécurité */
    if (racine == NULL || racine->nb_enfants == 0) {
        return meilleur_coup;
    }
    
    /* Trier les coups avec l'heuristique Fastest-First */
    ordonner_coups_endgame(racine, joueur_ordi);
    
    /* Parcourir tous les enfants de l'arbre */
    for (i = 0; i < racine->nb_enfants; i++) {
        /* Appliquer NegaScout sur chaque enfant */
        valeur = nega_scout_endgame(
            racine->enfants[i], 
            profondeur - 1, 
            alpha, 
            beta, 
            0 /* L'adversaire joue après notre coup */
        );
        
        /* Affichage du score pour chaque coup */
        printf("Coup (%d, %d) : Valeur NegaScout Endgame = %d\n", 
               racine->enfants[i]->coup.ligne, 
               racine->enfants[i]->coup.colonne, 
               valeur);
        
        /* Chercher toujours le maximum puisqu'on évalue du point de vue de l'ordinateur */
        if (valeur > meilleure_valeur) {
            meilleure_valeur = valeur;
            meilleur_coup = racine->enfants[i]->coup;
            
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

/* ************************************************************************* 
        VERSION DU ENDGAME MAIS AVEC MEMOIRE OPTIMISEE
************************************************************************* */
/* Fonction de mise à jour de la mémoire pour l'endgame */
void mettre_a_jour_memoire_endgame(size_t taille, int ajouter) {
    if (ajouter) {
        memoire_courante_endgame_optimisee += taille;
        memoire_totale_partie += taille;
        
        /* Mettre à jour la mémoire maximale utilisée si nécessaire */
        if (memoire_courante_endgame_optimisee > memoire_max_endgame_optimisee) {
            memoire_max_endgame_optimisee = memoire_courante_endgame_optimisee;
        }
    } else {
        memoire_courante_endgame_optimisee -= taille;
    }
}
/* Fonction pour ordonner les coups avec l'heuristique Fastest-First pour l'Endgame */
void ordonner_coups_endgame_optimise(Plateau *plateau, Position *coups, int nb_coups, Joueur joueur) {
    int i, j;
    int *scores;
    Joueur adversaire;
    Position temp;
    HistoriqueCoup historique;
    int score_tmp, mobilite_adversaire, stabilite_joueur;
    size_t taille_scores;
    
    /* Vérification des paramètres */
    if (nb_coups <= 1) {
        return;
    }
    
    /* Initialisation des variables */
    adversaire = (joueur == NOIR) ? BLANC : NOIR;
    scores = (int*)malloc(nb_coups * sizeof(int));
    
    if (scores == NULL) {
        printf("Erreur : allocation mémoire échouée pour les scores.\n");
        return;
    }
    /* Mettre à jour les compteurs de mémoire */
    taille_scores = nb_coups * sizeof(int);
    mettre_a_jour_memoire_endgame(taille_scores, 1);
    /* Calculer le score de chaque coup selon les critères de l'endgame */
    for (i = 0; i < nb_coups; i++) {
        /* Priorité maximale si le coup est un coin */
        if (est_coin(coups[i])) {
            scores[i] = 10000;
        } else {
            /* Jouer le coup temporairement pour évaluer sa valeur */
            jouer_coup_avec_historique(plateau, coups[i], joueur, &historique);
            /* Score basé sur la mobilité de l'adversaire (inversé) */
            mobilite_adversaire = calculer_mobilite_immediate(plateau, adversaire);
            /* Score basé sur la stabilité des pions du joueur */
            stabilite_joueur = evaluation_stabilite_etape7(plateau, joueur);
            /* Combiner les critères avec une forte pondération pour la mobilité */
            scores[i] = -mobilite_adversaire * 10 + stabilite_joueur;
            /* Annuler le coup */
            annuler_coup_avec_historique(plateau, &historique);
        }
    }
    /* Trier les coups par score décroissant (tri à bulle) , a la fin 
    ce tri a bulle est suffisant car on a pas trop de coups qui restent */
    for (i = 0; i < nb_coups - 1; i++) {
        for (j = 0; j < nb_coups - i - 1; j++) {
            if (scores[j] < scores[j + 1]) {
                /* Échanger les scores */
                score_tmp = scores[j];
                scores[j] = scores[j + 1];
                scores[j + 1] = score_tmp;
                /* Échanger les coups */
                temp = coups[j];
                coups[j] = coups[j + 1];
                coups[j + 1] = temp;
            }
        }
    }
    /* Libérer la mémoire et ajuster les compteurs */
    free(scores);
    mettre_a_jour_memoire_endgame(taille_scores, 0);
}
/* Fonction NegaScout optimisée pour l'Endgame */
int negascout_endgame_optimise(Plateau *plateau, int profondeur, int alpha, int beta, 
                              Joueur joueur_actif, Joueur joueur_ordi, 
                              HistoriqueCoup *historique) {
    int meilleure_valeur, valeur, valeur_fenetre;
    int nb_coups, nb_coups_adversaire;
    Position *coups_possibles, *coups_adversaire;
    int i;
    int est_max = (joueur_actif == joueur_ordi);
    HistoriqueCoup historique_local;
    size_t taille_coups_possibles, taille_coups_adversaire, taille_historique;
    Joueur adversaire = (joueur_actif == NOIR) ? BLANC : NOIR;
    int continuer_recherche = 1;
    int resultat;
    
    /* Incrémenter le compteur de nœuds explorés */
    compteur_noeuds_endgame_optimise++;
    
    /* Si la partie est terminée, évaluer directement avec le score exact */
    if (partie_terminee(plateau)) {
        return evaluation_plateau(plateau, joueur_ordi); /* Score exact */
    }

    /* Condition d'arrêt : profondeur atteinte */
    if (profondeur == 0) {
        return evaluation_ponderee7(plateau, joueur_ordi);
    }
    
    /* Générer tous les coups possibles */
    coups_possibles = generer_coups_possibles(plateau, joueur_actif, &nb_coups);
    
    /* Mettre à jour les compteurs de mémoire pour les coups possibles */
    taille_coups_possibles = sizeof(Position) * nb_coups;
    mettre_a_jour_memoire_endgame(taille_coups_possibles, 1);
    
    /* Si aucun coup possible, vérifier si l'adversaire peut jouer */
    if (nb_coups == 0) {
        coups_adversaire = generer_coups_possibles(plateau, adversaire, &nb_coups_adversaire);
        
        /* Mettre à jour les compteurs de mémoire pour les coups adverses */
        taille_coups_adversaire = sizeof(Position) * nb_coups_adversaire;
        mettre_a_jour_memoire_endgame(taille_coups_adversaire, 1);
        
        /* Si l'adversaire ne peut pas jouer non plus, la partie est terminée */
        if (nb_coups_adversaire == 0) {
            resultat = evaluation_plateau(plateau, joueur_ordi);
            
            /* Libérer la mémoire et ajuster les compteurs */
            mettre_a_jour_memoire_endgame(taille_coups_possibles, 0);
            mettre_a_jour_memoire_endgame(taille_coups_adversaire, 0);
            free(coups_possibles);
            free(coups_adversaire);
            
            return resultat;
        }
        
        /* Sinon, passer le tour et continuer avec l'adversaire */
        valeur = negascout_endgame_optimise(plateau, profondeur - 1, alpha, beta,
                                          adversaire, joueur_ordi, historique);
        
        /* Libérer la mémoire et ajuster les compteurs */
        mettre_a_jour_memoire_endgame(taille_coups_possibles, 0);
        mettre_a_jour_memoire_endgame(taille_coups_adversaire, 0);
        free(coups_possibles);
        free(coups_adversaire);
        
        return valeur;
    }
    
    /* Trier les coups avec l'heuristique Fastest-First pour l'endgame */
    ordonner_coups_endgame_optimise(plateau, coups_possibles, nb_coups, joueur_actif);
    
    /* Initialiser la meilleure valeur selon MAX ou MIN */
    if (est_max) {
        meilleure_valeur = -1000000;
    } else {
        meilleure_valeur = 1000000;
    }
    
    /* Explorer tous les coups possibles */
    for (i = 0; i < nb_coups && continuer_recherche; i++) {
        /* Jouer le coup et enregistrer l'historique */
        jouer_coup_avec_historique(plateau, coups_possibles[i], joueur_actif, &historique_local);
        
        /* Mettre à jour les compteurs de mémoire pour l'historique */
        taille_historique = sizeof(HistoriqueCoup);
        mettre_a_jour_memoire_endgame(taille_historique, 1);
        
        /* Premier enfant: recherche avec fenêtre complète */
        if (i == 0) {
            valeur = negascout_endgame_optimise(plateau, profondeur - 1, alpha, beta,
                                             adversaire, joueur_ordi, historique);
        } else {
            /* Autres enfants: d'abord recherche avec fenêtre nulle */
            if (est_max) {
                /* Pour MAX, on teste avec une fenêtre [alpha, alpha+1] */
                valeur_fenetre = negascout_endgame_optimise(plateau, profondeur - 1, alpha, alpha + 1,
                                                         adversaire, joueur_ordi, historique);
                
                /* Si la valeur est dans la fenêtre, refaire la recherche avec fenêtre complète */
                if (valeur_fenetre > alpha && valeur_fenetre < beta) {
                    valeur = negascout_endgame_optimise(plateau, profondeur - 1, alpha, beta,
                                                     adversaire, joueur_ordi, historique);
                } else {
                    valeur = valeur_fenetre;
                }
            } else {
                /* Pour MIN, on teste avec une fenêtre [beta-1, beta] */
                valeur_fenetre = negascout_endgame_optimise(plateau, profondeur - 1, beta - 1, beta,
                                                         adversaire, joueur_ordi, historique);
                
                /* Si la valeur est dans la fenêtre, refaire la recherche avec fenêtre complète */
                if (valeur_fenetre > alpha && valeur_fenetre < beta) {
                    valeur = negascout_endgame_optimise(plateau, profondeur - 1, alpha, beta,
                                                     adversaire, joueur_ordi, historique);
                } else {
                    valeur = valeur_fenetre;
                }
            }
        }
        
        /* Annuler le coup */
        annuler_coup_avec_historique(plateau, &historique_local);
        
        /* Réduire la mémoire courante après avoir libéré l'historique */
        mettre_a_jour_memoire_endgame(taille_historique, 0);
        
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
            continuer_recherche = 0;
        }
    }
    
    /* Libérer la mémoire des coups possibles et ajuster le compteur */
    mettre_a_jour_memoire_endgame(taille_coups_possibles, 0);
    free(coups_possibles);
    
    return meilleure_valeur;
}
/* Fonction pour choisir le meilleur coup en utilisant l'algorithme NegaScout optimisé pour l'Endgame */
Position choisir_meilleur_coup_endgame_optimise(Plateau *plateau, Joueur joueur_actif, int profondeur) {
    Position *coups_possibles;
    int nb_coups;
    int i, valeur;
    Position meilleur_coup;
    Position meilleur_coup_non_final = {-1, -1};
    Position meilleur_coup_final = {-1, -1};
    int alpha = -1000000;
    int beta = 1000000;
    int meilleure_valeur_non_finale = -1000000;
    int meilleure_valeur_finale = -1000000;
    HistoriqueCoup historique_local;
    Joueur joueur_ordi = joueur_actif;
    size_t taille_plateau, taille_coups, taille_historique;
    int trouve_position_finale = 0;
    int *est_position_finale;
    
    /* Réinitialiser les compteurs pour l'endgame mais conserver memoire_totale_partie */
    compteur_noeuds_endgame_optimise = 0;
    memoire_courante_endgame_optimisee = 0;
    memoire_max_endgame_optimisee = 0;
    
    printf("Endgame Optimisé - Profondeur: %d\n", profondeur);
    
    /* Calcul de la mémoire pour le plateau */
    taille_plateau = sizeof(Plateau);
    mettre_a_jour_memoire_endgame(taille_plateau, 1);
    
    /* Générer tous les coups possibles pour le joueur actif */
    coups_possibles = generer_coups_possibles(plateau, joueur_actif, &nb_coups);
    
    /* Calcul de la mémoire pour les coups possibles */
    taille_coups = nb_coups * sizeof(Position);
    mettre_a_jour_memoire_endgame(taille_coups, 1);
    
    /* Allouer de la mémoire pour stocker les informations sur les positions finales */
    est_position_finale = (int*)malloc(nb_coups * sizeof(int));
    mettre_a_jour_memoire_endgame(nb_coups * sizeof(int), 1);
    
    /* Initialiser le tableau */
    for (i = 0; i < nb_coups; i++) {
        est_position_finale[i] = 0;
    }
    
    /* Si aucun coup possible, retourner une position invalide */
    if (nb_coups == 0) {
        free(coups_possibles);
        free(est_position_finale);
        mettre_a_jour_memoire_endgame(nb_coups * sizeof(int), 0);
        meilleur_coup.ligne = -1;
        meilleur_coup.colonne = -1;
        return meilleur_coup;
    }
    
    /* Initialiser le meilleur coup au premier coup possible (sera remplacé) */
    meilleur_coup_non_final = coups_possibles[0];
    
    /* Explorer tous les coups possibles */
    for (i = 0; i < nb_coups; i++) {
        /* Jouer le coup et enregistrer l'historique */
        jouer_coup_avec_historique(plateau, coups_possibles[i], joueur_actif, &historique_local);
        
        /* Vérifier IMMÉDIATEMENT si ce coup mène à une position finale */
        if (partie_terminee(plateau)) {
            est_position_finale[i] = 1;
            trouve_position_finale = 1;
        }
        
        /* Calcul de la mémoire pour l'historique */
        taille_historique = sizeof(HistoriqueCoup);
        mettre_a_jour_memoire_endgame(taille_historique, 1);
        
        /* Appel à NegaScout avec le joueur adversaire */
        valeur = negascout_endgame_optimise(plateau, profondeur - 1, alpha, beta,
                                         (joueur_actif == NOIR) ? BLANC : NOIR, 
                                         joueur_ordi, &historique_local);
        
        /* Annuler le coup */
        annuler_coup_avec_historique(plateau, &historique_local);
        
        /* Réduire la mémoire courante après avoir libéré l'historique */
        mettre_a_jour_memoire_endgame(taille_historique, 0);
        
        /* Afficher le score pour chaque coup */
        printf("Coup (%d, %d) : Valeur NegaScout Endgame Optimisé = %d %s\n", 
               coups_possibles[i].ligne, 
               coups_possibles[i].colonne, 
               valeur,
               est_position_finale[i] ? "(Position finale)" : "");
        
        /* Traiter séparément les coups qui mènent à des positions finales */
        if (est_position_finale[i]) {
            /* Parmi les positions finales, prendre celle avec la meilleure évaluation */
            if (valeur > meilleure_valeur_finale) {
                meilleure_valeur_finale = valeur;
                meilleur_coup_final = coups_possibles[i];
            }
        } else {
            /* Parmi les positions non finales, prendre celle avec la meilleure évaluation */
            if (valeur > meilleure_valeur_non_finale) {
                meilleure_valeur_non_finale = valeur;
                meilleur_coup_non_final = coups_possibles[i];
                
                /* Mettre à jour alpha (uniquement pour les positions non-finales) */
                if (meilleure_valeur_non_finale > alpha) {
                    alpha = meilleure_valeur_non_finale;
                }
            }
        }
    }
    
    /* Afficher le nombre de nœuds explorés */
    printf("Nœuds explorés avec NegaScout Endgame Optimisé: %d\n", compteur_noeuds_endgame_optimise);
    
    /* CHOIX ABSOLU : Si une position finale est trouvée, la choisir peu importe sa valeur */
    if (trouve_position_finale) {
        printf("Position finale détectée! Coup choisi : (%d, %d)\n", 
               meilleur_coup_final.ligne, meilleur_coup_final.colonne);
        
        /* Afficher le coup choisi avec sa valeur finale */
        printf("Meilleur coup choisi : (%d, %d) avec valeur = %d\n", 
               meilleur_coup_final.ligne, 
               meilleur_coup_final.colonne, 
               meilleure_valeur_finale);
               
        /* Libérer la mémoire et ajuster les compteurs */
        mettre_a_jour_memoire_endgame(nb_coups * sizeof(int), 0);
        mettre_a_jour_memoire_endgame(taille_coups, 0);
        free(est_position_finale);
        free(coups_possibles);
        
        return meilleur_coup_final;
    } else {
        /* Aucune position finale trouvée, utiliser le meilleur coup non final */
        /* Afficher le coup choisi avec sa valeur non finale */
        printf("Meilleur coup choisi : (%d, %d) avec valeur = %d\n", 
               meilleur_coup_non_final.ligne, 
               meilleur_coup_non_final.colonne, 
               meilleure_valeur_non_finale);
               
        /* Libérer la mémoire et ajuster les compteurs */
        mettre_a_jour_memoire_endgame(nb_coups * sizeof(int), 0);
        mettre_a_jour_memoire_endgame(taille_coups, 0);
        free(est_position_finale);
        free(coups_possibles);
        
        return meilleur_coup_non_final;
    }
    
    /* Afficher les statistiques de mémoire  ... AFFICHER POUR DEBUGER 
    printf("Mémoire utilisée par l'Endgame: %zu octets (%.2f Ko)\n", 
           memoire_courante_endgame_optimisee, 
           memoire_courante_endgame_optimisee / 1024.0);
    printf("max de mémoire Endgame: %zu octets (%.2f Ko)\n",
           memoire_max_endgame_optimisee,
           memoire_max_endgame_optimisee / 1024.0);
    printf("Mémoire totale partie (NegaScout + Endgame): %zu octets (%.2f Ko)\n", 
           memoire_totale_partie, 
           memoire_totale_partie / 1024.0); */
}
/* 
 * Fonction de transition vers l'algorithme Endgame
 * Cette fonction est appelée lorsque le jeu entre dans la phase finale
 * Elle gère le transfert des statistiques de mémoire de NegaScout à Endgame
 */
void transition_vers_endgame(size_t memoire_utilisee_negascout) {
    /* Initialiser la mémoire totale de la partie avec celle déjà utilisée par NegaScout */
    memoire_totale_partie = memoire_utilisee_negascout;
    
    /* Réinitialiser les compteurs spécifiques à l'Endgame */
    compteur_noeuds_endgame_optimise = 0;
    memoire_courante_endgame_optimisee = 0;
    memoire_max_endgame_optimisee = 0;
    
    printf("--------------------------------------------------\n");
    printf("Transition vers l'algorithme Endgame optimisé\n");
    printf("Mémoire utilisée avant Endgame: %lu octets (%.2f Ko)\n",
           (unsigned long)memoire_totale_partie, 
           memoire_totale_partie / 1024.0);
    printf("--------------------------------------------------\n");
}
#endif /* _ENDGAME_C_ */


