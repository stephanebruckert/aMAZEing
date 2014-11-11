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
 * @file maze.c
 * Permet la gestion d'un labyrinthe
 *
 * @todo faire un map viewer
 * @todo dans la création du grand labyrinthe, choisir le nombre du couloir le plus grand
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "game.h"
#include "maze.h"
#include "mydefines.h"

/* Structure d'une cellule */
struct _Cell {
    /* 0 = mur
     > 0 = chemin
     -5 = bonus
     -10 = malus
     */
    int id;
    char doors;
};

/* Structure d'un labyrinthe */
struct _Maze{
    int width; //Largeur
    int height; //Hauteur
    char * name;
	Cell cell[ROWS][COLUMNS];
    llist ma_liste;
};

/**
 * Affiche le labyrinthe sans rien d'autre
 * @param maze un labyrinthe
 */
void display(Maze maze) {
    for (int i = 0; i < maze.height; i++) {
        for (int j = 0; j < maze.width; j++) {
            switch (maze.cell[i][j].id) {
                case 0:
                    printf("[]");
                    break;
                    
                case -5:
                    printf(" +");
                    break;
                    
                case -10:
                    printf(" -");
                    break;
                    
                default:
                    printf("  ");
                    break;
            }
        }
        printf("\n");
    }
}

/**
 * Affiche le labyrinthe avec sa solution
 * @param maze un labyrinthe
 * @param solution la liste des coordonnées représentant la solution
 */
void displaySolution(Maze maze, llist solution) {
    for (int i = 0; i < maze.height; i++) {
        for (int j = 0; j < maze.width; j++) {
            if (maze.cell[i][j].id == 0) {
                printf("[]");
            } else if (rechercherElement(solution, i, j) != NULL) {
                printf(" .");
            } else {
                printf("  ");
            }
        }
        printf("\n");
    }
}

/**
 * Indique si une porte est ouverte
 * @param l'ensemble des portes de la cellule
 * @param la porte recherchée
 * @return différent de 0 si la porte est ouverte
 */
int doorExists(char doors, char door_ref) {
    return doors & door_ref;
}

/**
 * Permet d'ouvrir une porte
 * @param doors l'ensemble des portes d'une cellule
 * @param door la porte à ouvrir
 */
void setDoor(char *doors, int door) {
    *doors |= door;
}

/**
 * Obtient le nombre de portes ouvertes d'une cellule
 * @param doors les portes d'une cellule
 * @return le nombre de portes ouvertes
 */
int getAmountDoors(char doors) {
    int amount = 0;
    
    amount += doorExists(doors, DOOR_N) > 0 ? 1 : 0;
    amount += doorExists(doors, DOOR_E) > 0 ? 1 : 0;
    amount += doorExists(doors, DOOR_S) > 0 ? 1 : 0;
    amount += doorExists(doors, DOOR_W) > 0 ? 1 : 0;
    
    return amount;
}

/**
 * Obtient la porte en face d'une porte 
 * Exemple : NORD en face de SUD, EST en face de OUEST
 * @param door une porte
 * @return la porte d'en face
 */
int getOppositeDoor(int door) {
    //TODO en faire un switch
    if (door == DOOR_E) return DOOR_W;
    if (door == DOOR_W) return DOOR_E;
    if (door == DOOR_S) return DOOR_N;
    if (door == DOOR_N) return DOOR_S;
    
    return 0;
}

/**
 * Obtient la porte suivante dans le sens des aiguilles d'une montre
 * @param la porte actuelle
 * @return la porte suivante
 */
char getNextDoorClockwise(char door) {
    return (door == DOOR_W) ? DOOR_N : door << 1;
}

/**
 * Obtient la porte suivante dans le sens inverse des aiguilles d'une montre
 * @param la porte actuelle
 * @return la porte précédente
 */
char getNextDoorCounterclockwise(char door) {
    return (door == DOOR_N) ? DOOR_W : door >> 1;
}

