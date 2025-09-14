#ifndef _JEU_H_
#define _JEU_H_

#include <stdio.h>
#include "../include/reversi.h"
#include "../include/affichage.h"
#include "../include/ia.h"
#include "../include/evaluation.h"
#include "../include/endgame.h"
#include "../include/nega_memoire.h"
#include "../include/affichage_graphique.h"

/* Structure pour configurer l'IA */
typedef struct {
    int niveau;            /* Niveau de l'IA (1-6) */
    int profondeur;        /* Profondeur de l'arbre pour les niveaux 3+ */
    int mode_graphique;    /* Mode d'affichage (0=terminal, 1=graphique) */
    int profondeur_endgame; /* Augmentation de profondeur pour l'endgame */
} ConfigurationJeu;

/* Prototype des fonctions de jeu */
void jouer_partie_unifiee(Plateau *plateau, ConfigurationJeu *config);

#endif /* _JEU_H_ */