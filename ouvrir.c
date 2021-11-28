#include <stdio.h>
#include <stdlib.h>
#include "asem.h"
#include "shm.h"

// Fichier ouvrir.c à rédiger
int main (int argc, char *argv []) {

    ainit(argv[0]);

    if(argc != 4)
    {
        fprintf(stderr, "usage: %s : %d args\n", argv[0], argc - 1);
        return 1;
    }

    int n, m, t;

    if((n = atoi(argv[1])) <= 0)
    {
        fprintf(stderr, "usage: %s : n=%d\n", argv[0], n);
        return 1;
    }

    if((m = atoi(argv[2])) <= 0)
    {
        fprintf(stderr, "usage: %s : m=%d\n", argv[0], m);
        return 1;
    }

    if((t = atoi(argv[3])) <= 0)
    {
        fprintf(stderr, "usage: %s : t=%d\n", argv[0], t);
        return 1;
    }

    vaccinodrome_t* vaccinodrome = create_vaccinodrome();

    vaccinodrome->medecins = m;
    vaccinodrome->sieges = n;
    vaccinodrome->temps = t;

    asem_init(&vaccinodrome->waitingRoomFill, "WaitingRoomFill", 1, 0);
    asem_init(&vaccinodrome->waitingRoomEmpty, "WaitingRoomEmpty", 1, n);


    adebug(1, "Ouverture reussie!");

    return 0;

}