/**
 * Fonction récursive qui recherche l'ensemble des cellules appartenant au même couloir qu'une cellule.
 * Affecte à l'ensemble du couloir le même identifiant de cellule.
 * @param maze le labyrinthe sur lequel on travaille
 * @param i l'ordonnée de la cellule concernée
 * @param j l'abscisse de la cellule concernée
 * @param door l'entrée par laquelle on arrive
 * @param id l'identifiant de la cellule concernée
 */
void seekNeighboursRecur(Maze *maze, int i, int j, int door, int id) {
    (*maze).cell[i][j].id = id;
    
    //On ouvre la porte par laquelle on arrive
    setDoor(&(maze->cell[i][j].doors), door);

    int doors = (*maze).cell[i][j].doors;
    
    int tested_door;
    
    //Si l'entrée NORD est ouverte et que l'on ne vient pas de cette entrée
    if (doorExists(doors, DOOR_N) != 0 && door != (tested_door = DOOR_N)) {
        //La porte NORD est ouverte, j'y rentre
        //Je sors de la case par la porte opposée : SUD
        seekNeighboursRecur(maze,
              affectDoorI(i, tested_door),
              affectDoorJ(j, tested_door),
              getOppositeDoor(tested_door),
              id);
    }
    if (doorExists(doors, DOOR_E) != 0 && door != (tested_door = DOOR_E)) {
        seekNeighboursRecur(maze,
              affectDoorI(i, tested_door),
              affectDoorJ(j, tested_door),
              getOppositeDoor(tested_door),
              id);
    }
    if (doorExists(doors, DOOR_S) != 0 && door != (tested_door = DOOR_S)) {
        seekNeighboursRecur(maze,
              affectDoorI(i, tested_door),
              affectDoorJ(j, tested_door),
              getOppositeDoor(tested_door),
              id);
    }
    if (doorExists(doors, DOOR_W) != 0 && door != (tested_door = DOOR_W)) {
        seekNeighboursRecur(maze,
              affectDoorI(i, tested_door),
              affectDoorJ(j, tested_door),
              getOppositeDoor(tested_door),
              id);
    }
}

/**
 * Trouve la solution d'un labyrinthe et l'affiche
 * @param maze le labyrinthe en question
 */
void mazeSolution(Maze *maze) {
    int current_i = 1;
    int current_j = 0;
    int from = DOOR_W;
    llist solution = NULL;
    //int counter = 0;
    
    while (!(current_i == (maze->height)-2 && current_j == (maze->width)-1)) {  
        //Ajout ou suppression dans la liste
        /*
        if (rechercherElement(solution, current_i, current_j) != NULL) {
            solution = supprimerElement(solution, current_i, current_j);
            counter++;
        } else {
            for (int i=0; i < counter; i++) {
                solution = supprimerElementEnTete(solution);
            } */
            solution = ajouterEnTete(solution, current_i, current_j);
            /*
            counter = 0;
        }
         */
        
        //Trouve la prochaine porte à droite
        while ((doorExists(maze->cell[current_i][current_j].doors, from = getNextDoorCounterclockwise(from))) == 0);
        
        //On emprunte la porte trouvée
        affectDoor(&(current_i), &(current_j), from);
        
        //Porte par laquelle on est rentrée une fois dans la nouvelle case
        from = getOppositeDoor(from);
    }
    
    displaySolution(*maze, solution);
}

/**
 * Modifie directement les coordonnées i et j en fonction
 * du mouvement vertical ou horizontal effectué.
 * @param la coordonnée horizontale de la cellule
 * @param la coordonnée verticale de la cellule
 */
void affectDoor(int *i, int *j, int door) {
    *i = affectDoorI(*i, door);
    *j = affectDoorJ(*j, door);
}

