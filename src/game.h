/* -*- c-basic-offset: 3 -*-
 *
 * ENSICAEN
 * 6 Boulevard Marechal Juin
 * F-14050 Caen Cedex
 *
 * Ce fichier appartient à son auteur.
 * Il ne peut être reproduit.
 */

/**
 * @author Stéphane Bruckert <stephane.bruckert@ecole.ensicaen.fr>
 * @version     0.9 - 12-11-2012
 */

/**
 * @file game.h
 * Fichier header du fichier game.c
 */


#ifndef C_Proj_game_h
#define C_Proj_game_h

#include "maze.h"

/* Structures propres à une partie de jeu */
typedef struct _Guy Guy;
typedef struct _Game Game;
typedef struct _Monster Monster;

/* Affichage d'une partie */
void displayGame(Game game);
/* Initialise et lance une partie */
Game newGame(Maze maze);
int startGame(Game game);

/* Étape du jeu */
char getNextAction(Game game);
void nextStep(Game *game, char way);

/* Formule de calcul du score */
float scoreFormula(int points, long time);

/* Incorpore des monstres dans la partie */
Game fillWithMonsters(Game game, float percent_patrols, float percent_ghosts, float percent_guards);
int initGhostDirection(Monster monster, Maze maze);

/* Evolution des monstres */
void monsterEvoluate (Monster *monster, Game *game, void (*ptr_mobility)(Monster*, Game));
void monsterPatroling(Monster *monster, Game game);
void monsterGhosting(Monster *monster, Game game);
void monsterGuarding(Monster *monster, Game game);
    
#endif
