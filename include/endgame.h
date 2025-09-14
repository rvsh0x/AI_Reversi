#ifndef _ENDGAME_H_
#define _ENDGAME_H_

#include <stdio.h>
#include <stdlib.h>

#include "reversi.h"
#include "ia.h"
#include "evaluation.h"

/* Variables globales */
extern int compteur_noeuds_endgame; /* Compteur de nœuds explorés dans l'endgame */

extern int compteur_noeuds_endgame_optimise;
extern size_t memoire_totale_endgame_optimisee; 
extern size_t memoire_courante_endgame_optimisee;
extern size_t memoire_max_endgame_optimisee;
extern size_t memoire_totale_partie;

/* qq petits soucis dans la version avec un arbre 
    -> pas reglé car on a decidé de ne pas l'utiliser */
arbre creer_arbre_position_endgame(Plateau *plateau, Joueur joueur, int profondeur);
int est_coin(Position coup);
void ordonner_coups_endgame(arbre noeud, Joueur joueur);
int nega_scout_endgame(arbre noeud, int profondeur, int alpha, int beta, int est_max);
Position choisir_meilleur_coup_endgame(arbre racine, int profondeur);

/* Version optimisée de l'algorithme d'Endgame -> Mémoire optimisée */
void ordonner_coups_endgame_optimise(Plateau *plateau, Position *coups, int nb_coups, Joueur joueur);
int negascout_endgame_optimise(Plateau *plateau, int profondeur, int alpha, int beta, 
    Joueur joueur_actif, Joueur joueur_ordi, 
    HistoriqueCoup *historique);
Position choisir_meilleur_coup_endgame_optimise(Plateau *plateau, Joueur joueur_actif, int profondeur);
void transition_vers_endgame(size_t memoire_utilisee_negascout);
#endif /* _ENDGAME_H_ */