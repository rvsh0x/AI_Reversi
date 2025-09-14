#include <stdio.h>
#include <stdlib.h>
#include "../include/reversi.h"
#include "../include/affichage.h"
#include "../include/ia.h"
/* include "../include/affichage_graphique.h" */ /* Partie graphique désactivée */

/* Initialise le plateau avec les positions de départ */
void initialiser_plateau(Plateau *plateau) {
    int i, j;

    /* Initialisation du plateau vide */
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            plateau->cases[i][j] = VIDE;
        }
    }

    /* Position initiale des pions */
    plateau->cases[3][3] = BLANC;
    plateau->cases[3][4] = NOIR;
    plateau->cases[4][3] = NOIR;
    plateau->cases[4][4] = BLANC;
}

/* Vérifie si une position est dans les limites du plateau */
int est_dans_plateau(Position pos) {
    return pos.ligne >= 0 && pos.ligne < TAILLE_PLATEAU &&
           pos.colonne >= 0 && pos.colonne < TAILLE_PLATEAU;
}

/* Directions possibles pour la capture (horizontal, vertical, diagonal) */
Position directions[] = {
    {-1, -1}, {-1, 0}, {-1, 1},
    {0, -1},           {0, 1},
    {1, -1},  {1, 0},  {1, 1}
};

/* Vérifie si un coup est valide */
int coup_valide(Plateau *plateau, Position pos, Joueur joueur) {
    int i, compte;
    Joueur adversaire;
    Position cur;

    /* Vérification des limites du plateau */
    if (!est_dans_plateau(pos)) {
        return 0;
    }

    /* La case doit être vide */
    if (plateau->cases[pos.ligne][pos.colonne] != VIDE) {
        return 0;
    }

    adversaire = (joueur == NOIR) ? BLANC : NOIR;

    /* Le coup est valide s'il permet de retourner au moins un pion */
    for (i = 0; i < 8; i++) {
        cur = pos;
        compte = 0;

        /* Déplacement dans la direction courante */
        cur.ligne += directions[i].ligne;
        cur.colonne += directions[i].colonne;

        /* Tant qu'on trouve des pions adverses */
        while (est_dans_plateau(cur) && plateau->cases[cur.ligne][cur.colonne] == adversaire) {
            compte++;
            cur.ligne += directions[i].ligne;
            cur.colonne += directions[i].colonne;
        }

        /* Si on trouve un de nos pions après une série de pions adverses */
        if (est_dans_plateau(cur) && plateau->cases[cur.ligne][cur.colonne] == joueur && compte > 0) {
            return 1;
        }
    }

    return 0;
}

/* Compte le nombre de pions qui seraient retournés pour un coup donné */
int compter_pions_a_retourner(Plateau *plateau, Position pos, Joueur joueur) {
    int i, compte, pions_retournes;
    Position cur;
    Joueur adversaire;

    pions_retournes = 0;
    adversaire = (joueur == NOIR) ? BLANC : NOIR;

    /* Vérification dans toutes les directions */
    for (i = 0; i < 8; i++) {
        compte = 0;
        cur = pos;

        /* Déplacement dans la direction courante */
        cur.ligne += directions[i].ligne;
        cur.colonne += directions[i].colonne;

        /* Tant qu'on trouve des pions adverses */
        while (est_dans_plateau(cur) && plateau->cases[cur.ligne][cur.colonne] == adversaire) {
            compte++;
            cur.ligne += directions[i].ligne;
            cur.colonne += directions[i].colonne;
        }

        /* Si on trouve un de nos pions après une série de pions adverses */
        if (est_dans_plateau(cur) && plateau->cases[cur.ligne][cur.colonne] == joueur && compte > 0) {
            pions_retournes += compte;
        }
    }

    return pions_retournes;
}

