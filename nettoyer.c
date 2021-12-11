#include <stdio.h>
#include "shm.h"

// Fichier nettoyer.c à rédiger
int main (int argc, char *argv []) {
    ainit(argv[0]);

    if(argc != 1)
    {
        fprintf(stderr, "usage: %s : %d args\n", argv[0], argc - 1);
        return 1;
    }

    int ret = 0; // On souhaite utiliser adebug()
    vaccinodrome_t* vaccinodrome = get_vaccinodrome(&ret);

    if(ret < 0 || vaccinodrome == NULL)
        return 0;

    if(ret < 0)
    {
        adebug(2, "Il n'y a rien a nettoyer.");
        return 0;
    }

    destroy_vaccinodrome(vaccinodrome);

    adebug(2, "Nettoyage reussi!");

    return 0;
}
