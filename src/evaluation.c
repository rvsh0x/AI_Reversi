#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/evaluation.h"

/* Tableau global de directions */
static const int DIRECTIONS_HUIT[8][2] = {
    {-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, 
    {0, 1}, {1, -1}, {1, 0}, {1, 1}
};

static const int DIRECTIONS_QUATRE[4][2] = {
    {0, 1},  /* Horizontal */
    {1, 0},  /* Vertical */
    {1, 1},  /* Diagonale \ */
    {1, -1}  /* Diagonale / */
};

/* Fonction pour évaluer la qualité d'un plateau pour un joueur -> IA etape 3 */
int evaluation_plateau(Plateau *plateau, Joueur joueur) {
    int score = 0;
    int i, j;
    Joueur adversaire = (joueur == NOIR) ? BLANC : NOIR;

    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            if (plateau->cases[i][j] == joueur) {
                score++;
            } else if (plateau->cases[i][j] == adversaire) {
                score--;
            }
        }
    }

    return score;
}

/* Fonction pour calculer les ensembles consécutifs sans comptage multiple 
    - elle utilse un tableau de marquage (3d) pour éviter de compter plusieurs fois le meme ensemble 
    - parcourt les quatre directions (horizontale, verticale, diagonale \ et diagonale /)
    - elle compte les ensembles de au moins taille donnée 
        par exemple si taille = 2 , un alignement de 5 pions sera compté comme un seul ensemble
        et non pas 4 ensembles de 2 pions */
int calculer_ensembles_consecutifs(Plateau *plateau, Joueur joueur, int taille) {
    int i, j, k, compteur, total;
    int marque[TAILLE_PLATEAU][TAILLE_PLATEAU][4]; /* un tableau a trois dimensions pour marquer les cases */
    int ligne, colonne, d_ligne, d_colonne;
    int continuer_parcours;
    int dans_plateau;
    int est_joueur;
    
    total = 0;
    /* Initialiser le tableau de marquage */
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            for (k = 0; k < 4; k++) {
                marque[i][j][k] = 0;
            }
        }
    }
    
    /* Parcourir chaque case du plateau */
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            /* Vérifier si la case appartient au joueur */
            if (plateau->cases[i][j] == joueur) {
                /* Vérifier dans chaque direction */
                for (k = 0; k < 4; k++) {
                    /* Si cette case a déjà été comptée dans cette direction, passer */
                    if (!marque[i][j][k]) {
                        compteur = 1;
                        ligne = i;
                        colonne = j;
                        d_ligne = DIRECTIONS_QUATRE[k][0];
                        d_colonne = DIRECTIONS_QUATRE[k][1];
                        continuer_parcours = 1;
                        
                        /* Parcourir les cases dans la direction donnée */
                        while (continuer_parcours) {
                            ligne += d_ligne;
                            colonne += d_colonne;
                            
                            /* Vérifier les limites du plateau et la validité de la case */
                            dans_plateau = (ligne >= 0 && ligne < TAILLE_PLATEAU && 
                                          colonne >= 0 && colonne < TAILLE_PLATEAU);
                            
                            if (dans_plateau) {
                                est_joueur = (plateau->cases[ligne][colonne] == joueur);
                                
                                if (est_joueur) {
                                    compteur++;
                                    marque[ligne][colonne][k] = 1;  /* Marquer cette case comme comptée */
                                } else {
                                    /* Sortir si on rencontre un pion adverse */
                                    continuer_parcours = 0;
                                }
                            } else {
                                /* Sortir si on rencontre une limite du plateau */
                                continuer_parcours = 0;
                            }
                        }
                        
                        /* Si un ensemble de la taille demandée ou plus est trouvé */
                        if (compteur >= taille) {
                            total++;
                        }
                    }
                }
            }
        }
    }
    
    return total;
}

/* 
 * Fonction d'évaluation pour l'étape 4
 * Cette fonction améliore l'évaluation simple de l'étape 3 en prenant en compte
 * les ensembles de 2 et 3 pions consécutifs, qui sont des indicateurs de force
 * positionnelle dans le jeu de Reversi.
 */
int evaluation_etape4(Plateau *plateau, Joueur joueur) {
    int score_base;
    int score_ensembles;
    int ens2_plus, ens3_plus;
    int ens2_exact, ens3_exact;
    int ens2_plus_adv, ens3_plus_adv;
    int ens2_exact_adv, ens3_exact_adv;
    Joueur adversaire;
    
    adversaire = (joueur == NOIR) ? BLANC : NOIR;
    
    /* Évaluation de base (différence de pions) */
    score_base = evaluation_plateau(plateau, joueur);

    /* Obtenir les ensembles cumulatifs */
    ens2_plus = calculer_ensembles_consecutifs(plateau, joueur, 2);
    ens3_plus = calculer_ensembles_consecutifs(plateau, joueur, 3);
    ens2_plus_adv = calculer_ensembles_consecutifs(plateau, adversaire, 2);
    ens3_plus_adv = calculer_ensembles_consecutifs(plateau, adversaire, 3);
    
    /* Calculer les ensembles de taille exacte */
    ens2_exact = ens2_plus - ens3_plus;         /* Exactement 2 pions */
    ens3_exact = ens3_plus;                     /* 3 pions ou plus */
    ens2_exact_adv = ens2_plus_adv - ens3_plus_adv;
    ens3_exact_adv = ens3_plus_adv;
    
    /* Calcul du score pour les ensembles avec pondération différenciée
     * - Ensemble de 2 pions: 2 points
     * - Ensemble de 3 pions ou plus: 5 points
     */
    score_ensembles = (ens2_exact * 2 + ens3_exact * 5) -
                      (ens2_exact_adv * 2 + ens3_exact_adv * 5);

    return score_base + score_ensembles;
}