/* Retourne les pions capturés dans toutes les directions */
void retourner_pions(Plateau *plateau, Position pos, Joueur joueur) {
    int i, compte;
    Position cur, temp;
    Joueur adversaire;

    adversaire = (joueur == NOIR) ? BLANC : NOIR;

    /* Parcours de toutes les directions */
    for (i = 0; i < 8; i++) {
        cur = pos;
        temp = pos;
        compte = 0;

        /* Déplacement dans la direction courante */
        cur.ligne += directions[i].ligne;
        cur.colonne += directions[i].colonne;

        /* Compte les pions adverses dans cette direction */
        while (est_dans_plateau(cur) && plateau->cases[cur.ligne][cur.colonne] == adversaire) {
            compte++;
            cur.ligne += directions[i].ligne;
            cur.colonne += directions[i].colonne;
        }

        /* Si on trouve un de nos pions après une série de pions adverses */
        if (est_dans_plateau(cur) && plateau->cases[cur.ligne][cur.colonne] == joueur && compte > 0) {
            /* Retourne tous les pions dans cette direction */
            while (compte > 0) {
                temp.ligne += directions[i].ligne;
                temp.colonne += directions[i].colonne;
                plateau->cases[temp.ligne][temp.colonne] = joueur;
                compte--;
            }
        }
    }
}

/* Applique un coup sur le plateau et retourne les pions capturés */
void appliquer_coup(Plateau *plateau, Position pos, Joueur joueur) {
    int i, compte;
    Position cur, temp;
    Joueur adversaire;

    /* Vérifie si le coup est valide */
    if (!coup_valide(plateau, pos, joueur)) {
        printf("Erreur : Le coup (%d, %d) n'est pas valide.\n", pos.ligne + 1, pos.colonne + 1);
        return;
    }

    adversaire = (joueur == NOIR) ? BLANC : NOIR;

    /* Place le pion du joueur */
    plateau->cases[pos.ligne][pos.colonne] = joueur;

    /* Parcours de toutes les directions */
    for (i = 0; i < 8; i++) {
        cur = pos;
        temp = pos;
        compte = 0;

        /* Déplacement dans la direction courante */
        cur.ligne += directions[i].ligne;
        cur.colonne += directions[i].colonne;

        /* Compte les pions adverses dans cette direction */
        while (est_dans_plateau(cur) && plateau->cases[cur.ligne][cur.colonne] == adversaire) {
            compte++;
            cur.ligne += directions[i].ligne;
            cur.colonne += directions[i].colonne;
        }

        /* Si on trouve un de nos pions après une série de pions adverses */
        if (est_dans_plateau(cur) && plateau->cases[cur.ligne][cur.colonne] == joueur && compte > 0) {
            /* Retourne tous les pions dans cette direction */
            while (compte > 0) {
                temp.ligne += directions[i].ligne;
                temp.colonne += directions[i].colonne;
                plateau->cases[temp.ligne][temp.colonne] = joueur;
                compte--;
            }
        }
    }
}

