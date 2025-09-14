#ifndef _AFFICHAGE_GRAPHIQUE_H_
#define _AFFICHAGE_GRAPHIQUE_H_

#include "reversi.h"

/* Constantes pour l'affichage graphique */
#define TAILLE_FENETRE 600
#define LARGEUR_BORDURE 40             /* Largeur de la bordure marron */
#define TAILLE_CASE (TAILLE_FENETRE / 10)
#define MARGE 60
#define TAILLE_PLATEAU_GRAPHIQUE (TAILLE_CASE * 8)
#define RAYON_PION (TAILLE_CASE / 2 - 5)
#define RAYON_POINT_PLACEMENT 5        /* Rayon des points rouges indiquant les placements valides */

/* Couleurs */
#define COULEUR_FOND MLV_rgba(46, 139, 87, 255)      /* Vert forêt */
#define COULEUR_BORDURE MLV_rgba(139, 69, 19, 255)   /* Brun */
#define COULEUR_LABELS MLV_rgba(255, 215, 0, 255)    /* Or */
#define COULEUR_BLANC MLV_rgba(255, 255, 255, 255)
#define COULEUR_NOIR MLV_rgba(0, 0, 0, 255)
#define COULEUR_QUADRILLAGE MLV_rgba(44, 110, 73, 255) /* Vert un peu plus foncé */
#define COULEUR_COUP_VALIDE MLV_rgba(255, 0, 0, 255)   /* Rouge */

/* Fonctions d'affichage graphique */
void initialiser_fenetre_graphique();
void afficher_plateau_graphique(Plateau *plateau, int est_tour_humain);
int recuperer_clic(Position *coup);
void afficher_coup_ordinateur_graphique(Position coup);
void afficher_resultat_graphique(Plateau *plateau);
void fermer_fenetre_graphique();

#endif /* _AFFICHAGE_GRAPHIQUE_H_ */