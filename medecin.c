#include <stdio.h>
#include <string.h>
#include "shm.h"

// Fichier medecin.c à rédiger
int main(int argc, char *argv[]) {
    ainit(argv[0]);

    if (argc != 1) {
        fprintf(stderr, "usage: %s : %d args\n", argv[0], argc - 1);
        return 1;
    }

    int ret = 1; // On souhaite utiliser raler()
    vaccinodrome_t *vaccinodrome = get_vaccinodrome(&ret);

    if (ret < 0 || vaccinodrome == NULL)
        return -1;

    // On veut un nouveau numero de medecin (de 0 à m-1)
    int numMedecin = vaccinodrome->currMedecins++;

    // On initialise un nouveau box
    box_t* box = &vaccinodrome->boxes[numMedecin -1]; //vaccinodrome->boxes[numMedecin - 1];
    box->status = 0;

   // memset(&box->demandeVaccin, 0, sizeof(asem_t));

    CHK(asem_init(&box->demandeVaccin, "demandeVaccin", 1, 0));
    CHK(asem_init(&box->termineVaccin, "termineVaccin", 1, 0));

    adebug(99, "init demandeVaccin nom=%s", box->demandeVaccin.nom);

    int asemVal;
    CHK(asem_post(&vaccinodrome->medecinDisponibles));

    while (1) {
        TCHK(asem_wait(&box->demandeVaccin));

        adebug(99, "vaccindorome status = %d", vaccinodrome->statut);

        // On verifie si le vaccinodrome est ferme
        if (vaccinodrome->statut == 1) {
            // On recupere la place disponible dans le vaccinodrome
            CHK(asem_getvalue(&vaccinodrome->waitingRoom, &asemVal));
            adebug(99, "asemVal(%d) == vzccindrome->sieges(%d)", asemVal, vaccinodrome->sieges);
            if (asemVal == vaccinodrome->sieges) {
                adebug(99, "Le medecin %d est parti.", numMedecin);
                // Le medecin peut partir, il n'y a plus aucun patient
                break;
            }

            // Si il y'a encore un client on s'en occupe.
        }

        fprintf(stdout, "Vaccination du client %s par le medecin %d\n", box->patient, numMedecin);

        asem_post(&box->termineVaccin);
        //snprintf(box.patient, 10, "");
        box->status = 0; // Box a nouveau disponible
        asem_post(&vaccinodrome->medecinDisponibles);
    }


    return 0;
}