int evaluation_positions_strategiques(Plateau *plateau, Joueur joueur) {
    int score = 0;
    int i, j;
    Joueur adversaire = (joueur == NOIR) ? BLANC : NOIR;
    
    /* Définir les coins */
    Position coins[4] = {
        {0, 0}, {0, TAILLE_PLATEAU - 1},
        {TAILLE_PLATEAU - 1, 0}, {TAILLE_PLATEAU - 1, TAILLE_PLATEAU - 1}
    };

    /* Définir les cases adjacentes aux coins */
    Position adjacents_coins[12] = {
        {0, 1}, {1, 0}, {1, 1}, /* Coin haut-gauche */
        {0, TAILLE_PLATEAU - 2}, {1, TAILLE_PLATEAU - 1}, {1, TAILLE_PLATEAU - 2}, /* Coin haut-droit */
        {TAILLE_PLATEAU - 2, 0}, {TAILLE_PLATEAU - 1, 1}, {TAILLE_PLATEAU - 2, 1}, /* Coin bas-gauche */
        {TAILLE_PLATEAU - 2, TAILLE_PLATEAU - 1}, {TAILLE_PLATEAU - 1, TAILLE_PLATEAU - 2}, {TAILLE_PLATEAU - 2, TAILLE_PLATEAU - 2} /* Coin bas-droit */
    };
    
    /* 1. Évaluer les coins */
    for (i = 0; i < 4; i++) {
        if (plateau->cases[coins[i].ligne][coins[i].colonne] == joueur) {
            score += 15; /* Bonus élevé pour les coins */
        } else if (plateau->cases[coins[i].ligne][coins[i].colonne] == adversaire) {
            score -= 15; /* Malus si l'adversaire occupe un coin */
        }
    }

    /* 2. Évaluer les cases adjacentes aux coins */
    for (i = 0; i < 12; i++) {
        if (plateau->cases[adjacents_coins[i].ligne][adjacents_coins[i].colonne] == joueur) {
            score -= 7; /* Malus pour les cases adjacentes aux coins */
        } else if (plateau->cases[adjacents_coins[i].ligne][adjacents_coins[i].colonne] == adversaire) {
            score += 7; /* Bonus si l'adversaire occupe ces cases */
        }
    }

    /* 3. Évaluer les bords (sans compter à nouveau les coins et leurs adjacents) */
    /* Bord haut et bas */
    for (j = 1; j < TAILLE_PLATEAU - 1; j++) {
        /* Bord haut */
        if (plateau->cases[0][j] == joueur && 
            j != 1 && j != TAILLE_PLATEAU - 2) {  /* Éviter les cases déjà comptées */
            score += 5;
        } else if (plateau->cases[0][j] == adversaire && 
                  j != 1 && j != TAILLE_PLATEAU - 2) {
            score -= 5;
        }
        
        /* Bord bas */
        if (plateau->cases[TAILLE_PLATEAU-1][j] == joueur && 
            j != 1 && j != TAILLE_PLATEAU - 2) {
            score += 5;
        } else if (plateau->cases[TAILLE_PLATEAU-1][j] == adversaire && 
                  j != 1 && j != TAILLE_PLATEAU - 2) {
            score -= 5;
        }
    }
    
    /* Bord gauche et droit (sans les coins) */
    for (i = 1; i < TAILLE_PLATEAU - 1; i++) {
        /* Bord gauche */
        if (plateau->cases[i][0] == joueur && 
            i != 1 && i != TAILLE_PLATEAU - 2) {
            score += 5;
        } else if (plateau->cases[i][0] == adversaire && 
                  i != 1 && i != TAILLE_PLATEAU - 2) {
            score -= 5;
        }
        
        /* Bord droit */
        if (plateau->cases[i][TAILLE_PLATEAU-1] == joueur && 
            i != 1 && i != TAILLE_PLATEAU - 2) {
            score += 5;
        } else if (plateau->cases[i][TAILLE_PLATEAU-1] == adversaire && 
                  i != 1 && i != TAILLE_PLATEAU - 2) {
            score -= 5;
        }
    }
    
    /* 4. Évaluer des séquences sur les bords de manière plus simple */
    /* Séquences horizontales sur bords haut et bas */
    for (j = 0; j < TAILLE_PLATEAU - 2; j++) {
        /* Bord haut */
        if (plateau->cases[0][j] == joueur && 
            plateau->cases[0][j+1] == joueur && 
            plateau->cases[0][j+2] == joueur) {
            score += 10;
        } else if (plateau->cases[0][j] == adversaire && 
                  plateau->cases[0][j+1] == adversaire && 
                  plateau->cases[0][j+2] == adversaire) {
            score -= 10;
        }
        
        /* Bord bas */
        if (plateau->cases[TAILLE_PLATEAU-1][j] == joueur && 
            plateau->cases[TAILLE_PLATEAU-1][j+1] == joueur && 
            plateau->cases[TAILLE_PLATEAU-1][j+2] == joueur) {
            score += 10;
        } else if (plateau->cases[TAILLE_PLATEAU-1][j] == adversaire && 
                  plateau->cases[TAILLE_PLATEAU-1][j+1] == adversaire && 
                  plateau->cases[TAILLE_PLATEAU-1][j+2] == adversaire) {
            score -= 10;
        }
    }
    
    /* Séquences verticales sur bords gauche et droit */
    for (i = 0; i < TAILLE_PLATEAU - 2; i++) {
        /* Bord gauche */
        if (plateau->cases[i][0] == joueur && 
            plateau->cases[i+1][0] == joueur && 
            plateau->cases[i+2][0] == joueur) {
            score += 10;
        } else if (plateau->cases[i][0] == adversaire && 
                  plateau->cases[i+1][0] == adversaire && 
                  plateau->cases[i+2][0] == adversaire) {
            score -= 10;
        }
        
        /* Bord droit */
        if (plateau->cases[i][TAILLE_PLATEAU-1] == joueur && 
            plateau->cases[i+1][TAILLE_PLATEAU-1] == joueur && 
            plateau->cases[i+2][TAILLE_PLATEAU-1] == joueur) {
            score += 10;
        } else if (plateau->cases[i][TAILLE_PLATEAU-1] == adversaire && 
                  plateau->cases[i+1][TAILLE_PLATEAU-1] == adversaire && 
                  plateau->cases[i+2][TAILLE_PLATEAU-1] == adversaire) {
            score -= 10;
        }
    }

    return score;
}

