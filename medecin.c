#include <stdio.h>
#include "shm.h"

// Fichier medecin.c à rédiger
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

    // On veut un nouveau numero de medecin (de 0 à m-1)
    int numMedecin = vaccinodrome->currMedecins++;

    // On initialise un nouveau box
    box_t box = vaccinodrome->boxes[numMedecin-1];
    box.status = 0;

    CHK(asem_init(&box.demandeVaccin, "", 1, 0));
    CHK(asem_init(&box.termineVaccin, "", 1, 0));

    int asemVal;
    CHK(asem_post(&vaccinodrome->medecinDisponibles));

    while(1) {
        TCHK(asem_wait(&box.demandeVaccin));

        // On verifie si le vaccinodrome est ferme
        if(vaccinodrome->statut == 1)
        {
            // On recupere la place disponible dans le vaccinodrome
            CHK(asem_getvalue(&vaccinodrome->waitingRoom, &asemVal));

            if(asemVal == vaccinodrome->sieges)
            {
                adebug(1, "Le medecin %d est parti.", numMedecin);
                // Le medecin peut partir, il n'y a plus aucun patient
                break;
            }

            // Si il y'a encore un client on s'en occupe.
        }

        fprintf(stdout, "Vaccination du client %s par le medecin %d\n", box.patient, numMedecin);

        asem_post(&box.termineVaccin);
        box.patient = "";
        box.status = 0; // Box a nouveau disponible
        asem_post(&vaccinodrome->medecinDisponibles);
    }


    return 0;
}
