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

#include "asem.h"

typedef struct siege
{
    int siege;
    int statut;
} siege_t;

typedef struct box
{
    asem_t demandeVaccin; // Le patient demande un vaccin
    asem_t termineVaccin; // Le medecin a vaccine le patient
    asem_t boxMutex;
    int status;
    int medecin;
    char patient[10];
} box_t;

typedef struct vaccinodrome
{
    int nbSieges;
    int nbMedecins;
    int temps;

    asem_t waitingRoom; // Places libres
    asem_t medecinDisponibles; // Medecins disponibles
    asem_t asemMutex;
    asem_t siegeMutex;
    asem_t waitingMutex;
    asem_t fermer;

    int currMedecins;
    int currPatientWaiting;

    int statut; // Statut : 0 => ouvert ; 1 => fermé

    box_t medecins[];
} vaccinodrome_t;

vaccinodrome_t* get_vaccinodrome(int* err);

vaccinodrome_t* create_vaccinodrome(int* err, int medecins, int sieges);

void destroy_vaccinodrome(vaccinodrome_t *vaccinodrome);

void raler(const char *msg);

siege_t* find_siege(vaccinodrome_t* vaccinodrome, int statut);

siege_t* get_sieges(vaccinodrome_t* vaccinodrome);

siege_t* get_siege_at(vaccinodrome_t *vaccinodrome, int i);

int count_sieges_occupes(vaccinodrome_t* vaccinodrome);

#endif // SHM_H