/* 
 * Fonction d'évaluation pour l'étape 5 (Alpha-Beta)
 * Cette fonction combine la pondération des positions stratégiques
 * et des ensembles de pions, adaptée selon la phase de jeu
 */
int evaluation_etape5(Plateau *plateau, Joueur joueur) {
    int score_positions;
    int score_ensembles;
    int score_base;
    int score_final;
    const char* phase;
    Joueur adversaire;
    int ens2_plus, ens3_plus, ens4_plus;
    int ens2, ens3, ens4;
    int ens2_plus_adv, ens3_plus_adv, ens4_plus_adv;
    int ens2_adv, ens3_adv, ens4_adv;
    
    adversaire = (joueur == NOIR) ? BLANC : NOIR;
    phase = determiner_phase_partie(plateau);
    score_base = evaluation_plateau(plateau, joueur);
    score_positions = evaluation_positions_strategiques(plateau, joueur);
    
    /* Obtenir les ensembles cumulatifs */
    ens2_plus = calculer_ensembles_consecutifs(plateau, joueur, 2);
    ens3_plus = calculer_ensembles_consecutifs(plateau, joueur, 3);
    ens4_plus = calculer_ensembles_consecutifs(plateau, joueur, 4);
    
    ens2_plus_adv = calculer_ensembles_consecutifs(plateau, adversaire, 2);
    ens3_plus_adv = calculer_ensembles_consecutifs(plateau, adversaire, 3);
    ens4_plus_adv = calculer_ensembles_consecutifs(plateau, adversaire, 4);
    
    /* Calculer les ensembles de taille exacte */
    ens2 = ens2_plus - ens3_plus;        /* Exactement 2 pions */
    ens3 = ens3_plus - ens4_plus;        /* Exactement 3 pions */
    ens4 = ens4_plus;                    /* 4 pions ou plus */
    
    ens2_adv = ens2_plus_adv - ens3_plus_adv;
    ens3_adv = ens3_plus_adv - ens4_plus_adv;
    ens4_adv = ens4_plus_adv;
    
    /* Calculer le score des ensembles avec pondération précise */
    score_ensembles = (ens2 * 2 + ens3 * 4 + ens4 * 8) -
                      (ens2_adv * 2 + ens3_adv * 4 + ens4_adv * 8);
    
    /* Adapter la pondération selon la phase de jeu */
    if (strcmp(phase, "debut") == 0) {
        /* En début de partie, priorité aux positions stratégiques */
        score_final = score_base * 1 + score_positions * 5 + score_ensembles * 2;
    }
    else if (strcmp(phase, "milieu") == 0) {
        /* En milieu de partie, équilibre entre positions et ensembles */
        score_final = score_base * 1 + score_positions * 3 + score_ensembles * 4;
    }
    else { /* "fin" */
        /* En fin de partie, priorité aux ensembles de pions et au score de base */
        score_final = score_base * 3 + score_positions * 2 + score_ensembles * 5;
    }
    
    return score_final;
}

/* ****************** Améliorations de l'évaluation ******************* */
/* Fonctions pour calculer la mobilité d'un joueur */
/* Fonction pour calculer la mobilité immédiate d'un joueur */
int calculer_mobilite_immediate(Plateau *plateau, Joueur joueur) {
    int nb_coups;
    Position *coups_possibles;

    coups_possibles = generer_coups_possibles(plateau, joueur, &nb_coups);
    free(coups_possibles);

    return nb_coups;
}

/* Fonction pour compter les pions en frontière (adjacents à une case vide) */
int compter_pions_frontiere(Plateau *plateau, Joueur joueur) {
    int i, j, k;
    int nb_frontiere = 0;
    int est_frontiere;
    int di, dj;
    
    /* Parcourir toutes les cases du plateau */
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            if (plateau->cases[i][j] == joueur) {
                /* Vérifier si le pion est à la frontière (adjacent à une case vide) */
                est_frontiere = 0;
                for (k = 0; k < 8 && !est_frontiere; k++) {
                    di = i + DIRECTIONS_HUIT[k][0];
                    dj = j + DIRECTIONS_HUIT[k][1];
                    if (di >= 0 && di < TAILLE_PLATEAU && dj >= 0 && dj < TAILLE_PLATEAU && 
                        plateau->cases[di][dj] == VIDE) {
                        est_frontiere = 1;
                    }
                }
                
                /* Comptabiliser le pion frontière */
                if (est_frontiere) {
                    nb_frontiere++;
                }
            }
        }
    }
    
    return nb_frontiere;
}

