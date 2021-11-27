#include <stdio.h>
#include "asem.h"

// Fichier fermer.c à rédiger
int main (int argc, char *argv []) {
    ainit(argv[0]);

    if(argc != 1)
    {
        fprintf(stderr, "usage: %s : %d args\n", argv[0], argc - 1);
        return 1;
    }

    return 0;
}
