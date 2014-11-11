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
 * @file list.c
 * Permet la gestion d'une liste chainée de coordonnées
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct element element;
struct element
{
    int i;
    int j;
    struct element *nxt;
};
typedef element* llist;

llist ajouterEnTete(llist liste, int i, int j)
{
    /* On crée un nouvel élément */
    element* nouvelElement = malloc(sizeof(element));
    
    /* On assigne la valeur au nouvel élément */
    nouvelElement->i = i;
    nouvelElement->j = j;
    
    /* On assigne l'adresse de l'élément suivant au nouvel élément */
    nouvelElement->nxt = liste;
    
    /* On retourne la nouvelle liste, i.e. le pointeur sur le premier élément */
    return nouvelElement;
}

llist ajouterEnFin(llist liste, int i, int j)
{
    /* On crée un nouvel élément */
    element* nouvelElement = malloc(sizeof(element));
    
    /* On assigne la valeur au nouvel élément */
    nouvelElement->i = i;
    nouvelElement->j = j;
    
    /* On ajoute en fin, donc aucun élément ne va suivre */
    nouvelElement->nxt = NULL;
    
    if(liste == NULL)
    {
        /* Si la liste est videé il suffit de renvoyer l'élément créé */
        return nouvelElement;
    }
    else
    {
        /* Sinon, on parcourt la liste à l'aide d'un pointeur temporaire et on
         indique que le dernier élément de la liste est relié au nouvel élément */
        element* temp=liste;
        while(temp->nxt != NULL)
        {
            temp = temp->nxt;
        }
        temp->nxt = nouvelElement;
        return liste;
    }
}

void afficherListe(llist liste)
{
    element *tmp = liste;
    /* Tant que l'on n'est pas au bout de la liste */
    while(tmp != NULL)
    {
        /* On affiche */
        printf("%d %d\n", tmp->i, tmp->j);
        /* On avance d'une case */
        tmp = tmp->nxt;
    }
}

int nombreElements(llist liste)
{
    /* Si la liste est vide, il y a 0 élément */
    if(liste == NULL)
        return 0;
    
    /* Sinon, il y a un élément (celui que l'on est en train de traiter)
     plus le nombre d'éléments contenus dans le reste de la liste */
    return nombreElements(liste->nxt)+1;
}

llist supprimerElementEnTete(llist liste)
{
    if(liste != NULL)
    {
        /* Si la liste est non vide, on se prépare à renvoyer l'adresse de
         l'élément en 2ème position */
        element* aRenvoyer = liste->nxt;
        /* On libère le premier élément */
        free(liste);
        /* On retourne le nouveau début de la liste */
        return aRenvoyer;
    }
    else
    {
        return NULL;
    }
}

llist supprimerElement(llist liste, int i, int j)
{
    /* Liste vide, il n'y a plus rien à supprimer */
    if(liste == NULL)
        return NULL;
    
    /* Si l'élément en cours de traitement doit être supprimé */
    if(liste->i == i && liste->j == j)
    {
        /* On le supprime en prenant soin de mémoriser
         l'adresse de l'élément suivant */
        element* tmp = liste->nxt;
        free(liste);
        /* L'élément ayant été supprimé, la liste commencera à l'élément suivant
         pointant sur une liste qui ne contient plus aucun élément ayant la valeur recherchée */
        tmp = supprimerElement(tmp, i, j);
        return tmp;
    }
    else
    {
        /* Si l'élement en cours de traitement ne doit pas être supprimé,
         alors la liste finale commencera par cet élément et suivra une liste ne contenant
         plus d'élément ayant la valeur recherchée */
        liste->nxt = supprimerElement(liste->nxt, i, j);
        return liste;
    }
}

llist rechercherElement(llist liste, int i, int j)
{
    element *tmp=liste;
    /* Tant que l'on n'est pas au bout de la liste */
    while(tmp != NULL)
    {
        if(tmp->i == i && tmp->j == j)
        {
            /* Si l'élément a la valeur recherchée, on renvoie son adresse */
            return tmp;
        }
        tmp = tmp->nxt;
    }
    return NULL;
}

llist element_i(llist liste, int indice)
{
    int i;
    /* On se déplace de i cases, tant que c'est possible */
    for(i=0; i<indice && liste != NULL; i++)
    {
        liste = liste->nxt;
    }
    
    /* Si l'élément est NULL, c'est que la liste contient moins de i éléments */
    if(liste == NULL)
    {
        return NULL;
    }
    else
    {
        /* Sinon on renvoie l'adresse de l'élément i */
        return liste;
    }
}