/* Fonction pour calculer le meilleur coup potentiel qui minimise la mobilité adverse */
int calculer_mobilite_potentielle(Plateau *plateau, Joueur joueur) {
    int nb_coups, i;
    Position *coups_possibles;
    Plateau plateau_copie;
    Joueur adversaire;
    int mobilite_adversaire;
    int meilleure_mobilite_adverse = 100; /* Valeur haute pour permettre minimisation */
    
    adversaire = (joueur == NOIR) ? BLANC : NOIR;
    coups_possibles = generer_coups_possibles(plateau, joueur, &nb_coups);
    
    /* Si aucun coup possible, retourner 0 */
    if (nb_coups == 0) {
        free(coups_possibles);
        return 0;
    }
    
    /* Pour chaque coup possible du joueur actuel */
    for (i = 0; i < nb_coups; i++) {
        /* Copier le plateau et jouer le coup */
        plateau_copie = *plateau;
        jouer_coup(&plateau_copie, coups_possibles[i], joueur);
        
        /* Calculer la mobilité de l'adversaire après ce coup */
        mobilite_adversaire = calculer_mobilite_immediate(&plateau_copie, adversaire);
        
        /* Mettre à jour la meilleure (plus faible) mobilité adverse */
        if (mobilite_adversaire < meilleure_mobilite_adverse) {
            meilleure_mobilite_adverse = mobilite_adversaire;
        }
    }
    
    free(coups_possibles);
    
    /* Retourner l'inverse de la meilleure mobilité adverse 
       (moins l'adversaire a de coups, mieux c'est) */
    return 10 - meilleure_mobilite_adverse;
}

/* Fonction améliorée pour évaluer la mobilité */
int evaluation_mobilite(Plateau *plateau, Joueur joueur) {
    int mobilite_immediate_joueur, mobilite_immediate_adversaire;
    int frontiere_joueur, frontiere_adversaire;
    int mobilite_potentielle;
    int score_final;
    float ratio_mobilite, ratio_frontiere;
    Joueur adversaire;
    const char* phase;
    
    /* Initialisation des variables */
    adversaire = (joueur == NOIR) ? BLANC : NOIR;
    phase = determiner_phase_partie(plateau);
    
    /* 1. Calculer la mobilité immédiate */
    mobilite_immediate_joueur = calculer_mobilite_immediate(plateau, joueur);
    mobilite_immediate_adversaire = calculer_mobilite_immediate(plateau, adversaire);
    
    /* Calculer le ratio de mobilité immédiate */
    if (mobilite_immediate_joueur + mobilite_immediate_adversaire > 0) {
        ratio_mobilite = (float)(mobilite_immediate_joueur - mobilite_immediate_adversaire) / 
                        (mobilite_immediate_joueur + mobilite_immediate_adversaire);
    } else {
        ratio_mobilite = 0.0;
    }
    
    /* 2. Calculer les pions en frontière */
    frontiere_joueur = compter_pions_frontiere(plateau, joueur);
    frontiere_adversaire = compter_pions_frontiere(plateau, adversaire);
    
    /* Calculer le ratio de frontière (inverse car moins on a de pions en frontière, mieux c'est) */
    if (frontiere_joueur + frontiere_adversaire > 0) {
        ratio_frontiere = (float)(frontiere_adversaire - frontiere_joueur) / 
                          (frontiere_joueur + frontiere_adversaire);
    } else {
        ratio_frontiere = 0.0;
    }
    
    /* 3. Calculer la mobilité potentielle */
    mobilite_potentielle = calculer_mobilite_potentielle(plateau, joueur);
    
    /* 4. Adapter les poids selon la phase de jeu */
    if (strcmp(phase, "debut") == 0) {
        /* En début de partie: priorité à la réduction de frontière et mobilité potentielle */
        score_final = (int)((ratio_mobilite * 30) + (ratio_frontiere * 50) + (mobilite_potentielle * 20));
    } else if (strcmp(phase, "milieu") == 0) {
        /* En milieu de partie: équilibre entre tous les facteurs */
        score_final = (int)((ratio_mobilite * 40) + (ratio_frontiere * 40) + (mobilite_potentielle * 20));
    } else {
        /* En fin de partie: priorité à la mobilité immédiate */
        score_final = (int)((ratio_mobilite * 60) + (ratio_frontiere * 20) + (mobilite_potentielle * 20));
    }
    
    return score_final;
}

/* 
 * Types de stabilité implémentés:
 * 1. Stabilité absolue: pions qui ne peuvent jamais être retournés (coins et propagation)
 * 2. Semi-stabilité: pions qui sont difficiles à retourner (entourés, bords complets)
 * 3. Stabilité potentielle: pions qui pourraient devenir stables à court terme
 */

