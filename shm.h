#ifndef SHM_H
#define SHM_H

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#define TEMP_FILE_NAME "/vaccinodrome"
#define CHK(op)         do { if ((op) == -1) { raler (#op); } } while (0)
#define NCHK(op)        do { if ((op) == NULL) { raler (#op); } } while (0)
#define TCHK(op)        do { if ((errno=(op)) > 0) { raler (#op); } } while (0)
#define PERR(str, debug) do { if(debug == 1) adebug(2, str); else raler(str); } while(0)

#include "asem.h"

typedef struct siege
{
    int siege;
    int status;
} siege_t;

typedef struct box
{
    asem_t demandeVaccin; // Le patient demande un vaccin
    asem_t termineVaccin; // Le medecin a vaccine le patient
    int status;
    int medecin;
    char patient[10];
} box_t;

typedef struct vaccinodrome
{
    int sieges;
    int medecins;
    int temps;

    asem_t waitingRoom; // Places libres
    asem_t medecinDisponibles; // Medecins disponibles
    asem_t asemMutex;
    asem_t siegeMutex;

    int currMedecins;

    int statut; // Statut : 0 => ouvert ; 1 => fermé

    siege_t salleAttente[1];
    box_t boxes[1];
} vaccinodrome_t;

vaccinodrome_t* get_vaccinodrome(int* err);

vaccinodrome_t* create_vaccinodrome(int* err, int medecins, int sieges);

void destroy_vaccinodrome(vaccinodrome_t *vaccinodrome);

void raler(const char *msg);

#endif // SHM_H
