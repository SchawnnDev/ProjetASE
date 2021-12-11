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

    asem_destroy(&vaccinodrome->waitingRoom);
    asem_destroy(&vaccinodrome->medecinDisponibles);
    asem_destroy(&vaccinodrome->asemMutex);
    asem_destroy(&vaccinodrome->siegeMutex);
    asem_destroy(&vaccinodrome->waitingMutex);
    asem_destroy(&vaccinodrome->fermer);

    for (int i = 0; i < vaccinodrome->currMedecins; ++i)
    {
        box_t* box = &vaccinodrome->medecins[i];
        asem_destroy(&box->termineVaccin);
        asem_destroy(&box->demandeVaccin);
        asem_destroy(&box->boxMutex);
    }

    destroy_vaccinodrome(vaccinodrome);

    adebug(2, "Nettoyage reussi!");

    return 0;
}