/* Fonction pour propager la stabilité aux pions adjacents avec support pour semi-stabilité */
void propager_stabilite_etape7(Plateau *plateau, int est_stable[TAILLE_PLATEAU][TAILLE_PLATEAU], 
                       int est_semi_stable[TAILLE_PLATEAU][TAILLE_PLATEAU], Joueur joueur) {
    int i, j, k;
    int nx, ny;               /* Coordonnées des voisins */
    int changement;           
    int voisins_stables;      
    int directions_stables;   
    int compteur_directions_semi_stables;     
    int est_entoure;          
    int di, dj;               /* Déplacements pour vérifier les voisins */
    
    /* Continuer tant que la propagation se fait */
    changement = 1;
    while (changement) {
        changement = 0;
        
        for (i = 0; i < TAILLE_PLATEAU; i++) {
            for (j = 0; j < TAILLE_PLATEAU; j++) {
                /* Si la case appartient au joueur et n'est pas déjà marquée comme stable */
                if (plateau->cases[i][j] == joueur && !est_stable[i][j]) {
                    /* Vérification de stabilité absolue:
                       Un pion est stable s'il est entouré dans les 8 directions par:
                       - des pions stables de même couleur
                       - ou des bords du plateau */
                    voisins_stables = 1;
                    
                    /* Vérifier les 8 directions */
                    for (k = 0; k < 8 && voisins_stables; k++) {
                        nx = i + DIRECTIONS_HUIT[k][0];
                        ny = j + DIRECTIONS_HUIT[k][1];
                        
                        /* Si on sort du plateau, cette direction est stable */
                        if (nx < 0 || nx >= TAILLE_PLATEAU || ny < 0 || ny >= TAILLE_PLATEAU) {
                            /* Direction stable car bord du plateau */
                        }
                        /* Si le voisin est de la même couleur mais n'est pas stable */
                        else if (plateau->cases[nx][ny] == joueur && !est_stable[nx][ny]) {
                            voisins_stables = 0;
                        }
                        /* Si le voisin est vide, la direction n'est pas stable */
                        else if (plateau->cases[nx][ny] == VIDE) {
                            voisins_stables = 0;
                        }
                    }
                    
                    /* Si toutes les directions sont stables, marquer ce pion comme stable */
                    if (voisins_stables) {
                        est_stable[i][j] = 1;
                        est_semi_stable[i][j] = 0; /* Un pion stable n'est pas semi-stable */
                        changement = 1;
                    }
                }
                
                /* Vérification de semi-stabilité pour les pions non stables du joueur */
                if (plateau->cases[i][j] == joueur && !est_stable[i][j] && !est_semi_stable[i][j]) {
                    /* Critères de semi-stabilité:
                       1. Pions sur les bords avec plusieurs pions adjacents de même couleur
                       2. Pions complètement entourés (sans cases vides adjacentes)
                       3. Pions ayant plusieurs directions stables */
                    
                    /* Compteurs pour les différents critères */
                    directions_stables = 0;
                    compteur_directions_semi_stables = 0;
                    est_entoure = 1;
                    
                    /* Vérifier si le pion est sur un bord */
                    if (i == 0 || i == TAILLE_PLATEAU-1 || j == 0 || j == TAILLE_PLATEAU-1) {
                        /* Pion sur un bord - vérifier le nombre de pions adjacents de même couleur */
                        for (k = 0; k < 8; k++) {
                            di = i + DIRECTIONS_HUIT[k][0];
                            dj = j + DIRECTIONS_HUIT[k][1];
                            
                            if (di >= 0 && di < TAILLE_PLATEAU && dj >= 0 && dj < TAILLE_PLATEAU) {
                                if (plateau->cases[di][dj] == joueur) {
                                    compteur_directions_semi_stables++;
                                }
                                /* Vérifier si toutes les cases adjacentes sont occupées */
                                if (plateau->cases[di][dj] == VIDE) {
                                    est_entoure = 0;
                                }
                            } else {
                                /* Direction hors plateau = stable */
                                directions_stables++;
                            }
                        }
                        
                        /* Sur un bord avec au moins 3 voisins de même couleur = semi-stable */
                        if (compteur_directions_semi_stables >= 3) {
                            est_semi_stable[i][j] = 1;
                            changement = 1;
                        }
                    } else {
                        /* Pion non situé sur un bord */
                        for (k = 0; k < 8; k++) {
                            di = i + DIRECTIONS_HUIT[k][0];
                            dj = j + DIRECTIONS_HUIT[k][1];
                            
                            if (di >= 0 && di < TAILLE_PLATEAU && dj >= 0 && dj < TAILLE_PLATEAU) {
                                /* Vérifier si toutes les cases adjacentes sont occupées */
                                if (plateau->cases[di][dj] == VIDE) {
                                    est_entoure = 0;
                                }
                                /* Vérifier les directions stables */
                                if (plateau->cases[di][dj] == joueur && est_stable[di][dj]) {
                                    directions_stables++;
                                }
                            }
                        }
                        
                        /* Un pion complètement entouré (sans cases vides adjacentes) = semi-stable */
                        if (est_entoure) {
                            est_semi_stable[i][j] = 1;
                            changement = 1;
                        }
                        /* Un pion avec plusieurs directions stables = semi-stable */
                        else if (directions_stables >= 4) {
                            est_semi_stable[i][j] = 1;
                            changement = 1;
                        }
                    }
                }
            }
        }
    }
}

