#include <stdio.h>
#include "shm.h"

int main (int argc, char *argv []) {
    ainit(argv[0]);

    if(argc != 1)
    {
        fprintf(stderr, "usage: %s : %d args\n", argv[0], argc - 1);
        return 1;
    }

    int ret = 1; // On souhaite utiliser raler()
    vaccinodrome_t* vaccinodrome = get_vaccinodrome(&ret);

    if(ret < 0 || vaccinodrome == NULL)
        return -1;

    vaccinodrome->statut = 1;

    // TODO: Attendre que tous les clients soient sortis.
    do {

        for (int i = 0; i < vaccinodrome->currMedecins; ++i)
        {
            box_t box = vaccinodrome->boxes[i];
            asem_post(&box.demandeVaccin);
        }

        asem_getvalue(&vaccinodrome->waitingRoom, &ret);
    } while (ret != vaccinodrome->sieges);

    destroy_vaccinodrome(vaccinodrome);

    adebug(1, "Fermeture reussie!");

    return 0;
}
