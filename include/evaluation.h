#ifndef _EVALUATION_H_
#define _EVALUATION_H_

#include <stdio.h>
#include <stdlib.h>

#include "ia.h"
#include "reversi.h"
#include "endgame.h"

/* Fichier pour les fonctions d'évaluation de l'IA */

/* Fonctions d'évaluation de caractéristiques spécifiques */
int evaluation_plateau(Plateau *plateau, Joueur joueur); /* Évaluation simple du plateau -> NB de pions */
int evaluation_etape4(Plateau *plateau, Joueur joueur); /* Évaluation de l'étape 4 -> elle prend en compte les ensembles de 2 et 3 pions consécutifs */
int evaluation_positions_strategiques(Plateau *plateau, Joueur joueur);
int evaluation_etape5(Plateau *plateau, Joueur joueur); 
/* Améliorations de l'évaluation */
int evaluation_mobilite(Plateau *plateau, Joueur joueur);
int evaluation_stabilite_etape7(Plateau *plateau, Joueur joueur);
int evaluation_disk_square(Plateau *plateau, Joueur joueur);
int evaluation_parite(Plateau *plateau, Joueur joueur);
int evaluation_ensembles_consecutifs(Plateau *plateau, Joueur joueur);

/* Fonction principale d'évaluation pondérée */
int evaluation_ponderee7(Plateau *plateau, Joueur joueur);

/* Fonctions auxiliaires */
/* pour la mobilité */
int calculer_mobilite_immediate(Plateau *plateau, Joueur joueur);
int compter_pions_frontiere(Plateau *plateau, Joueur joueur);
int calculer_mobilite_potentielle(Plateau *plateau, Joueur joueur);
/* pour la stabilite -> etape 7 : */
int pion_stable_etape7(Plateau *plateau, int ligne, int colonne);
void propager_stabilite_etape7(Plateau *plateau, int est_stable[TAILLE_PLATEAU][TAILLE_PLATEAU], 
    int est_semi_stable[TAILLE_PLATEAU][TAILLE_PLATEAU], Joueur joueur);

const char* determiner_phase_partie(Plateau *plateau);
int calculer_ensembles_consecutifs(Plateau *plateau, Joueur joueur, int taille);

#endif /* _EVALUATION_H_ */