/* Fonction pour évaluer la stabilité des pions avec support pour semi-stabilité */
int evaluation_stabilite_etape7(Plateau *plateau, Joueur joueur) {
    int i, j;
    int score = 0;
    int est_stable_joueur[TAILLE_PLATEAU][TAILLE_PLATEAU];
    int est_stable_adversaire[TAILLE_PLATEAU][TAILLE_PLATEAU];
    int est_semi_stable_joueur[TAILLE_PLATEAU][TAILLE_PLATEAU];
    int est_semi_stable_adversaire[TAILLE_PLATEAU][TAILLE_PLATEAU];
    int nb_stables_joueur = 0;         
    int nb_stables_adversaire = 0;
    int nb_semi_stables_joueur = 0;     
    int nb_semi_stables_adversaire = 0;
    int difference_stabilite;           
    int nb_pions_total;                
    Joueur adversaire = (joueur == NOIR) ? BLANC : NOIR;
    const char* phase;                  
    Position coin;                      

    /* Initialiser les tableaux de stabilité et semi-stabilité */
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            est_stable_joueur[i][j] = 0;
            est_stable_adversaire[i][j] = 0;
            est_semi_stable_joueur[i][j] = 0;
            est_semi_stable_adversaire[i][j] = 0;
        }
    }
    
    /* Initialiser les coins comme stables en utilisant la fonction est_coin() */
    /* Coin supérieur gauche */
    coin.ligne = 0; coin.colonne = 0;
    if (est_coin(coin)) {
        if (plateau->cases[0][0] == joueur) est_stable_joueur[0][0] = 1;
        if (plateau->cases[0][0] == adversaire) est_stable_adversaire[0][0] = 1;
    }
    
    /* Coin supérieur droit */
    coin.ligne = 0; coin.colonne = TAILLE_PLATEAU-1;
    if (est_coin(coin)) {
        if (plateau->cases[0][TAILLE_PLATEAU-1] == joueur) est_stable_joueur[0][TAILLE_PLATEAU-1] = 1;
        if (plateau->cases[0][TAILLE_PLATEAU-1] == adversaire) est_stable_adversaire[0][TAILLE_PLATEAU-1] = 1;
    }
    
    /* Coin inférieur gauche */
    coin.ligne = TAILLE_PLATEAU-1; coin.colonne = 0;
    if (est_coin(coin)) {
        if (plateau->cases[TAILLE_PLATEAU-1][0] == joueur) est_stable_joueur[TAILLE_PLATEAU-1][0] = 1;
        if (plateau->cases[TAILLE_PLATEAU-1][0] == adversaire) est_stable_adversaire[TAILLE_PLATEAU-1][0] = 1;
    }
    
    /* Coin inférieur droit */
    coin.ligne = TAILLE_PLATEAU-1; coin.colonne = TAILLE_PLATEAU-1;
    if (est_coin(coin)) {
        if (plateau->cases[TAILLE_PLATEAU-1][TAILLE_PLATEAU-1] == joueur) est_stable_joueur[TAILLE_PLATEAU-1][TAILLE_PLATEAU-1] = 1;
        if (plateau->cases[TAILLE_PLATEAU-1][TAILLE_PLATEAU-1] == adversaire) est_stable_adversaire[TAILLE_PLATEAU-1][TAILLE_PLATEAU-1] = 1;
    }
    
    /* Propager la stabilité pour chaque joueur */
    propager_stabilite_etape7(plateau, est_stable_joueur, est_semi_stable_joueur, joueur);
    propager_stabilite_etape7(plateau, est_stable_adversaire, est_semi_stable_adversaire, adversaire);
    
    /* Calculer le score basé sur les pions stables et semi-stables */
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            if (est_stable_joueur[i][j]) {
                nb_stables_joueur++;
                score += 12; /* Bonus pour un pion stable du joueur */
            } 
            else if (est_semi_stable_joueur[i][j]) {
                nb_semi_stables_joueur++;
                score += 6; /* Bonus réduit pour un pion semi-stable du joueur */
            }
            
            if (est_stable_adversaire[i][j]) {
                nb_stables_adversaire++;
                score -= 12; /* Malus pour un pion stable de l'adversaire */
            }
            else if (est_semi_stable_adversaire[i][j]) {
                nb_semi_stables_adversaire++;
                score -= 6; /* Malus réduit pour un pion semi-stable de l'adversaire */
            }
        }
    }

    /* Compter le nombre total de pions sur le plateau */
    nb_pions_total = 0;
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            if (plateau->cases[i][j] != VIDE) {
                nb_pions_total++;
            }
        }
    }
    /* Détermination de la phase */
    phase = determiner_phase_partie(plateau);
    /* Bonus supplémentaire basé sur la différence relative de stabilité selon la phase */
    difference_stabilite = (nb_stables_joueur + nb_semi_stables_joueur/2) - 
                          (nb_stables_adversaire + nb_semi_stables_adversaire/2);
    
    if (strcmp(phase, "debut") == 0) {
        score += difference_stabilite * 4; /* Moins important en début de partie */
    } else if (strcmp(phase, "milieu") == 0) {
        score += difference_stabilite * 8; /* Plus important en milieu de partie */
    } else { /* "fin" ou "endgame" */
        score += difference_stabilite * 15; /* Crucial en fin de partie */
    }

    /* Bonus additionnel si aucun coin n'est occupé mais qu'on a des pions semi-stables */
    if (nb_stables_joueur == 0 && nb_stables_adversaire == 0 && 
        nb_semi_stables_joueur > nb_semi_stables_adversaire) {
        score += (nb_semi_stables_joueur - nb_semi_stables_adversaire) * 3;
    }

    return score;
}