/** 
 * Permet d'obtenir ien fonction du mouvement VERTICAL effectué
 * Ne modifie pas la valeur mais la renvoie
 * @param la coordonnée horizontale de la cellule
 * @param la coordonnée verticale de la cellule
 * @return int la valeur calculée de i
 */
int affectDoorI(int i, int door) {
    if (door == DOOR_N) {
        return --i;
    } else if (door == DOOR_S) {
        return ++i;
    }
    return i;
}

/**
 * Permet d'obtenir j en fonction du mouvement HORIZONTAL effectué
 * Ne modifie pas la valeur mais la renvoie
 * @param la coordonnée horizontale de la cellule
 * @param la coordonnée verticale de la cellule
 * @return int la valeur calculée de i
 */
int affectDoorJ(int j, int door) {
    if (door == DOOR_E) {
        return ++j;
    } else if (door == DOOR_W) {
        return --j;
    }
    return j;
}

/**
 * Génère un labyrinthe parfait aléatoirement
 * 
 * @param maze un labyrinthe initialisé
 * @return un labyrinthe parfait
 */
Maze generate(Maze maze) {
    int nb_elements = nombreElements(maze.ma_liste);
    
    while (nb_elements > 0) {
        
        //Choix aléatoire du mur à supprimer
        int indice = rand() % nb_elements;
        llist wall_to_break = element_i(maze.ma_liste, indice);
        
        //Conséquences
        int cell_w = maze.cell[wall_to_break->i][wall_to_break->j-1].id; //Cell de gauche
        int cell_e = maze.cell[wall_to_break->i][wall_to_break->j+1].id; //Cell de droite
        int cell_n = maze.cell[wall_to_break->i-1][wall_to_break->j].id; //Cell du haut
        int cell_s = maze.cell[wall_to_break->i+1][wall_to_break->j].id; //Cell du bas
        
        int i = wall_to_break->i;
        int j = wall_to_break->j;
        
        if (cell_w > 0 || cell_e > 0) {
            //Horizontal
            
            //Si il y a un couloir différent en face
            if (cell_w != cell_e) {
                //On casse le mur et on lui donne le numéro de la case de droite
                maze.cell[i][j].id = cell_e;
                
                //On modifie les portes du mur cassé
                setDoor(&maze.cell[i][j].doors, DOOR_E);
                setDoor(&maze.cell[i][j].doors, DOOR_W);
                
                //On ouvre la porte OUEST de la case EST
                setDoor(&maze.cell[i][j+1].doors, DOOR_W);
                
                //On insère le numéro de la case de droite dans la case de gauche et la fonction s'occupera des cases restantes
                seekNeighboursRecur(&maze, i, j-1, DOOR_E, cell_e); //la porte par laquelle on entre est la porte EST de la case
                //On arrive de la porte E avec le id à 6
            }
            //Sinon, on n'ouvre pas le mur
        } else if (cell_n > 0 || cell_s > 0) {
            //Vertical
            
            //Si il y a un couloir en face
            if (cell_n != cell_s) {
                
                //On casse le mur et on lui donne le numéro de la case de droite
                maze.cell[i][j].id = cell_n;
                //On modifie les portes du mur cassé
                setDoor(&maze.cell[i][j].doors, DOOR_N);
                setDoor(&maze.cell[i][j].doors, DOOR_S);
                
                //On ouvre la porte SUD de la case du NORD
                setDoor(&maze.cell[i-1][j].doors, DOOR_S);
                
                seekNeighboursRecur(&maze, i+1, j, DOOR_N, cell_n); //la porte par laquelle on entre dans la case du NORD est la porte SUD de la case
            }
            //Sinon, on n'ouvre pas le mur
        }
        
        //Dans n'importe quel cas, on ne cherchera plus à détruire ce mur
        //On peut l'enlever de la liste
        maze.ma_liste = supprimerElement(maze.ma_liste, i, j);
        
        nb_elements--;
    }
    
    return maze;
}

