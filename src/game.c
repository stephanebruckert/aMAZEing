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
 * @file game.c
 * Permet la gestion d'une partie de jeu
 *
 * @todo fonction qui compare la case d'un guy et la case d'un monstre pour lorsqu'ils se croisent
 * @todo faire une classe monster
 * @todo enlever 'moves' si pas utilisé
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "game.h"
#include "maze.h"
#include "list.h"
#include "mydefines.h"

#ifdef C_Proj_maze_h

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
    char *name;
	Cell cell[ROWS][COLUMNS];
    llist ma_liste;
};
#endif

/* Structure d'un monstre */
struct _Monster {
    int i;
    int j;
    char direction; //La direction de laquelle le monstre arrive (se référer aux constantes des portes)
    char t; //Le type, F, G, P //Fantome / Guard / Patrol
    int radius; //Le rayon dans le cas où c'est un fantome
};

/* Structure du joueur */
struct _Guy{
    int i;
    int j;
};

/* Structure d'une partie de jeu */
struct _Game {
    Maze maze;
    Guy guy;
    Monster * monster;
    int monsters;
    int points;
    long seconds;
    int moves;
};

/**
 * Affiche les infos nécessaires à la partie dont le labyrinthe
 * @param game le jeu en cours
 */
void displayGame(Game game) {
    
    //Nettoie le terminal (marche seulement sous linux/mac mais ça fait de la déco sous Windows).
    printf("\e[1;1H\e[2J\n");
    
    int monster_present;

    for (int i = 0; i < game.maze.height; i++) {
        for (int j = 0; j < game.maze.width; j++) {
            monster_present = -1;
            
            //On vérifie s'il y a un monstre sur cette case
            for (int k = 0; k < game.monsters; k++) {
                if (game.monster[k].i == i && game.monster[k].j == j) {
                    monster_present = k;
                    break;
                }
            }
            
            //On affiche notre guy par-dessus tout
            if (game.guy.i == i && game.guy.j == j) {
                printf("<>");
                
            //Ensuite les monstres
            } else if (monster_present >= 0) {
                printf(" %c", game.monster[monster_present].t);
                
            //Puis les objets
            } else {
                int id = game.maze.cell[i][j].id;
                
                switch (id) {
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
        }
        
        printf("\n");
    }
    
    float score = scoreFormula(game.points, game.seconds);
    
    printf("Time: %ld seconds. Points: %d. Score = %.2f\n", game.seconds, game.points, score);
    printf("Which way? ");
}


/**
 * Fais évoluer le jeu d'une étape
 * @param game le jeu en cours
 * @param way de quelle direction arrive le joueur
 */
void nextStep(Game *game, char way) {
    
    /* Deux pos :
     1) on met à jour les données des acteurs dans leur structure,
     on affiche les données au fur et à mesure.
     Conséq : on devra check à chaque affichage d'une cellule s'il y'a un acteur dedans.
     
     2) on met à jour les données des acteurs dans leur structure,
     on crée un nouveau tableau avec les nouveaux acteurs.
     on affiche ce nouveau tableau.
     Conséq :
     il faudra supprimer chaque acteur du tableau quand il se déplace
     pour calculer le prochain mouvement d'un acteur, il faudra parcourir tout le tableau pour le trouver
     3) notre laby est une structure
     
     */
    
    game->moves++;
    
    //On effectue le mouvement (On est déjà sur que la direction est envisageable)
    int move_i = 0;
    int move_j = 0;
    
    switch(way) {
        case 'z' :
            move_i--;
            break;
            
        case 'q':
            move_j--;
            break;
            
        case 's':
            move_i++;
            break;
            
        case 'd':
            move_j++;
            break;
            
        default:
            break;
    }
    
    game->guy.i += move_i;
    game->guy.j += move_j;
    
    //Algorithme du jeu
    
    /* Si l'utilisateur est tombé sur un objet de type malus ou bonus */
    int points_cell = game->maze.cell[game->guy.i][game->guy.j].id;
    
    switch (points_cell) {
        case -10:
            game->points += points_cell + 5;
            game->maze.cell[game->guy.i][game->guy.j].id = 1;
            break;
            
        case -5:
            game->points -= points_cell;
            game->maze.cell[game->guy.i][game->guy.j].id = 1;
            break;
            
        default:
            break;
    }
    
    //Ca sera de même pour les ennemis
    for (int i = 0; i < game->monsters; i++) {

        //Pour chaque monstre, on cherche sa mobilité
        switch(game->monster[i].t) {
            case 'F':
                monsterEvoluate(&(game->monster[i]), game, monsterGhosting);
                break;
                
            case 'G':
                monsterEvoluate(&(game->monster[i]), game, monsterGuarding);
                break;
                
            case 'P':
                monsterEvoluate(&(game->monster[i]), game, monsterPatroling);
                break;
                
            default:
                break;
        }
    }
}


/**
 * Récupère l'évènement utilisateur s'il est valide
 * @param game le jeu en cours
 * @return la direction choisie
 */
char getNextAction(Game game) {
    //scanf, on get le premier i et j valide
    char c;
    
    //Cherche directions possibles
    char doors = game.maze.cell[game.guy.i][game.guy.j].doors;
    
    //Affichage du jeu, légende, points,
    displayGame(game);
    
    while (1) {
        while ((c = getchar()) != EOF && c != '\n') {
            if ((c == 'z' & doorExists(doors, DOOR_N) != 0) || (c == 's' & doorExists(doors, DOOR_S) != 0)
                || (c == 'q' & doorExists(doors, DOOR_W) != 0) || (c == 'd' & doorExists(doors, DOOR_E) != 0)) {
                printf("\n");
                return c;
            }
        }
    }
}

/**
 * Détermine la porte à prendre pour un monstre qui vient d'une certaine direction, 
 * de quelconque type et à n'importe quel moment qui aurait le choix entre plusieurs portes
 * @param doors l'ensemble des portes ouvertes
 * @param door la porte d'où l'on vient et par laquelle on ne veut pas repartir
 * @return la porte à prendre
 */
int monsterNextDoor(int doors, int door) {
    int inc = 0;
    int amount_doors = getAmountDoors(doors);
        
    if (amount_doors == 1) {
        //Si un monstre se trouve dans une case avec une seule porte, il doit faire demi-tour
        //Donc on retourne la case d'où il vient
        return door;
    } else {
        //initialisation tableau
        int possible_doors[amount_doors - 1];
        
        for (int i = 0; i < amount_doors - 1; i++) {
            possible_doors[i] = 0;
        }
        
        //On crée un tableau avec les portes ouvertes
        //sauf qu'on inclue pas la porte d'où l'on vient
        if (doorExists(doors, DOOR_N) > 0 && door != DOOR_N) {
            possible_doors[inc++] = DOOR_N;
        }
        if (doorExists(doors, DOOR_E) > 0 && door != DOOR_E) {
            possible_doors[inc++] = DOOR_E;
        }
        if (doorExists(doors, DOOR_S) > 0 && door != DOOR_S) {
            possible_doors[inc++] = DOOR_S;
        }
        if (doorExists(doors, DOOR_W) > 0 && door != DOOR_W) {
            possible_doors[inc++] = DOOR_W;
        }
        
        //On fait un random sur les portes ouvertes (il y'en logiquement
        //maximum 3 vu qu'il y a une porte d'où l'on vient que l'on ne prend pas en compte
        int randomised = rand() % inc;

        return possible_doors[randomised];
    }
}

/**
 * Fonction qui permet de déterminer aléatoirement d'où vient un monstre quelconque si un monstre vient d'être initialisé
 * @param type le type du monstre
 * @param doors les portes disponibles au monstre
 * @return une porte au hasard parmi celles disponibles
 */
int initMonsterDirection(char type, int doors) {
    int i = 0;

    int opened_doors[4];

    if (type != 'F') {
        //On crée un tableau avec les portes ouvertes
        if (doorExists(doors, DOOR_N) > 0) {
            opened_doors[i++] = DOOR_N;
        }
        if (doorExists(doors, DOOR_E) > 0) {
            opened_doors[i++] = DOOR_E;
        }
        if (doorExists(doors, DOOR_S) > 0) {
            opened_doors[i++] = DOOR_S;
        }
        if (doorExists(doors, DOOR_W) > 0) {
            opened_doors[i++] = DOOR_W;
        }
    }

    return opened_doors[rand() % i];
    
    /* 
     * Donc maintenant, on peut dire que notre monstre vient d'une certaine direction même s'il vient d'être initialité.
     * La direction qu'il prendra ensuite ne sera plus la même et il ne fera pas de demi tour.
     */
}

/**
 * Initialise la direction d'un monstre.
 * Similaire à initMonsterDirection sauf que celle-ci est pour les fantomes seulement;
 * @param monster le fentome
 * @param maze le labyrinthe
 * @return la direction trouvée aléatoirement
 */
int initGhostDirection(Monster monster, Maze maze) {    
    int opened_doors[2];
    
    opened_doors[0] = monster.i <= maze.height / 2 ? DOOR_N : DOOR_S;
    opened_doors[1] = monster.j <= maze.width / 2 ? DOOR_W : DOOR_E;
    
    return opened_doors[rand() % 2];
}

/**
 * Evolution (1 étape) d'un monstre peu importe son type
 * @param monster un pointeur sur un monstre
 * @param game le jeu en cours
 * @param ptr_mobility un pointeur sur fonction récupérant l'algorithme de déplacement du monstre
 */
void monsterEvoluate (Monster *monster, Game *game, void (*ptr_mobility)(Monster*, Game)) {

    //Si le monstre est nouveau
    //TODO voir si on le met pas à part dans une fonction, ça ferait un test en moins à chaque appel
    (*ptr_mobility)(monster, *game);
    
    /*
    On doit vérifier que l'ancienne et la nouvelle case de notre monstre n'est pas la nouvelle case de notre guy 
    (ça voudrait respectivement dire qu'ils se soient croisés ou qu'ils se touchent)
    */
    if (game->guy.i == monster->i && game->guy.j == monster->j) {
        printf("fin de la partie\n");
    }
}

/**
 * L'algorithme de déplacement d'un monstre patrouilleur.
 *
 * Les patrouilleurs sont qui font la ronde en choisissant des chemins aléatoires. 
 * Ils sont à la recherche de notre guy
 * Ils retournent seulement sur leur chemin s'ils sont dans une impasse ou en face d'une entrée ou sortie.
 * @todo À chaque prise de décision, (càd quand il y a plusieurs portes), ils vérifient dans chaque couloir s'ils voient le joueur mais ne regardent jamais derrière eux.
 * 
 * @param monster le monstre de type patrouilleur
 * @param game la partie en cours
 */
void monsterPatroling(Monster *monster, Game game) {
    //Porte par laquelle on accède à la prochaine case
    int enter_by_that_door = monsterNextDoor(game.maze.cell[monster->i][monster->j].doors, monster->direction);
  
    //On affecte les changements associés à la sortie du monstre par la porte précédement calculée
    affectDoor(&(monster->i), &(monster->j), enter_by_that_door);

    //Porte par laquelle on est rentrée une fois dans la nouvelle case
    monster->direction = getOppositeDoor(enter_by_that_door);
    
    //TODO manque le fait qu'il vérifie chaque couloir
    //pas vérifié pour entrée / sortie
}

/**
 * Algorithme de déplacement d'un monstre de type fantome.
 *
 * Les fantomes appartiennent à des zones et se baladent aléatoirement à l'intérieur sans jamais aller en arrière.
 * Ils sont aveugles et ne poursuivent pas le joueur.
 *
 * @param monster le monstre de type patrouilleur
 * @param game la partie en cours
 */
void monsterGhosting(Monster *monster, Game game) {
    //Un monstre effectue des carrés

    int enter_by_that_door;

    if (game.moves % monster->radius == 0) {
        //Rotation dans le sens des aiguilles d'une montre ou pas
        if (monster->i < game.maze.height / 2) {
            if (monster->j < game.maze.width / 2) {
                enter_by_that_door = monster->direction == DOOR_E ? getNextDoorClockwise(DOOR_E) : DOOR_E;
            } else {
                enter_by_that_door = monster->direction == DOOR_S ? getNextDoorClockwise(DOOR_S) : DOOR_S;
            }
        } else {
            if (monster->j < game.maze.width / 2) {
                enter_by_that_door = monster->direction == DOOR_N ? getNextDoorClockwise(DOOR_N) : DOOR_N;
            } else {
                enter_by_that_door = monster->direction == DOOR_W ? getNextDoorClockwise(DOOR_W) : DOOR_W;
            }
        }
        
        monster->direction = getOppositeDoor(enter_by_that_door);
    } else {
        //Sinon la direction reste la même
        enter_by_that_door = getOppositeDoor(monster->direction);
    }
    
    //On affecte les changements associés à la sortie du monstre par la porte précédement calculée
    affectDoor(&(monster->i), &(monster->j), enter_by_that_door);
}

/**
 * Algorithme de déplacement d'un monstre de type garde.
 * Les gardes, recherche un trésor et le surveille, ils se trouvent dans une limite
 * [(trésor - (1 porte + 1 case)) <= monstre <= trésor + (1 porte  + 1 case)]
 * Ils ne regardent jamais derrière eux
 *
 * @param monster le monstre de type patrouilleur
 * @param game la partie en cours
 */
void monsterGuarding(Monster *monster, Game game) {
    //Porte par laquelle on accède à la prochaine case
    int enter_by_that_door = monsterNextDoor(game.maze.cell[monster->i][monster->j].doors, monster->direction);
    
    //On affecte les changements associés à la sortie du monstre par la porte précédement calculée
    affectDoor(&(monster->i), &(monster->j), enter_by_that_door);
    
    //Porte par laquelle on est rentrée une fois dans la nouvelle case
    monster->direction = getOppositeDoor(enter_by_that_door);
}

/**
 * Détermine les paramètres de la nouvelle partie
 * @param maze un laybrinthe
 * @return une partie de jeu
 */
Game newGame(Maze maze) {
    Game game;
    Guy guy;

    game.maze = maze;
    
    game.guy = guy;
    game.guy.i = 1;
    game.guy.j = 0;
    
    game.seconds = 0;
    game.points = 0;
    game.moves = 0;
    
    return game;
}

/**
 * Ajoute des monstres à la partie
 * @param game la partie en cours
 * @param le pourcentage de patrouilleurs
 * @param le pourcentage de fantomes
 * @param le pourcentage gardes
 * @return la partie en cours
 */
Game fillWithMonsters(Game game, float percent_patrols, float percent_ghosts, float percent_guards) {
    
    if (percent_patrols + percent_ghosts + percent_guards > 1) {
        return game; //TODO pas très nice
    }
    
    //On crée deux listes parce qu'on a besoin en priorité des cases qui font carrefour pour la création des gardes
    llist crossroads_cells = NULL; //les carrefours (cellules à 3 ou 4 portes)
    llist corridor_cells = NULL; //les couloirs (cellules à 1 ou 2 portes)
    
    //Recherche des cases vides (on ne prend pas en compte les portes
    //puisqu'on ne veut pas de bonus ou malus dessus)
    int i;
    
    for (i = 1; i < game.maze.height -1; i++) {
        for (int j = 1; j < game.maze.width -1; j++) {
            if (game.maze.cell[i][j].id > 0) {
                //TODO faire la même chose avec les objets (les placer au fond des couloirs par ex)
                //(en fait, réfléchir si ça serait vraiment bien)
                if (getAmountDoors(game.maze.cell[i][j].doors) >= 3) {
                    crossroads_cells = ajouterEnTete(crossroads_cells, i, j);
                } else {
                    corridor_cells = ajouterEnTete(corridor_cells, i, j);
                }
            }
        }
    }

    //Détermine le nombre d'éléments vides
    int nb_elements = nombreElements(crossroads_cells) + nombreElements(corridor_cells);
    
    //Le nombre de malus et bonus qu'on veut
    int nb_patrols = percent_patrols * nb_elements;
    int nb_ghosts = percent_ghosts * nb_elements;
    int nb_guards = percent_guards * nb_elements;

    game.monsters = nb_patrols + nb_ghosts + nb_guards;
    game.monster = malloc(sizeof(Monster) * (game.monsters));
    
    int monster_inc = 0;
    
    //TODO penser au cas où un garde OU patrouilleur se trouve juste devant la sortie ,
    //      arrive vers le joueur et le conduit inévitablement à rencontrer le monstre
    //TODO faudrait attribuer aux gardes des cases avec 3 portes
    //Pour un nombre nb_guards de fois
    //TODO en faire une fonction si temps
    for (i=0; i < nb_guards; i++) {
        //On choisit un élément aléatoirement
        
        llist selected;
        
        //TODO créer une fonction pour ça 
        //On choisira la liste des cells à deux portes 
        if (nombreElements(crossroads_cells) > 0) {
            selected = element_i(crossroads_cells, rand() % nombreElements(crossroads_cells));
            crossroads_cells = supprimerElement(crossroads_cells, selected->i, selected->j);
        } else {
            selected = element_i(corridor_cells, rand() % nombreElements(corridor_cells));
            corridor_cells = supprimerElement(corridor_cells, selected->i, selected->j);
        }
        
        //On le définit comme un patrouilleur
        game.monster[monster_inc].t = 'G';
        game.monster[monster_inc].i = selected->i;
        game.monster[monster_inc].j = selected->j;
        game.monster[monster_inc].direction = initMonsterDirection(game.monster[monster_inc].t,
                                                                   game.maze.cell[selected->i][selected->j].doors);
        monster_inc++;
    }

    //Pour un nombre nb_patrols de fois
    for (i=0; i < nb_patrols; i++) {
        //On choisit un élément aléatoirement
        llist selected;
        
        if (nombreElements(crossroads_cells) > 0) {
            selected = element_i(crossroads_cells, rand() % nombreElements(crossroads_cells));
            crossroads_cells = supprimerElement(crossroads_cells, selected->i, selected->j);
        } else {
            selected = element_i(corridor_cells, rand() % nombreElements(corridor_cells));
            corridor_cells = supprimerElement(corridor_cells, selected->i, selected->j);
        }
        
        //On le définit comme un patrouilleur
        game.monster[monster_inc].t = 'P';
        game.monster[monster_inc].i = selected->i;
        game.monster[monster_inc].j = selected->j;
        game.monster[monster_inc].direction = initMonsterDirection(game.monster[monster_inc].t,
                                                                   game.maze.cell[selected->i][selected->j].doors);
        monster_inc++;
    }
    
    //Pour un nombre nb_ghosts de fois
    for (i=0; i < nb_ghosts; i++) {
        //On choisit un élément aléatoirement
        llist selected;
        
        if (nombreElements(crossroads_cells) > 0) {
            selected = element_i(crossroads_cells, rand() % nombreElements(crossroads_cells));
            crossroads_cells = supprimerElement(crossroads_cells, selected->i, selected->j);
        } else {
            selected = element_i(corridor_cells, rand() % nombreElements(corridor_cells));
            corridor_cells = supprimerElement(corridor_cells, selected->i, selected->j);
        }
        
        //On le définit comme un patrouilleur
        game.monster[monster_inc].t = 'F';
        game.monster[monster_inc].i = selected->i;
        game.monster[monster_inc].j = selected->j;
        
        int max_area = (game.maze.height < game.maze.width) ? game.maze.height : game.maze.width;
        game.monster[monster_inc].radius = (rand() % (max_area/2 - 2)) + 2;
        game.monster[monster_inc].direction = initGhostDirection(game.monster[monster_inc], game.maze);
        monster_inc++;
    }
    
    return game;
}

/**
 * Formule du calcul du score
 * @param le nombre de points du joueur
 * @param le temps écoulé en secondes depuis le début de la partie
 * @return le score
 */
float scoreFormula(int points, long time) {
    if (time < 1) {
        return 0;
    } else {
        //TODO trouver une meilleure formule
        return points * 3 - time;
    }
}

/*
void save_score(Game game) {
    FILE* file = NULL;
    
    char name[80]; //TODO trouver taille max d'un fichier sous unix/win
        
    strcpy (name, "games/"); //Créer ce dossier automatiquement
    strcat (name, game.maze.name);
    strcat (name, ".cfg");
    
    file = fopen(name, "r");
    int width;
    char * player = NULL;
    
    int i = 0;
    
    if (file != NULL)
    {
        // On peut lire et écrire dans le fichier
        fscanf(file, "%d %s\n", &width, player);
        i++;
    }
    
    if (i == 0) {
        fclose (file);
        file = fopen(name, "w");
    }
}
*/

/**
 * Lance le jeu et s'occupe de son avancement
 */
int startGame(Game game) {
    //int win = 0;
    char way;
    
    long time_begin = time(NULL);
    
    //Tant que l'utilisateur n'a pas atteint la sortie
    while(game.guy.i != game.maze.height-2|| game.guy.j != game.maze.height-1) {
        way = getNextAction(game);
        nextStep(&game, way);
        game.seconds = time(NULL) - time_begin;
    }
    
    //TODO essayer de virer ça
    displayGame(game);
    printf("You won!\n");
    
    return 0;
}