/* Fonction améliorée pour vérifier si un pion est stable ou semi-stable */
int pion_stable_etape7(Plateau *plateau, int ligne, int colonne) {
    int i;
    int dx, dy;                /* Direction de déplacement */
    int nx, ny;                /* Coordonnées du voisin immédiat */
    int tx, ty;                /* Coordonnées pour parcourir dans une direction */
    int est_sur_bord;          
    int stable_dans_toutes_directions; 
    int semi_stable;           
    int directions_stables;    
    int peut_retourner;       
    int cases_vides_adjacentes;
    Joueur couleur_pion;       
    int fin_recherche;         
    Position position_coin;    

    /* Un pion vide n'est pas stable */
    couleur_pion = plateau->cases[ligne][colonne];
    if (couleur_pion == VIDE) {
        return 0;
    }
    
    /* Un pion est stable s'il est dans un coin */
    position_coin.ligne = ligne;
    position_coin.colonne = colonne;
    if (est_coin(position_coin)) {
        return 2; /* Les coins sont toujours stables (2 = stable) */
    }

    /* Vérifier si le pion est sur un bord */
    est_sur_bord = (ligne == 0 || ligne == TAILLE_PLATEAU - 1 || 
                   colonne == 0 || colonne == TAILLE_PLATEAU - 1);
    
    /* Compter les directions stables et les cases vides adjacentes */
    directions_stables = 0;
    cases_vides_adjacentes = 0;
    
    /* Vérifier si le pion est stable dans toutes les directions */
    stable_dans_toutes_directions = 1;
    
    for (i = 0; i < 8; i++) {
        dx = DIRECTIONS_HUIT[i][0];
        dy = DIRECTIONS_HUIT[i][1];
        nx = ligne + dx;
        ny = colonne + dy;
        
        /* Si la direction mène hors du plateau, elle est stable */
        if (nx < 0 || nx >= TAILLE_PLATEAU || ny < 0 || ny >= TAILLE_PLATEAU) {
            directions_stables++;
        }
        else if (plateau->cases[nx][ny] == VIDE) {
            /* Si la case adjacente est vide, le pion n'est pas stable */
            stable_dans_toutes_directions = 0;
            cases_vides_adjacentes++;
        }
        else if (plateau->cases[nx][ny] != couleur_pion) {
            /* Si la case adjacente est de couleur différente, vérifier si elle peut retourner ce pion */
            peut_retourner = 0;
            tx = nx + dx;      /* Position suivante dans la même direction */
            ty = ny + dy;
            fin_recherche = 0;
            
            while (tx >= 0 && tx < TAILLE_PLATEAU && ty >= 0 && ty < TAILLE_PLATEAU && !fin_recherche) {
                if (plateau->cases[tx][ty] == VIDE) {
                    /* Si on trouve une case vide, pas de retournement possible dans cette direction */
                    fin_recherche = 1;
                } else if (plateau->cases[tx][ty] == couleur_pion) {
                    /* Si on trouve un pion de même couleur, retournement possible */
                    peut_retourner = 1;
                    fin_recherche = 1;
                } else {
                    /* Continuer dans la même direction */
                    tx += dx;
                    ty += dy;
                }
            }
            
            if (peut_retourner) {
                stable_dans_toutes_directions = 0;
            } else {
                directions_stables++;
            }
        } else {
            /* Même couleur, peut être stable */
            directions_stables++;
        }
    }
    
    /* Si le pion est stable dans toutes les directions */
    if (stable_dans_toutes_directions) {
        return 2; /* Stable */
    }
    
    /* Critères de semi-stabilité */
    semi_stable = 0;
    
    /* Pion sur un bord avec au moins 5 directions stables */
    if (est_sur_bord && directions_stables >= 5) {
        semi_stable = 1;
    }
    /* Pion non sur un bord mais entouré (pas de case vide adjacente) */
    else if (!est_sur_bord && cases_vides_adjacentes == 0) {
        semi_stable = 1;
    }
    /* Pion avec au moins 6 directions stables */
    else if (directions_stables >= 6) {
        semi_stable = 1;
    }
    
    return semi_stable; /* 0 = non stable, 1 = semi-stable, 2 = stable */
}

/* Fonction pour déterminer la phase de la partie avec endgame = fin */
const char* determiner_phase_partie(Plateau *plateau) {
    int nb_pions, nb_cases_vides;
    int nb_coups_possibles_noir, nb_coups_possibles_blanc;
    int i, j;
    int coins_occupes;
    Position *coups_possibles;
    Position coins[4] = {
        {0, 0}, {0, TAILLE_PLATEAU - 1},
        {TAILLE_PLATEAU - 1, 0}, {TAILLE_PLATEAU - 1, TAILLE_PLATEAU - 1}
    };

    /* Initialisation des compteurs */
    nb_pions = 0;
    nb_cases_vides = 0;
    coins_occupes = 0;

    /* Compter le nombre de pions, cases vides, et coins occupés */
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            if (plateau->cases[i][j] != VIDE) {
                nb_pions++;
            } else {
                nb_cases_vides++;
            }
        }
    }

    /* Compter les coins occupés */
    for (i = 0; i < 4; i++) {
        if (plateau->cases[coins[i].ligne][coins[i].colonne] != VIDE) {
            coins_occupes++;
        }
    }

    /* Générer les coups possibles pour évaluer le facteur de branchement */
    coups_possibles = generer_coups_possibles(plateau, NOIR, &nb_coups_possibles_noir);
    free(coups_possibles);
    coups_possibles = generer_coups_possibles(plateau, BLANC, &nb_coups_possibles_blanc);
    free(coups_possibles);

    /* Critères unifiés pour la phase fin/endgame :
        - entre 48 et 51 cases remplies 
        - prise en compte de perfect play : 
            dans une litterature specialisée, sur reversi (les traveaux de Michael buro)
            on considere que le end game est la phase ou une recherche exhaustibe devient 
            possible -> correspond à 14-18 cases vides pour une recherche avancée */
    if (nb_cases_vides <= 14 ||  /* Seuil strict pour endgame profond */
        (nb_cases_vides <= 18 && (nb_coups_possibles_noir + nb_coups_possibles_blanc <= 8)) || /* Seuil étendu avec faible mobilité */
        (nb_pions >= 50 && coins_occupes >= 3)) { /* Configuration typique de fin de partie */
        return "endgame";
    }
    
    /* Si ce n'est pas un endgame/fin, déterminer si on est en début ou milieu de partie */
    if (nb_pions < 20) {
        return "debut";
    } else {
        return "milieu";
    }
}

/* Fonction pour calculer la parité */
int evaluation_parite(Plateau *plateau, Joueur joueur) {
    int nb_cases_vides;
    int nb_pions_joueur, nb_pions_adversaire;
    int i, j;
    nb_cases_vides = 0;
    nb_pions_joueur = 0;
    nb_pions_adversaire = 0;
    /* Compter le nombre de cases vides et de pions de chaque joueur */
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            if (plateau->cases[i][j] == VIDE) {
                nb_cases_vides++;
            } else if (plateau->cases[i][j] == joueur) {
                nb_pions_joueur++;
            } else {
                nb_pions_adversaire++;
            }
        }
    }

    /* Amélioration: parité basée à la fois sur le nombre de cases vides et le différentiel de pions */
    if (nb_cases_vides % 2 == 0) {
        /* Avantage si cases vides paires et le joueur a plus de pions que l'adversaire */
        if (nb_pions_joueur > nb_pions_adversaire) {
            return 10;
        } else {
            return -10;
        }
    } else {
        /* Avantage si cases vides impaires et le joueur a moins de pions que l'adversaire */
        if (nb_pions_joueur < nb_pions_adversaire) {
            return 10;
        } else {
            return -10;
        }
    }
}

