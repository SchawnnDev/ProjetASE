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

    adebug(99, "ret=1");

    if(ret < 0 || vaccinodrome == NULL)
        return -1;

    vaccinodrome->statut = 1;

    adebug(99, "vaccinodrome->status = 1");

    // TODO: Attendre que tous les clients soient sortis.
    do {

        for (int i = 0; i < vaccinodrome->currMedecins; ++i)
        {
            box_t* box = &(vaccinodrome->boxes[i]);
            adebug(99, "vaccinodrome->boxes[%d], status=%d, name=%s", i, box->status, box->demandeVaccin.nom);
            if(box->status != 0) continue;
            asem_getvalue(&(box->demandeVaccin), &ret);
            adebug(99, "asem_getvalue(&box->demandeVaccin, &ret) => %d", ret);
            if(ret > 1) continue;
            adebug(99, "asem_post box->demandeVaccin");
            asem_post(&(box->demandeVaccin));
        }

        asem_getvalue(&vaccinodrome->waitingRoom, &ret);
        adebug(99, "asem_getvalue(&vaccinodrome->waitingRoom, &ret) => %d", ret);
    } while (ret != vaccinodrome->sieges);

    destroy_vaccinodrome(vaccinodrome);

    adebug(1, "Fermeture reussie!");

    return 0;
}
