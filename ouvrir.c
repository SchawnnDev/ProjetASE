#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "shm.h"

// Fichier ouvrir.c à rédiger
int main (int argc, char *argv []) {

    ainit(argv[0]);

    if(argc != 4)
    {
        fprintf(stderr, "usage: %s : %d args\n", argv[0], argc - 1);
        return 1;
    }

    int n, m, t, ret;

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

    if((t = atoi(argv[3])) < 0)
    {
        fprintf(stderr, "usage: %s : t=%d\n", argv[0], t);
        return 1;
    }

    ret = 1; // On souhaite utiliser raler() et non debug()
    vaccinodrome_t* vaccinodrome = create_vaccinodrome(&ret, m);

    if(ret < 0 || vaccinodrome == NULL)
    {
        return -1;
    }

    vaccinodrome->medecins = m;
    vaccinodrome->sieges = n;
    vaccinodrome->temps = t;

    if(asem_init(&vaccinodrome->waitingRoom, "WaitingRoom", 1, n) == -1)
    {
        raler("asem_init\n");
    }

    if(asem_init(&vaccinodrome->medecinDisponibles, "medecinDisponibles", 1, 0) == -1)
    {
        raler("asem_init\n");
    }

    if(asem_init(&vaccinodrome->asemMutex, "asemMutex", 1, 1) == -1)
    {
        raler("asem_init\n");
    }

    // on initalise le tableau des box

    for (int i = 0; i < vaccinodrome->medecins; ++i) {
        box_t* box = &vaccinodrome->boxes[i];
        memset(box, 0, sizeof (box_t));
        box->siegeStatus = 0;
        box->siege = i;
    }

    adebug(1, "Ouverture reussie!");

    return 0;

}