/* Fonction pour évaluer les Disk-Square Tables */
int evaluation_disk_square(Plateau *plateau, Joueur joueur) {
    int score = 0;
    int i, j;
    const char* phase;
    int disk_square_debut[TAILLE_PLATEAU][TAILLE_PLATEAU] = {
        {20, -3, 11,  8,  8, 11, -3, 20},
        {-3, -7, -4,  1,  1, -4, -7, -3},
        {11, -4,  2,  2,  2,  2, -4, 11},
        { 8,  1,  2, -3, -3,  2,  1,  8},
        { 8,  1,  2, -3, -3,  2,  1,  8},
        {11, -4,  2,  2,  2,  2, -4, 11},
        {-3, -7, -4,  1,  1, -4, -7, -3},
        {20, -3, 11,  8,  8, 11, -3, 20}
    };
    
    int disk_square_fin[TAILLE_PLATEAU][TAILLE_PLATEAU] = {
        {10, 5, 5, 5, 5, 5, 5, 10},
        { 5, 1, 1, 1, 1, 1, 1,  5},
        { 5, 1, 1, 1, 1, 1, 1,  5},
        { 5, 1, 1, 1, 1, 1, 1,  5},
        { 5, 1, 1, 1, 1, 1, 1,  5},
        { 5, 1, 1, 1, 1, 1, 1,  5},
        { 5, 1, 1, 1, 1, 1, 1,  5},
        {10, 5, 5, 5, 5, 5, 5, 10}
    };
    
    /* Amélioration: utiliser des tables différentes selon la phase du jeu */
    phase = determiner_phase_partie(plateau);
    
    for (i = 0; i < TAILLE_PLATEAU; i++) {
        for (j = 0; j < TAILLE_PLATEAU; j++) {
            if (plateau->cases[i][j] == joueur) {
                if (strcmp(phase, "endgame") == 0) {
                    score += disk_square_fin[i][j];
                } else {
                    score += disk_square_debut[i][j];
                }
            } else if (plateau->cases[i][j] != VIDE) { /* le cas ou elle est occupée par l'adversaire */
                if (strcmp(phase, "endgame") == 0) { 
                    score -= disk_square_fin[i][j]; 
                } else {
                    score -= disk_square_debut[i][j];
                }
            }
        }
    }

    return score;
}

int evaluation_ensembles_consecutifs(Plateau *plateau, Joueur joueur) {
    int score = 0;
    int ens2_plus, ens3_plus, ens4_plus;
    int ens2, ens3, ens4;
    
    /* Obtenir les comptes cumulatifs */
    ens2_plus = calculer_ensembles_consecutifs(plateau, joueur, 2);
    ens3_plus = calculer_ensembles_consecutifs(plateau, joueur, 3);
    ens4_plus = calculer_ensembles_consecutifs(plateau, joueur, 4);
    
    /* Calculer les ensembles de taille exacte */
    ens2 = ens2_plus - ens3_plus;        /* Exactement 2 pions */
    ens3 = ens3_plus - ens4_plus;        /* Exactement 3 pions */
    ens4 = ens4_plus;                    /* 4 pions ou plus */
    
    /* Pondération différenciée par taille */
    score = ens2 * 1 + ens3 * 3 + ens4 * 5;
    
    return score;
}

/* Nouvelle fonction d'évaluation pondérée intégrant toutes les améliorations */
int evaluation_ponderee7(Plateau *plateau, Joueur joueur) {
    int score_positions;
    int score_mobilite;
    int score_stabilite;
    int score_disk_square;
    int score_parite;
    int score_ensembles;
    const char* phase;

    /* Calcul des scores pour chaque critère */
    score_positions = evaluation_positions_strategiques(plateau, joueur);
    score_mobilite = evaluation_mobilite(plateau, joueur);
    score_stabilite = evaluation_stabilite_etape7(plateau, joueur);
    score_disk_square = evaluation_disk_square(plateau, joueur);
    score_parite = evaluation_parite(plateau, joueur);
    score_ensembles = evaluation_ensembles_consecutifs(plateau, joueur);

    /* Déterminer la phase de la partie */
    phase = determiner_phase_partie(plateau);

    /* Adapter les pondérations en fonction de la phase */
    if (strcmp(phase, "debut") == 0) {
        return (score_positions * 2) + 
               (score_mobilite * 5) + 
               (score_stabilite * 2) + 
               (score_disk_square * 1) + 
               (score_ensembles * 1) +   /* Ajout des ensembles consécutifs avec faible poids en début de partie */
               (score_parite * 0);       /* Parité peu importante en début de partie */
    } else if (strcmp(phase, "milieu") == 0) {
        return (score_positions * 3) + 
               (score_mobilite * 4) + 
               (score_stabilite * 3) + 
               (score_disk_square * 1) + 
               (score_ensembles * 3) +   /* Ensembles plus importants en milieu de partie */
               (score_parite * 1);
    } else { /* "fin" */
        return (score_positions * 2) + 
               (score_mobilite * 1) + 
               (score_stabilite * 6) + 
               (score_disk_square * 0) +  /* Disk-square inutile en fin de partie -> soit on le laisse a 0 soit on le met a 1 */
               (score_ensembles * 2) +    /* Ensembles toujours importants mais moins qu'en milieu */
               (score_parite * 3);        /* Parité très importante en fin de partie */
    }
}