#ifndef SHM_H
#define SHM_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define TEMP_FILE_NAME "/vaccinodrome984165"
#define CHK(op)         do { if ((op) == -1) { raler (#op); } } while (0)
#define NCHK(op)        do { if ((op) == NULL) { raler (#op); } } while (0)
#define TCHK(op)        do { if ((errno=(op)) > 0) { raler (#op); } } while (0)
#define PERR(str, debug) do { if(debug == 1) adebug(2, str); else raler(str); } while(0)
#define MAX_NOM_PATIENT 11
#include "asem.h"

/**
 *  Structure représentant un siege
 *  siege => id du siege
 *  statut => statut du siege ; 0 = libre ; 1 = pris
 */
typedef struct siege
{
    int siege;
    int statut;
} siege_t;

/**
 * Box dans lequel exerce un medecin
 * status => 0 : libre ; 1 = pris ; 2 = ferme
 * medecin => id du medecin
 */
typedef struct box
{
    asem_t demandeVaccin; // Le patient demande un vaccin
    asem_t termineVaccin; // Le medecin a vaccine le patient
    int status;
    int medecin;
    char patient[MAX_NOM_PATIENT];
} box_t;

/**
 * Structure partagée dans la mémoire
 * Contient informations de base
 */
typedef struct vaccinodrome
{
    int nbSieges;
    int nbMedecins;
    int temps;

    asem_t waitingRoom; // Places libres
    asem_t medecinDisponibles; // Medecins disponibles
    asem_t fermer; // Fermeture

    // Semaphores binaires
    asem_t asemMutex;
    asem_t siegeMutex;
    asem_t waitingMutex;

    int currMedecins;
    int currPatientWaiting;

    int statut; // Statut : 0 => ouvert ; 1 => fermé

    box_t medecins[];
} vaccinodrome_t;

/**
 * Charger le vaccinodrome depuis la mémoire partagée
 * @param err S'il y'a une erreur, err=-1 sinon err=0
 * @return Un pointeur vers la structure vaccinodrome chargée depuis la mémoire
 */
vaccinodrome_t* get_vaccinodrome(int* err);

/**
 * Crée l'espace de mémoire partagée pour la structure vaccinodrome_t
 * en fonction du nombre de medecins et sieges
 * @param err S'il y'a une erreur, err=-1 (err=-2 si vaccinodrome non existant) sinon err=0
 * @param medecins Nombre de médecins prédefini
 * @param sieges Nombre de sièges prédefini
 * @return Un pointeur vers la structure vaccinodrome crée en mémoire partagée
 */
vaccinodrome_t* create_vaccinodrome(int* err, int medecins, int sieges);

/**
 * Supprimer la mémoire partagée ainsi que les sémaphores
 * @param vaccinodrome Pointeur vers la structure vaccinodrome a détruire
 */
void destroy_vaccinodrome(vaccinodrome_t *vaccinodrome);

/**
 * Envoie un perror
 * @param msg Message d'erreur
 */
void raler(const char *msg);

/**
 * Trouver un siege dans le vaccinodrome dont le statut est statut.
 * @param vaccinodrome Un pointeur vers la structure vaccinodrome
 * @param statut Le statut
 * @return Premier siege trouvé. si aucun a été trouvé => NULL
 */
siege_t* find_siege(vaccinodrome_t* vaccinodrome, int statut);

/**
 * Récupèrer le pointeur vers le tableau des sièges dans la mémoire
 * Le tableau est positionné dans la mémoire à la fin de la structure
 * vaccinodrome et de son tableau de medecins.
 * @param vaccinodrome Un pointeur vers la structure vaccinodrome
 * @return Pointeur vers le premier element du tableau des sièges
 */
siege_t* get_sieges(vaccinodrome_t* vaccinodrome);

/**
 * Récupèrer le pointeur vers un siege dans le tableau des sièges
 * dans la mémoire
 * @param vaccinodrome Un pointeur vers la structure vaccinodrome
 * @param i i-ème siege que l'on souhaite recuperer
 * @return Pointeur vers siège à la i-ème position
 */
siege_t* get_siege_at(vaccinodrome_t *vaccinodrome, int i);

/**
 * Compte le nombre de sièges occupés (donc sièges où le statut=1)
 * @param vaccinodrome Un pointeur vers la structure vaccinodrome
 * @return Nombre de sièges occupés
 */
int count_sieges_occupes(vaccinodrome_t* vaccinodrome);

#endif // SHM_H
