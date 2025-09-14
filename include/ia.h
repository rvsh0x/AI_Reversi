#ifndef _IA_H_
#define _IA_H_

#include "reversi.h"

/* pour les compteurs et les tests : */
extern int compteur_noeuds_minimax; 
extern int compteur_noeuds_alphabeta; 
extern int compteur_noeuds_negascout; 
extern int compteur_noeuds_memoire;
extern size_t memoire_totale_arbre; 
extern size_t memoire_arbre_courant; 
extern size_t memoire_courante_optimisee;
extern size_t memoire_totale_optimisee; 
extern size_t memoire_max_optimisee;


/* Structure pour représenter un nœud de l'arbre */
typedef struct Noeud {
    Plateau plateau;           /* Plateau correspondant à ce nœud */
    Position coup;             /* Coup joué pour arriver à ce nœud */
    struct Noeud **enfants;    /* Liste des enfants (nœuds suivants) */
    int nb_enfants;            /* Nombre d'enfants */
    int evaluation;            /* Valeur d'évaluation de ce nœud */
} Noeud;

typedef Noeud* arbre;


/*****************************************************************************************
 * Fonctions de l'IA pour l'étape 3 du projet : 
 * IA : arbre avec profondeur de 2 et évaluation simple 
******************************************************************************************/
void reinitialiser_memoire_arbre_courant();
arbre creer_noeud(Plateau *plateau, Position coup);
arbre creer_arbre_position(Plateau *plateau, Joueur joueur, int profondeur);
int minimax(arbre a, int profondeur, int est_max);
Position choisir_meilleur_coup(arbre a);
void liberer_arbre(arbre a);

/*****************************************************************************************
 * Fonctions de l'IA pour l'étape 4 du projet : 
 * IA : arbre avec plus grandeur profondeur et évaluation plus complexe 
******************************************************************************************/
int minimax_etape4(arbre a, int profondeur, int est_max);
Position choisir_meilleur_coup_etape4(arbre a, int profondeur);

/******************************************************************************************
 * Fonctions de l'IA pour l'étape 5 du projet : 
 * IA : arbre avec optimisation alpha-bêta et évaluation plus complexe
******************************************************************************************/
int alpha_beta(arbre a, int profondeur, int alpha, int beta, int est_max);
/* alpha_beta dans reversi : 
    - si un coup de l'IA garatit un score supérieur à beta pour l'humain,
    on arrête l'exploration des autres coups possibles pour l'IA 
    - si un coup de l'humain garatit un score inférieur à alpha pour l'IA,
    on arrête l'exploration des autres coups possibles pour l'humain */
Position choisir_meilleur_coup_alphabeta(arbre a, int profondeur);
/******************************************************************************************
 * Fonctions de l'IA pour l'étape 6 du projet : 
 * IA : arbre avec optimisation memoire et utilisation d'un seul plateau
******************************************************************************************/
/* Structure pour stocker l'historique des coups */
typedef struct {
    Position position;         /* Position du coup joué */
    Joueur pions_retournes[TAILLE_PLATEAU * TAILLE_PLATEAU][2];  /* Positions des pions retournés */
    int nb_pions_retournes;    /* Nombre de pions retournés */
} HistoriqueCoup;

/* Fonctions pour la gestion d'un seul plateau */
Position alpha_beta_memoire_optimisee(Plateau *plateau, Joueur joueur_actif, int profondeur);
int alpha_beta_optimise(Plateau *plateau, int profondeur, int alpha, int beta, 
                        Joueur joueur_actif, Joueur joueur_ordi, 
                        HistoriqueCoup *historique, int *compteur);
void jouer_coup_avec_historique(Plateau *plateau, Position pos, Joueur joueur, 
                               HistoriqueCoup *historique);
void annuler_coup_avec_historique(Plateau *plateau, HistoriqueCoup *historique);
void afficher_statistiques_finales();

/******************************************************************************************
 * Fonctions de l'IA pour l'étape 7 du projet : 
 * IA :  avec optimisations et initatives personnelles
******************************************************************************************/
int nega_scout_arbre_sans_tri(arbre noeud, int profondeur, int alpha, int beta, int est_max);
int nega_scout_arbre_avec_tri(arbre noeud, int profondeur, int alpha, int beta, int est_max);
/* Gestion des coups */
void ordonner_coups_statique(arbre noeud); /* Tri des coups selon une table de priorité */

/* Choix du meilleur coup */
Position choisir_meilleur_coup_avec_tri(arbre arbre_jeu, int profondeur); /* Choix du meilleur coup avec tri */
Position choisir_meilleur_coup_sans_tri(arbre arbre_jeu, int profondeur); /* Choix du meilleur coup sans tri */

/* avec un tri dynamique : */
void ordonner_coups_dynamique(arbre noeud, Joueur joueur); /* Tri dynamique des coups */
int nega_scout_arbre_dynamique(arbre noeud, int profondeur, int alpha, int beta, int est_max);
Position choisir_meilleur_coup_dynamique(arbre arbre_jeu, int profondeur); /* Choix du meilleur coup avec tri dynamique */

#endif /* _IA_H_ */