/**
 * Initialise un labyrinthe avant qu'on le génère
 * @param width la largeur du labyrinthe
 * @param height la hauteur du labyrinthe
 * @param name le nom du labyrinthe
 * @return un labyrinthe initialisé
 */
Maze mazeInit(int width, int height, char * name) {
    Maze * maze = (Maze *) malloc(sizeof(Maze));
    
    maze->width = width;
    maze->height = height;
    maze->ma_liste = NULL;
    maze->name = name;
    
    int id = 0;
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            
            if (i == 0 || i == height-1 || j == 0 || j == width-1) {
                maze->cell[i][j].id = 0;
            } else if (i % 2 == 0 || j % 2 == 0) {
                maze->cell[i][j].id = 0;
                
                if (i%2 == 0 ^ j%2 == 0) {
                    maze->ma_liste = ajouterEnTete(maze->ma_liste, i, j);
                }
            } else {
                maze->cell[i][j].id = ++id;
            }
            maze->cell[i][j].doors = 0;
        }
    }
    
    //Ajout de la porte d'entrée
    maze->cell[1][0].id = -1;
    setDoor(&maze->cell[1][0].doors, DOOR_E);
    setDoor(&maze->cell[1][1].doors, DOOR_W);
    
    //Ajout de la porte de sortie
    maze->cell[height-2][width-1].id = -1;
    setDoor(&maze->cell[height-2][width-1].doors, DOOR_W);
    setDoor(&maze->cell[height-2][width-2].doors, DOOR_E);
    
    return *maze;
}

/**
 * Complète un labyrinthe avec un certain montant de bonus ou malus
 *
 * @param maze le labyrinthe
 * @param malus_percent le % de malus a ajouter.
 * @param bonus_percent le % de bonus a ajouter
 * @return maze le labyrinthe rempli
 */
Maze fillWithObjects(Maze maze, float malus_percent, float bonus_percent) {
    
    if (malus_percent + bonus_percent > 1) {
        return maze; //TODO pas très nice
    }

    llist empty_cells = NULL;
    
    //Recherche des cases vides (on ne prend pas en compte les portes
    //puisqu'on ne veut pas de bonus ou malus dessus)
    //TODO en faire une fonction pck aussi utilisé dans game
    int i;
    
    for (i = 1; i < maze.height -1; i++) {
        for (int j = 1; j < maze.width -1; j++) {
            if (maze.cell[i][j].id != 0) {
                empty_cells = ajouterEnTete(empty_cells, i, j);
                //TODO rendre le jeu plus intéressant en ne récupérant que les cases entourées de 3 portes
            }
        }
    }
    
    //Détermine le nombre de cases vides 
    int nb_elements = nombreElements(empty_cells);
    
    //Le nombre de malus et bonus qu'on veut
    int nb_malus = malus_percent * nb_elements;
    int nb_bonus = bonus_percent * nb_elements;
    
    //Pour un nombre nb_malus de fois
    for (i=0; i < nb_malus; i++) {
        //On choisit un élément aléatoirement
        llist selected = element_i(empty_cells, rand() % nombreElements(empty_cells));
        
        //On le définit comme un malus
        maze.cell[selected->i][selected->j].id = -10;
        
        //Et on le supprime pour qu'il ne puisse pas être resélectionné
        empty_cells = supprimerElement(empty_cells, selected->i, selected->j);
    }
    
    for (i=0; i < nb_bonus; i++) {
        //On choisit un élément aléatoirement
        llist selected = element_i(empty_cells, rand() % nombreElements(empty_cells));
        
        //On le définit comme un malus
        maze.cell[selected->i][selected->j].id = -5;
        
        //Et on le supprime pour qu'il ne puisse pas être resélectionné
        empty_cells = supprimerElement(empty_cells, selected->i, selected->j);
    }
    
    return maze;
}

/**
 * Sauvegarde un labyrinthe dans un fichier
 * @param le labyrinthe sauvegardé
 */
