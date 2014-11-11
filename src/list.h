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
 * @file list.h
 * Header du fichier list.c
 */

#ifndef C_Proj_list_h
#define C_Proj_list_h

typedef struct element element;
struct element
{
    int i;
    int j;
    struct element *nxt;
};
typedef element* llist;

llist supprimerElementEnTete(llist liste);
llist ajouterEnTete(llist liste, int i, int j);
llist ajouterEnFin(llist liste, int i, int j);

void afficherListe(llist liste);
int nombreElements(llist liste);
llist rechercherElement(llist liste, int i, int j);

llist supprimerElement(llist liste, int i, int j);
llist element_i(llist liste, int indice);

#endif
