#ifndef _AFFICHAGE_H_
#define _AFFICHAGE_H_

#include "reversi.h"

/*Fonctions d'affichage*/
void afficher_plateau(Plateau *plateau);
void afficher_coup_ordinateur(Position coup);
void demander_coup(Position *coup);
void afficher_resultat_final(Plateau *plateau);
void afficher_bienvenue();
void afficher_aide(char *nom_programme);

#endif  /* _AFFICHAGE_H_ */