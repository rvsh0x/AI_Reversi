#ifndef _REVERSI_H_
#define _REVERSI_H_

#define TAILLE_PLATEAU 8


/* Enumération pour représenter les types de joueurs et les cases */
typedef enum {
    VIDE = 0,  /* Case vide */
    NOIR,      /* Joueur noir */
    BLANC      /* Joueur blanc */
} Joueur;

/* Structure pour représenter une position sur le plateau */
typedef struct {
    int ligne;
    int colonne;
} Position;

/* Structure pour le plateau de jeu */
typedef struct {
    Joueur cases[TAILLE_PLATEAU][TAILLE_PLATEAU];
    Joueur joueur_humain;    
    Joueur ordinateur;       
} Plateau;

/* Fonctions principales */
void initialiser_plateau(Plateau *plateau);
int coup_valide(Plateau *plateau, Position pos, Joueur joueur);
int compter_pions_a_retourner(Plateau *plateau, Position pos, Joueur joueur);
void retourner_pions(Plateau *plateau, Position pos, Joueur joueur);
Position coup_ordinateur(Plateau *plateau);
int position_gagnante(Plateau *plateau);
void appliquer_coup(Plateau *plateau, Position pos, Joueur joueur);
void jouer_coup(Plateau *plateau, Position pos, Joueur joueur);
void annuler_coup(Plateau *plateau, Position pos, Joueur joueur);
void compter_pions(Plateau *plateau, int *score_noir, int *score_blanc);
Position* generer_coups_possibles(Plateau *plateau, Joueur joueur, int *nb_coups);
int partie_terminee(Plateau *plateau);
void jouer_partie(Plateau *plateau, int mode_graphique);
/*void jouer_partie_IA_niveau_1(Plateau *plateau, int mode_graphique);*/

/* Fonction utilitaire */
int est_dans_plateau(Position pos);

#endif /* _REVERSI_H_ */