#ifndef _NEGA_MEMOIRE_H_
#define _NEGA_MEMOIRE_H_

#include "ia.h"
#include "evaluation.h"

/* compteurs globaux */
extern int compteur_noeuds_negascout_optimise;
extern int compteur_noeuds_negascout_tri_statique_optimise;
extern int compteur_noeuds_negascout_tri_dynamique_optimise;
extern size_t memoire_totale_negascout_optimisee;
extern size_t memoire_courante_negascout_optimisee;

/* Prototypes des fonctions */
void ordonner_coups_statique_optimise(Position *coups, int nb_coups);
void ordonner_coups_dynamique_optimise(Plateau *plateau, Position *coups, int nb_coups, Joueur joueur);

int negascout_optimise_sans_tri(Plateau *plateau, int profondeur, int alpha, int beta,
    Joueur joueur_actif, Joueur joueur_ordi, 
    HistoriqueCoup *historique, int *compteur);

int negascout_optimise_tri_statique(Plateau *plateau, int profondeur, int alpha, int beta,
    Joueur joueur_actif, Joueur joueur_ordi, 
    HistoriqueCoup *historique, int *compteur);

int negascout_optimise_tri_dynamique(Plateau *plateau, int profondeur, int alpha, int beta,
    Joueur joueur_actif, Joueur joueur_ordi, 
    HistoriqueCoup *historique, int *compteur);

Position negascout_memoire_optimisee_sans_tri(Plateau *plateau, Joueur joueur_actif, int profondeur);
Position negascout_memoire_optimisee_tri_statique(Plateau *plateau, Joueur joueur_actif, int profondeur);
Position negascout_memoire_optimisee_tri_dynamique(Plateau *plateau, Joueur joueur_actif, int profondeur);












#endif /* _NEGA_MEMOIRE_H_ */