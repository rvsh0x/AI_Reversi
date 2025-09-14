#ifndef _JEU_C_
#define _JEU_C_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../include/jeu.h"

/* Fonction unifiée pour jouer une partie avec n'importe quelle version d'IA */
void jouer_partie_unifiee(Plateau *plateau, ConfigurationJeu *config) {
    Position coup;
    Position pos;
    Joueur joueur_actuel;
    int tour = 1; /* 1 pour Noir, 0 pour Blanc */
    int passe_consecutif = 0;
    int coup_possible;
    int i, j;
    arbre arbre_jeu;
    const char* phase = NULL;
    int profondeur_ajustee;
    int coup_valide_trouve;
    int clic_valide;
    /* Variable ajoutée pour déterminer si c'est le tour du joueur humain */
    int est_tour_humain;

    /* Initialisation de l'interface graphique si nécessaire */
    if (config->mode_graphique) {
        initialiser_fenetre_graphique();
    }

    /* Boucle principale du jeu */
    while (!partie_terminee(plateau) && passe_consecutif < 2) {
        /* Affichage du plateau */
        if (config->mode_graphique) {
            /* Déterminer si c'est le tour du joueur humain pour l'affichage des coups valides */
            joueur_actuel = tour ? NOIR : BLANC;
            est_tour_humain = (joueur_actuel == plateau->joueur_humain);
            afficher_plateau_graphique(plateau, est_tour_humain); 
        } else {
            afficher_plateau(plateau);
        }

        /* Pour l'IA de niveau 6, déterminer la phase de jeu */
        if (config->niveau == 6) {
            phase = determiner_phase_partie(plateau);
            if (strcmp(phase, "endgame") == 0) {
                printf("Phase de fin de partie (Endgame) détectée!\n");
            }
        }

        joueur_actuel = tour ? NOIR : BLANC;
        coup_possible = 0;

        /* Vérifier s'il y a des coups possibles sans break */
        i = 0;
        coup_valide_trouve = 0;
        while (i < TAILLE_PLATEAU && !coup_valide_trouve) {
            j = 0;
            while (j < TAILLE_PLATEAU && !coup_valide_trouve) {
                pos.ligne = i;
                pos.colonne = j;
                if (coup_valide(plateau, pos, joueur_actuel)) {
                    coup_possible = 1;
                    coup_valide_trouve = 1;
                }
                j++;
            }
            i++;
        }

        if (!coup_possible) {
            printf("Le joueur %c doit passer son tour.\n", (joueur_actuel == NOIR) ? 'N' : 'B');
            passe_consecutif++;
            tour = !tour;
        } else {
            passe_consecutif = 0;

            if ((joueur_actuel == NOIR && plateau->joueur_humain == NOIR) || 
                (joueur_actuel == BLANC && plateau->joueur_humain == BLANC)) {
                /* Tour du joueur humain */
                do {
                    if (config->mode_graphique) {
                        printf("Cliquez sur une case pour jouer.\n");
                        clic_valide = 0;
                        do {
                            clic_valide = recuperer_clic(&coup);
                            if (!clic_valide) {
                                printf("Clic en dehors du plateau. Veuillez réessayer.\n");
                                /* C'est le tour du joueur humain, donc on passe 1 */
                                afficher_plateau_graphique(plateau, 1);
                            }
                        } while (!clic_valide);
                    } else {
                        demander_coup(&coup);
                    }

                    if (!coup_valide(plateau, coup, joueur_actuel)) {
                        printf("Coup invalide. Veuillez réessayer.\n");
                        if (config->mode_graphique) {
                            /* C'est le tour du joueur humain, donc on passe 1 */
                            afficher_plateau_graphique(plateau, 1);
                        }
                    }
                } while (!coup_valide(plateau, coup, joueur_actuel));

                jouer_coup(plateau, coup, joueur_actuel);
            } else {
                /* Tour de l'ordinateur - selon le niveau d'IA configuré */
                switch (config->niveau) {
                    case 1: /* Aléatoire */
                        coup = coup_ordinateur(plateau);
                        break;
                        
                    case 2: /* Avec arbre fixe de profondeur 2 */
                        arbre_jeu = creer_arbre_position(plateau, joueur_actuel, 2);
                        coup = choisir_meilleur_coup(arbre_jeu);
                        liberer_arbre(arbre_jeu);
                        break;
                        
                    case 3: /* Avec arbre de profondeur variable */
                        arbre_jeu = creer_arbre_position(plateau, joueur_actuel, config->profondeur);
                        coup = choisir_meilleur_coup_etape4(arbre_jeu, config->profondeur);
                        liberer_arbre(arbre_jeu);
                        break;
                        
                    case 4: /* Avec Alpha-Beta */
                        reinitialiser_memoire_arbre_courant();
                        arbre_jeu = creer_arbre_position(plateau, joueur_actuel, config->profondeur);
                        coup = choisir_meilleur_coup_alphabeta(arbre_jeu, config->profondeur);
                        liberer_arbre(arbre_jeu);
                        break;
                    case 5: /* Avec optimisation mémoire */
                        printf("IA utilise l'algorithme Alpha-Beta avec optimisation mémoire, profondeur %d\n", config->profondeur);
                        coup = alpha_beta_memoire_optimisee(plateau, joueur_actuel, config->profondeur);
                        /*printf("Mémoire utilisée: %.2f Ko\n", memoire_totale_optimisee );*/
                        break;
                    case 6: /* IA avancée avec adaptation phase de jeu */
                        if (strcmp(phase, "endgame") == 0) {
                            /* Phase d'endgame, augmenter la profondeur et utiliser l'algorithme spécialisé */
                            profondeur_ajustee = config->profondeur + config->profondeur_endgame;
                            printf("IA utilise l'algorithme Endgame optimisé avec profondeur %d\n", profondeur_ajustee);
                            
                            /* Version avec arbre (commentée) */
                            /*
                            compteur_noeuds_endgame = 0;
                            arbre_jeu = creer_arbre_position_endgame(plateau, joueur_actuel, profondeur_ajustee);
                            coup = choisir_meilleur_coup_endgame(arbre_jeu, profondeur_ajustee);
                            printf("Nœuds explorés en Endgame: %d\n", compteur_noeuds_endgame);
                            liberer_arbre(arbre_jeu);
                            */
                            
                            /*transition_vers_endgame(memoire_totale_negascout_optimisee);*/
                            coup = choisir_meilleur_coup_endgame_optimise(plateau, joueur_actuel, profondeur_ajustee);
                            /*printf("Nœuds explorés en Endgame: %d\n", compteur_noeuds_endgame_optimise);*/
                        } else {
                            /* Phase normale, utiliser l'algorithme avancé standard */
                            printf("IA utilise l'algorithme NegaScout avec tri statique, profondeur %d\n", config->profondeur);
                            
                            /* Version avec arbre (commentée) */
                            /*
                            compteur_noeuds_negascout = 0;
                            arbre_jeu = creer_arbre_position(plateau, joueur_actuel, config->profondeur);
                            coup = choisir_meilleur_coup_avec_tri(arbre_jeu, config->profondeur);
                            printf("Nœuds explorés avec NegaScout: %d\n", compteur_noeuds_negascout);
                            liberer_arbre(arbre_jeu);
                            */
                            
                            /* Version optimisée en mémoire */
                            coup = negascout_memoire_optimisee_tri_statique(plateau, joueur_actuel, config->profondeur);
                            /*printf("Mémoire utilisée: %.2f Ko\n", memoire_totale_negascout_optimisee );*/
                        }
                        break;
                        
                    default:
                        /* Ne devrait jamais arriver grâce à la validation dans main() */
                        coup = coup_ordinateur(plateau);
                        break;
                }

                if (config->mode_graphique) {
                    afficher_coup_ordinateur_graphique(coup); 
                } else {
                    afficher_coup_ordinateur(coup);
                }
                jouer_coup(plateau, coup, joueur_actuel);
            }

            tour = !tour; /* Passage au joueur suivant */
        }
    }

    /* Affichage du résultat final */
    if (config->mode_graphique) {
        afficher_resultat_graphique(plateau); 
        /* fermer_fenetre_graphique();*/
        afficher_statistiques_finales();
    } else {
        afficher_plateau(plateau);
        afficher_resultat_final(plateau);
        afficher_statistiques_finales();
    }
}

#endif /* _JEU_C_ */