void mazeSave(Maze maze) {
    FILE* file = NULL;
    
    char name[80]; //TODO need trouver taille max d'un fichier sous unix/win
    
    strcpy (name, "games/"); //Créer ce dossier automatiquement
    strcat (name, maze.name);
    strcat (name, ".init");
    
    file = fopen(name, "w");
    
    if (file != NULL)
    {
        // On peut lire et écrire dans le fichier
        fprintf(file, "%d %d\n", maze.height, maze.width);
        
        for (int i = 0; i < maze.height; i++) {
            for (int j = 0; j < maze.width; j++) {
                int id = maze.cell[i][j].id;
                
                switch (id) {
                    case 0:
                        fprintf(file, "#");
                        break;
                    case -5:
                        fprintf(file, "+");
                        break;
                    case -10:
                        fprintf(file, "-");
                        break;
                    default:
                        fprintf(file, " ");
                        break;
                }
            }
            fprintf(file, "\n");
        }
        
        fclose(file);
    }
    else
    {
        printf("Impossible d'ouvrir le fichier test.txt");
    }
}

/**
 * Charge un labyrinthe à partir de son nom
 * @param name le nom du labyrinthe
 * @return un labyrinthe en mémoire
 */
Maze mazeLoad(char *name) {
    Maze * maze = (Maze *) malloc(sizeof(Maze));
    
    FILE* file = NULL;
    
    int res;
    
    char filename[80]; //TODO need trouver taille max d'un fichier sous unix/win
    
    strcpy (filename, "games/"); //Créer ce dossier automatiquement
    strcat (filename, name);
    strcat (filename, ".init");
    
    file = fopen(filename, "r");
    
    if (file == NULL) {
        printf("Error while opening the file\n");
        exit(-1);
    }
    
    int width;
    int height;
        
    res = fscanf(file, "%d %d\n", &width, &height);
    
    if ( res!=2 ){
        printf("Error while reading the amount of lines and columns\n");
        exit(-1);
    }
    
    maze->width = width;
    maze->height = height;
    maze->ma_liste = NULL;
    maze->name = name;
    
    char character = '\n';
    int j = 0;
    int i = 0;
    
    while (character != EOF && character != '\0') {
        
        switch (character = fgetc(file)) {
            case '#':
                maze->cell[i][j].id = 0;
                break;
                
            case '+':
                maze->cell[i][j].id = -5;
                break;
                
            case '-':
                maze->cell[i][j].id = -10;
                break;
            
            case '\n': case '\377':
                break;
                
            default:
                maze->cell[i][j].id = 1;
                break;
        }
                
        j++;
        
        if (character == '\n') {
            i++;
            j=0;
        } else if (character == '\377') {
            break;
        }
    }
    
    fclose(file);
    
    //TODO en faire une fonction pck apparait deux fois
    //Ajout de la porte d'entrée
    setDoor(&maze->cell[1][0].doors, DOOR_E);
    setDoor(&maze->cell[1][1].doors, DOOR_W);
    
    //Ajout de la porte de sortie
    setDoor(&maze->cell[height-2][width-1].doors, DOOR_W);
    setDoor(&maze->cell[height-2][width-2].doors, DOOR_E);
    
    //On retrouve l'ouverture des portes facilement au lieu de s'être compliqué la vie en les enregistrant dans un fichier
    for (int i = 1; i < height-1; i++) {
        for (int j = 1; j < width-1; j++) {
            if (maze->cell[i-1][j].id != 0) setDoor(&(maze->cell[i][j].doors), DOOR_N);
            if (maze->cell[i+1][j].id != 0) setDoor(&(maze->cell[i][j].doors), DOOR_S);
            if (maze->cell[i][j-1].id != 0) setDoor(&(maze->cell[i][j].doors), DOOR_W);
            if (maze->cell[i][j+1].id != 0) setDoor(&(maze->cell[i][j].doors), DOOR_E);
        }
    }

    return *maze;
}