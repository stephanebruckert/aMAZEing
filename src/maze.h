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
 * @file maze.h
 * Permet la gestion d'un labyrinthe
 */

#include "list.h"

#ifndef C_Proj_maze_h
#define C_Proj_maze_h

/* Structures nécessaires à la création d'un labyrinthe */
typedef struct _Cell Cell;
typedef struct _Maze Maze;

/* Affichage d'un labyrinthe */
void display(Maze maze);
void displaySolution(Maze maze, llist solution);

/* Opérations sur les portes */
int doorExists(char doors, char door_ref);
void setDoor(char *doors, int door);
int getOppositeDoor(int door);
int getAmountDoors(char doors);

char getNextDoorClockwise(char door);
char getNextDoorCounterclockwise(char door);

void affectDoor(int *i, int *j, int door);
int affectDoorI(int i, int door);
int affectDoorJ(int j, int door);

/* Initialisation du labyrinthe */
Maze mazeInit(int width, int height, char *name);

/* Création du labyrinthe */
Maze generate(Maze maze);
void seekNeighboursRecur(Maze *maze, int i, int j, int door, int id);

/* Ajout d'objets au labyrinthes */
Maze fillWithObjects(Maze maze, float malus_percent, float bonus_percent);

/* I/O sur les labyrinthes */
void mazeSave(Maze maze);
Maze mazeLoad(char * name);

/* Trouve la solution d'un labyrinthe */
void mazeSolution(Maze *maze);

#endif
