#ifndef _AFFICHAGE_GRAPHIQUE_C_
#define _AFFICHAGE_GRAPHIQUE_C_

#include <MLV/MLV_all.h>
#include <stdio.h>
#include "../include/affichage_graphique.h"
#include "../include/reversi.h"

/* Fonction pour initialiser la fenêtre graphique */
void initialiser_fenetre_graphique() {
    /* Création de la fenêtre uniquement - pas d'initialisation audio */
    MLV_create_window("Reversi", "Reversi", TAILLE_FENETRE, TAILLE_FENETRE);
    MLV_clear_window(MLV_COLOR_GRAY);
    MLV_actualise_window();
    
    printf("Fenêtre graphique initialisée\n");
}

/* Fonction pour afficher le plateau de jeu */
void afficher_plateau_graphique(Plateau *plateau, int est_tour_humain) {
    int i, j;
    char score[50];
    char lettre[2] = "A";
    char chiffre[2] = "1";
    Position pos;
    int compte_noir,compte_blanc ;

    
    /* Effacer la fenêtre et remettre le fond gris */
    MLV_clear_window(MLV_COLOR_GRAY);
    
    /* Dessiner le fond du plateau (vert) avec bordure (brun) */
    MLV_draw_filled_rectangle(MARGE - LARGEUR_BORDURE, MARGE - LARGEUR_BORDURE, 
                             TAILLE_PLATEAU_GRAPHIQUE + 2*LARGEUR_BORDURE, 
                             TAILLE_PLATEAU_GRAPHIQUE + 2*LARGEUR_BORDURE, 
                             COULEUR_BORDURE);
    
    MLV_draw_filled_rectangle(MARGE, MARGE, 
                             TAILLE_PLATEAU_GRAPHIQUE, 
                             TAILLE_PLATEAU_GRAPHIQUE, 
                             COULEUR_FOND);
    
    /* Dessiner le quadrillage */
    /* Lignes horizontales */
    for (i = 0; i <= TAILLE_PLATEAU; i++) {
        MLV_draw_line(MARGE, MARGE + i * TAILLE_CASE, 
                     MARGE + TAILLE_PLATEAU_GRAPHIQUE, MARGE + i * TAILLE_CASE,
                     COULEUR_QUADRILLAGE);
    }
    
    /* Lignes verticales */
    for (j = 0; j <= TAILLE_PLATEAU; j++) {
        MLV_draw_line(MARGE + j * TAILLE_CASE, MARGE,
                     MARGE + j * TAILLE_CASE, MARGE + TAILLE_PLATEAU_GRAPHIQUE,
                     COULEUR_QUADRILLAGE);
    }
    
    /* Afficher les lettres (colonnes) en haut et en bas dans la bordure marron */
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        lettre[0] = 'A' + i;
        
        /* Lettres en haut dans la bordure */
        MLV_draw_text(MARGE + i * TAILLE_CASE + TAILLE_CASE/2 - 5, 
                     MARGE - LARGEUR_BORDURE/2 - 8, 
                     lettre, 
                     COULEUR_LABELS);
        
        /* Lettres en bas dans la bordure */
        MLV_draw_text(MARGE + i * TAILLE_CASE + TAILLE_CASE/2 - 5, 
                     MARGE + TAILLE_PLATEAU_GRAPHIQUE + LARGEUR_BORDURE/2 - 8, 
                     lettre, 
                     COULEUR_LABELS);
    }
    
    /* Afficher les chiffres (lignes) à gauche et à droite dans la bordure marron */
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        chiffre[0] = '1' + i;
        
        /* Chiffres à gauche dans la bordure */
        MLV_draw_text(MARGE - LARGEUR_BORDURE/2 - 5, 
                     MARGE + i * TAILLE_CASE + TAILLE_CASE/2 - 8, 
                     chiffre, 
                     COULEUR_LABELS);
        
        /* Chiffres à droite dans la bordure */
        MLV_draw_text(MARGE + TAILLE_PLATEAU_GRAPHIQUE + LARGEUR_BORDURE/2 - 5, 
                     MARGE + i * TAILLE_CASE + TAILLE_CASE/2 - 8, 
                     chiffre, 
                     COULEUR_LABELS);
    }
    
    /* Dessiner les pions */
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            if (plateau->cases[i][j] != VIDE) {
                if (plateau->cases[i][j] == NOIR) {
                    MLV_draw_filled_circle(MARGE + j * TAILLE_CASE + TAILLE_CASE/2, 
                                         MARGE + i * TAILLE_CASE + TAILLE_CASE/2, 
                                         RAYON_PION, 
                                         COULEUR_NOIR);
                } else {
                    MLV_draw_filled_circle(MARGE + j * TAILLE_CASE + TAILLE_CASE/2, 
                                         MARGE + i * TAILLE_CASE + TAILLE_CASE/2, 
                                         RAYON_PION, 
                                         COULEUR_BLANC);
                }
            }
        }
    }
    
    /* Dessiner des points rouges pour indiquer où le joueur peut jouer, seulement si c'est son tour */
    if (est_tour_humain) {
        for (i = 0; i < TAILLE_PLATEAU; i++) {
            for (j = 0; j < TAILLE_PLATEAU; j++) {
                pos.ligne = i;
                pos.colonne = j;
                if (plateau->cases[i][j] == VIDE && coup_valide(plateau, pos, plateau->joueur_humain)) {
                    MLV_draw_filled_circle(MARGE + j * TAILLE_CASE + TAILLE_CASE/2, 
                                         MARGE + i * TAILLE_CASE + TAILLE_CASE/2, 
                                         RAYON_POINT_PLACEMENT, 
                                         COULEUR_COUP_VALIDE);
                }
            }
        }
    }
    
    /* Afficher le score actuel */
    compte_noir = 0 ;
    compte_blanc = 0;
    
    /* Compte les pions de chaque couleur */
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            if (plateau->cases[i][j] == NOIR) {
                compte_noir++;
            } else if (plateau->cases[i][j] == BLANC) {
                compte_blanc++;
            }
        }
    }
    
    /* Affichage du score actuel en haut */
    sprintf(score, "Score: Noir %d - Blanc %d", compte_noir, compte_blanc);
    MLV_draw_text(MARGE, MARGE - LARGEUR_BORDURE - 20, score, COULEUR_LABELS);
    
    MLV_actualise_window();
}

