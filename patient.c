#include <stdio.h>
#include <string.h>
#include "asem.h"

// Fichier patient.c à rédiger
int main (int argc, char *argv []) {
    ainit(argv[0]);

    if(argc != 2)
    {
        fprintf(stderr, "usage: %s : %d args\n", argv[0], argc - 1);
        return 1;
    }

    char* nom = argv[1];
    size_t len = strlen(nom);

    if(len == 0)
    {
        fprintf(stderr, "usage: %s : nom vide\n", argv[0]);
        return 1;
    }

    if(len > 10)
    {
        fprintf(stderr, "usage: %s : nom = %zu octets\n", argv[0], len);
        return 1;
    }

    return 0;
}