/* Annule un coup sur le plateau et restaure les pions capturés */
void annuler_coup(Plateau *plateau, Position coup, Joueur joueur) {
    int i; 
    Position cur; 
    Position temp; 
    Joueur adversaire; 
    /* Déterminer l'adversaire */
    adversaire = (joueur == NOIR) ? BLANC : NOIR;

    /* Restaurer la case où le pion a été placé */
    plateau->cases[coup.ligne][coup.colonne] = VIDE;
    /* Debug : Afficher la case restaurée */
    printf("DEBUG: Case (%d, %d) restaurée à VIDE\n", coup.ligne + 1, coup.colonne + 1);

    /* Parcours de toutes les directions */
    for (i = 0; i < 8; i++) {
        /* Initialiser les positions courantes */
        cur = coup;
        temp = coup;

        /* Déplacement dans la direction courante */
        cur.ligne += directions[i].ligne;
        cur.colonne += directions[i].colonne;

        /* Debug : Afficher la direction parcourue */
        printf("DEBUG: Parcours direction (%d, %d)\n", directions[i].ligne, directions[i].colonne);

        /* Parcourir les pions du joueur dans cette direction */
        while (est_dans_plateau(cur) && plateau->cases[cur.ligne][cur.colonne] == joueur) {
            cur.ligne += directions[i].ligne;
            cur.colonne += directions[i].colonne;
        }

        /* Si on trouve un pion adverse après une série de pions du joueur */
        if (est_dans_plateau(cur) && plateau->cases[cur.ligne][cur.colonne] == adversaire) {
            /* Debug : Afficher la position où la restauration commence */
            printf("DEBUG: Restauration des pions adverses à partir de (%d, %d)\n", cur.ligne + 1, cur.colonne + 1);

            /* Restaurer les pions adverses dans cette direction */
            while (temp.ligne != cur.ligne || temp.colonne != cur.colonne) {
                temp.ligne += directions[i].ligne;
                temp.colonne += directions[i].colonne;
                plateau->cases[temp.ligne][temp.colonne] = adversaire;

                /* Debug : Afficher chaque pion restauré */
                printf("DEBUG: Pion restauré à (%d, %d) pour l'adversaire\n", temp.ligne + 1, temp.colonne + 1);
            }
        }
    }

    /* Debug : Afficher l'état final du plateau après l'annulation */
    printf("DEBUG: État du plateau après annulation du coup (%d, %d):\n", coup.ligne + 1, coup.colonne + 1);
    afficher_plateau(plateau); /* Fonction pour afficher le plateau */
}

/* Joue un coup sur le plateau */
void jouer_coup(Plateau *plateau, Position pos, Joueur joueur) {
    if (!coup_valide(plateau, pos, joueur)) {
        return;
    }

    plateau->cases[pos.ligne][pos.colonne] = joueur;
    retourner_pions(plateau, pos, joueur);
}

/* Compte le nombre de pions de chaque couleur sur le plateau */
void compter_pions(Plateau *plateau, int *score_noir, int *score_blanc) {
    int i, j;
    *score_noir = 0;
    *score_blanc = 0;
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            /* Incrémenter le compteur approprié selon la couleur du pion */
            if (plateau->cases[i][j] == NOIR) {
                (*score_noir)++;
            } else if (plateau->cases[i][j] == BLANC) {
                (*score_blanc)++;
            }
        }
    }
}
/* Fonction pour générer tous les coups possibles pour un joueur */
Position* generer_coups_possibles(Plateau *plateau, Joueur joueur, int *nb_coups) {
    Position *coups;
    Position pos;
    int i, j, capacite;

    capacite = 10; /* Capacité initiale de la liste */
    coups = (Position *)malloc(capacite * sizeof(Position));
    if (coups == NULL) {
        printf("Erreur : allocation mémoire échouée pour les coups possibles.\n");
        exit(EXIT_FAILURE);
    }

    *nb_coups = 0;

    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            pos.ligne = i;
            pos.colonne = j;
            if (coup_valide(plateau, pos, joueur)) {
                if (*nb_coups >= capacite) {
                    capacite *= 2;
                    coups = (Position *)realloc(coups, capacite * sizeof(Position));
                    if (coups == NULL) {
                        printf("Erreur : réallocation mémoire échouée pour les coups possibles.\n");
                        exit(EXIT_FAILURE);
                    }
                }
                coups[*nb_coups] = pos;
                (*nb_coups)++;
                /*printf("DEBUG: Coup valide généré : (%d, %d)\n", pos.ligne + 1, pos.colonne + 1);*/
            }
        }
    }

    return coups;
}
/* Vérifie si la partie est terminée */
int partie_terminee(Plateau *plateau) {
    int nb_coups_noir, nb_coups_blanc;
    Position *coups_noir, *coups_blanc;
    int terminee;
    
    /* Générer les coups possibles pour chaque joueur */
    coups_noir = generer_coups_possibles(plateau, NOIR, &nb_coups_noir);
    coups_blanc = generer_coups_possibles(plateau, BLANC, &nb_coups_blanc);
    
    /* La partie est terminée si aucun joueur ne peut jouer */
    terminee = (nb_coups_noir == 0 && nb_coups_blanc == 0);
    
    /* Libérer la mémoire */
    free(coups_noir);
    free(coups_blanc);
    
    return terminee;
}