/* Fonction pour récupérer un clic de l'utilisateur et le convertir en position */
int recuperer_clic(Position *coup) {
    int x, y;
    
    /* Attente d'un clic souris */
    MLV_wait_mouse(&x, &y);
    
    /* Vérifier si le clic est dans la zone du plateau */
    if (x < MARGE || x >= MARGE + TAILLE_PLATEAU_GRAPHIQUE || 
        y < MARGE || y >= MARGE + TAILLE_PLATEAU_GRAPHIQUE) {
        return 0; /* Clic en dehors du plateau */
    }
    
    /* Convertir les coordonnées du clic en indices de case */
    coup->colonne = (x - MARGE) / TAILLE_CASE;
    coup->ligne = (y - MARGE) / TAILLE_CASE;
    
    return 1; /* Clic valide */
}

/* Fonction pour afficher le coup de l'ordinateur */
void afficher_coup_ordinateur_graphique(Position coup) {
    char message[50];
    
    /* Formatage du message */
    sprintf(message, "L'ordinateur joue en %d%c", coup.ligne + 1, 'A' + coup.colonne);
    
    /* Affichage du message */
    MLV_draw_filled_rectangle(MARGE, MARGE - LARGEUR_BORDURE - 50, 
                             TAILLE_PLATEAU_GRAPHIQUE/2, 25, 
                             MLV_COLOR_WHITE);
    MLV_draw_text(MARGE + 10, MARGE - LARGEUR_BORDURE - 45, message, MLV_COLOR_BLACK);
    MLV_actualise_window();
    
    /* Pause pour que le joueur puisse voir le message */
    MLV_wait_milliseconds(1000);
}

/* Fonction pour afficher le résultat final */
void afficher_resultat_graphique(Plateau *plateau) {
    int compte_noir = 0, compte_blanc = 0;
    int i, j;
    int x, y;
    char message[100];
    
    /* Compte les pions de chaque couleur */
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            if (plateau->cases[i][j] == NOIR) {
                compte_noir++;
            } else if (plateau->cases[i][j] == BLANC) {
                compte_blanc++;
            }
        }
    }
    
    /* Créer un fond semi-transparent sur le plateau */
    MLV_draw_filled_rectangle(MARGE - LARGEUR_BORDURE, MARGE - LARGEUR_BORDURE,
                             TAILLE_PLATEAU_GRAPHIQUE + 2*LARGEUR_BORDURE,
                             TAILLE_PLATEAU_GRAPHIQUE + 2*LARGEUR_BORDURE,
                             MLV_rgba(0, 0, 0, 128)); /* Noir semi-transparent */
    
    /* Créer une boîte pour le message final */
    MLV_draw_filled_rectangle(MARGE + TAILLE_PLATEAU_GRAPHIQUE/4, MARGE + TAILLE_PLATEAU_GRAPHIQUE/3,
                             TAILLE_PLATEAU_GRAPHIQUE/2, TAILLE_PLATEAU_GRAPHIQUE/4,
                             MLV_COLOR_WHITE);
    
    /* Affichage du score final */
    sprintf(message, "Fin de la partie !");
    MLV_draw_text(MARGE + TAILLE_PLATEAU_GRAPHIQUE/2 - 60, 
                 MARGE + TAILLE_PLATEAU_GRAPHIQUE/3 + 20, 
                 message, MLV_COLOR_BLACK);
    
    sprintf(message, "Score final : Noir %d - Blanc %d", compte_noir, compte_blanc);
    MLV_draw_text(MARGE + TAILLE_PLATEAU_GRAPHIQUE/2 - 100, 
                 MARGE + TAILLE_PLATEAU_GRAPHIQUE/3 + 50, 
                 message, MLV_COLOR_BLACK);
    
    if (compte_noir > compte_blanc) {
        MLV_draw_text(MARGE + TAILLE_PLATEAU_GRAPHIQUE/2 - 80, 
                     MARGE + TAILLE_PLATEAU_GRAPHIQUE/3 + 80, 
                     "Le joueur Noir gagne !", MLV_COLOR_BLACK);
    } else if (compte_blanc > compte_noir) {
        MLV_draw_text(MARGE + TAILLE_PLATEAU_GRAPHIQUE/2 - 80, 
                     MARGE + TAILLE_PLATEAU_GRAPHIQUE/3 + 80, 
                     "Le joueur Blanc gagne !", MLV_COLOR_BLACK);
    } else {
        MLV_draw_text(MARGE + TAILLE_PLATEAU_GRAPHIQUE/2 - 50, 
                     MARGE + TAILLE_PLATEAU_GRAPHIQUE/3 + 80, 
                     "Match nul !", MLV_COLOR_BLACK);
    }
    MLV_actualise_window();
    
    /* Attendre un clic avant de fermer */
    MLV_wait_mouse(&x, &y);
}

/* Fonction pour libérer les ressources graphiques */
void fermer_fenetre_graphique() {
    MLV_free_window();
}

#endif /* _AFFICHAGE_GRAPHIQUE_C_ */