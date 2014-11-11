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
 * @file main.c
 * Fichier principal du jeu
 * Ce fichier lance le jeu et affiche le menu
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "maze.h"
#include "list.h"

#define COLUMNS 199
#define ROWS 199

#ifdef C_Proj_maze_h

/* define cell structure */
struct _Cell {
    /* 0 = mur
     > 0 = chemin
     -5 = bonus
     -10 = malus
     */
    int id;
    char doors;
};

/* define maze structure */
struct _Maze{
    int width; //Largeur
    int height; //Hauteur
    char * name;
	Cell cell[ROWS][COLUMNS];
    llist ma_liste;
};

#endif

#ifdef C_Proj_game_h

struct _Guy{
    int i;
    int j;
};

struct _Game {
    Maze maze;
    Guy guy;
    long seconds;
    int points;
};

#endif

/**
 * Affiche le menu du programme
 */
int afficherMenu() {
    //Essayer de créer un menu améliorer sans int 
    printf("\nMENU\n");
    printf("1. \tCréer un labyrinthe\n");
    printf("2. \tCharger un labyrinthe\n");
    printf("3. \tJouer\n");
    printf("4. \tSolution\n");
    printf("5. \tQuitter\n");
    
    int choix;
    printf("  ·\tYour choice: ");
    scanf("%d", &choix);
    
    return choix;
}

/*
 * Fonction main
 */
int main(int argc, const char * argv[])
{
    int another_game;
    int choice;
    int height;
    int width;
    char name[60]; //TODO bad
    
    Maze maze;
    Game game;

    srand(time(NULL));

    int maze_loaded = 1;
    
    printf("Welcome in Maze\n");
    do {
        choice = afficherMenu();
        
        switch (choice) {
            case 1:
                //TODO enlever define pour width et height
                printf("  ~\tWidth? (max COLUMNS) ");
                scanf("%d", &width);
                
                printf("  ~\tHeight? (max ROWS) ");
                scanf("%d", &height);
                
                printf("  ~\tName? ");
                scanf("%60s", name);
                
                maze = mazeInit(width, height, name);
                maze = generate(maze);
                maze = fillWithObjects(maze, 0.1, 0.1); //il faut que A + B = 100
                mazeSave(maze);
                maze_loaded = 0;
                
                break;
                
            case 2:
                printf("  ~\tName? ");
                scanf("%60s", name);
                maze = mazeLoad(name);
                break;
                
            case 3:
                if (maze_loaded == 0) {
                    game = newGame(maze);
                    game = fillWithMonsters(game, 0.05, 0.05, 0.05);
                    another_game = startGame(game);
                } else {
                    printf("You have to create and/or load a maze beforehand!\n");
                }
                break;
                
            case 4:
                if (maze_loaded == 0) {
                    mazeSolution(&maze);
                } else {
                    printf("You have to create and/or load a maze beforehand!\n");
                }
                break;

            default:
                break;
        };
    } while (choice != 5);
    
    return 0;
}