/* Retourne 1 si le joueur courant a plus de pions */
int position_gagnante(Plateau *plateau) {
    int i, j, compte_noir = 0, compte_blanc = 0;

    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            if (plateau->cases[i][j] == NOIR) {
                compte_noir++;
            } else if (plateau->cases[i][j] == BLANC) {
                compte_blanc++;
            }
        }
    }

    return (plateau->joueur_humain == NOIR) ? (compte_noir > compte_blanc) : (compte_blanc > compte_noir);
}

/* Choisit un coup aléatoire pour l'ordinateur */
Position coup_ordinateur(Plateau *plateau) {
    int i, j, nb_coups = 0;
    Position pos;
    Position coups_possibles[TAILLE_PLATEAU * TAILLE_PLATEAU];

    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            pos.ligne = i;
            pos.colonne = j;
            if (coup_valide(plateau, pos, plateau->ordinateur)) {
                coups_possibles[nb_coups] = pos;
                nb_coups++;
            }
        }
    }

    if (nb_coups == 0) {
        pos.ligne = -1;
        pos.colonne = -1;
        return pos;
    }

    return coups_possibles[rand() % nb_coups];
}

void jouer_partie(Plateau *plateau, int mode_graphique) {
    Position coup;
    Position pos;
    Joueur joueur_actuel;
    int tour = 1; /* 1 pour Noir, 0 pour Blanc */
    int passe_consecutif = 0;
    int coup_possible;
    int i, j;

    /* Boucle principale du jeu */
    while (!partie_terminee(plateau) && passe_consecutif < 2) {
        /* Affichage du plateau */
        if (mode_graphique) {
            /* afficher_plateau_graphique(plateau); */ /* Partie graphique désactivée */
        } else {
            afficher_plateau(plateau);
        }

        joueur_actuel = tour ? NOIR : BLANC;
        coup_possible = 0;

        /* Vérifie s'il y a des coups possibles */
        for (i = 0; i < TAILLE_PLATEAU; i++) {
            for (j = 0; j < TAILLE_PLATEAU; j++) {
                pos.ligne = i; /* Initialisation explicite de la position */
                pos.colonne = j;
                if (coup_valide(plateau, pos, joueur_actuel)) {
                    coup_possible = 1;
                }
            }
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
                    if (mode_graphique) {
                        /* printf("Cliquez sur une case pour jouer.\n"); */
                        /* while (!recuperer_clic(&coup)); */ /* Attente d'un clic valide */
                    } else {
                        demander_coup(&coup);
                    }

                    if (!coup_valide(plateau, coup, joueur_actuel)) {
                        printf("Coup invalide. Veuillez réessayer.\n");
                    }
                } while (!coup_valide(plateau, coup, joueur_actuel));

                jouer_coup(plateau, coup, joueur_actuel);
            } else {
                /* Tour de l'ordinateur */
                coup = coup_ordinateur(plateau);
                if (mode_graphique) {
                    /* afficher_coup_ordinateur_graphique(coup); */ /* Partie graphique désactivée */
                } else {
                    afficher_coup_ordinateur(coup);
                }
                jouer_coup(plateau, coup, joueur_actuel);
            }

            tour = !tour; /* Passe au joueur suivant */
        }
    }

    /* Affichage du résultat final */
    if (mode_graphique) {
        /* afficher_resultat_graphique(plateau); */ /* Partie graphique désactivée */
    } else {
        afficher_plateau(plateau);
        afficher_resultat_final(plateau);
    }
}

