//
// Created by ipers on 28/11/2021.
//

#ifndef SHM_H
#define SHM_H

#include <stdlib.h>
#include <stdio.h>
#include <stdnoreturn.h>
#include <errno.h>

#define TEMP_FILE_NAME "/vaccinodrome"
#define CHK(op)         do { if ((op) == -1) { raler (#op); } } while (0)
#define NCHK(op)        do { if ((op) == NULL) { raler (#op); } } while (0)
#define TCHK(op)        do { if ((errno=(op)) > 0) { raler (#op); } } while (0)

noreturn void
raler(const char *msg)                 // Une fonction simple pour les erreurs
{
    perror(msg);
    exit(1);
}

#include "asem.h"

struct siege
{
    int nb;
    asem_t sem;
    int status;
};

struct box
{
    int nb;
    asem_t sem;
    int status;
};

typedef struct vaccinodrome
{
    int sieges;
    int medecins;
    int temps;

    asem_t waitingRoomFill;
    asem_t waitingRoomEmpty;

    int currMedecins;

    struct box boxes[];
} vaccinodrome_t;

vaccinodrome_t* get_vaccinodrome();

vaccinodrome_t* create_vaccinodrome();

void destroy_vaccinodrome();

#endif // SHM_H
