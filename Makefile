# Compilateur et options
CC = gcc
CFLAGS = -W -Wall -pedantic -std=c89 -O2 `pkg-config --cflags MLV`
LDFLAGS = `pkg-config --libs-only-other --libs-only-L MLV`
LDLIBS = `pkg-config --libs-only-l MLV`

# Répertoires
SRC = src
INC = include
TEST = test
BIN = bin
OBJ = $(BIN)/obj
PRES = presentation

# Création des répertoires si nécessaires
$(shell mkdir -p $(BIN) $(OBJ))

# Nom de l'exécutable principal
REVERSI = $(BIN)/reversi

# Fichiers source principaux
SOURCES = main.c reversi.c affichage.c ia.c evaluation.c endgame.c jeu.c ia_memoire.c nega_memoire.c affichage_graphique.c
OBJETS = $(addprefix $(OBJ)/, $(SOURCES:.c=.o))

# Dépendances communes pour les tests
DEPS_TEST = ia.o evaluation.o reversi.o affichage.o endgame.o ia_memoire.o nega_memoire.o
OBJETS_TEST = $(addprefix $(OBJ)/, $(DEPS_TEST))

# Règle principale
all: $(REVERSI) tests

$(REVERSI): $(OBJETS)
		$(CC) $^ -o $@ $(LDFLAGS) $(LDLIBS)

# Règle générique pour la compilation des fichiers sources du programme principal
$(OBJ)/%.o: $(SRC)/%.c
		$(CC) $(CFLAGS) -c $< -o $@

# Tests
tests: gen_plateaux test_memoire minmax_alphabeta test_evaluation test_algos ia_vs_ia test_coherence

# Règles pour les exécutables de test avec leurs dépendances spécifiques
gen_plateaux: $(OBJ)/test_ia_partie_complete.o $(OBJETS_TEST)
		$(CC) $^ -o $(TEST)/$@ $(LDFLAGS) $(LDLIBS)

test_memoire: $(OBJ)/test_memoire.o $(filter-out $(OBJ)/nega_memoire.o, $(OBJETS_TEST))
		$(CC) $^ -o $(TEST)/$@ $(LDFLAGS) $(LDLIBS)

minmax_alphabeta: $(OBJ)/test_minmax_alphabeta.o $(OBJETS_TEST)
		$(CC) $^ -o $(TEST)/$@ $(LDFLAGS) $(LDLIBS)

test_evaluation: $(OBJ)/test_evaluation.o $(OBJETS_TEST)
		$(CC) $^ -o $(TEST)/$@ $(LDFLAGS) $(LDLIBS)

test_algos: $(OBJ)/test_algos_sur_plateaux.o $(OBJETS_TEST)
		$(CC) $^ -o $(TEST)/$@ $(LDFLAGS) $(LDLIBS)

ia_vs_ia: $(OBJ)/test_ia_contre_ia.o $(OBJETS_TEST) $(OBJ)/affichage_graphique.o
		$(CC) $^ -o $(BIN)/$@ $(LDFLAGS) $(LDLIBS)

test_coherence: $(OBJ)/test_coherance.o $(OBJETS_TEST)
		$(CC) $^ -o $(TEST)/$@ $(LDFLAGS) $(LDLIBS)

# Règle générique pour les fichiers de test
$(OBJ)/test_%.o: $(TEST)/test_%.c
		$(CC) $(CFLAGS) -c $< -o $@

# Compilation de la présentation LaTeX
presentation: $(PRES)/presentation.tex
		cd $(PRES) && pdflatex presentation.tex
		cd $(PRES) && pdflatex presentation.tex

# Nettoyage
clean:
		rm -rf $(BIN)
		rm -f $(TEST)/gen_plateaux $(TEST)/test_memoire $(TEST)/minmax_alphabeta $(TEST)/test_evaluation $(TEST)/test_algos $(TEST)/test_coherence

# Nettoyage des fichiers de présentation temporaires
clean-presentation:
		rm -f $(PRES)/*.aux $(PRES)/*.log $(PRES)/*.nav $(PRES)/*.out $(PRES)/*.snm $(PRES)/*.toc $(PRES)/*.vrb

# Nettoyage complet (code + présentation)
clean-all: clean clean-presentation

# Dépendances spécifiques - pour inclure les fichiers d'en-tête
$(OBJ)/main.o: $(INC)/reversi.h $(INC)/affichage.h $(INC)/ia.h $(INC)/evaluation.h $(INC)/endgame.h $(INC)/affichage_graphique.h
$(OBJ)/nega_memoire.o: $(INC)/ia.h $(INC)/reversi.h $(INC)/affichage.h $(INC)/evaluation.h $(INC)/endgame.h $(INC)/nega_memoire.h
$(OBJ)/ia_memoire.o: $(INC)/ia.h $(INC)/reversi.h $(INC)/affichage.h $(INC)/evaluation.h $(INC)/endgame.h
$(OBJ)/reversi.o: $(INC)/reversi.h
$(OBJ)/affichage.o: $(INC)/affichage.h
$(OBJ)/affichage_graphique.o: $(INC)/affichage_graphique.h $(INC)/reversi.h
$(OBJ)/jeu.o: $(INC)/ia.h $(INC)/reversi.h $(INC)/affichage.h $(INC)/evaluation.h $(INC)/endgame.h
$(OBJ)/ia.o: $(INC)/ia.h $(INC)/reversi.h
$(OBJ)/endgame.o: $(INC)/endgame.h $(INC)/reversi.h $(INC)/ia.h $(INC)/evaluation.h
$(OBJ)/evaluation.o: $(INC)/evaluation.h $(INC)/reversi.h $(INC)/ia.h

.PHONY: all clean tests presentation clean-presentation clean-all