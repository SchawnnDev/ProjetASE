#include <stdio.h>
#include <string.h>
#include <unistd.h>
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
    {
        adebug(0, "test");
        return -1;
    }

    if(vaccinodrome->currMedecins + 1 > vaccinodrome->medecins)
    {
        fprintf(stderr, "Capacitee de medecins depassee.\n");
        return -1;
    }

    // On souhaite que la création soit en section critique,
    // car à l'incrémentation de currMedecin++
    // un potentiel patient pourrait réserver le box
    // alors que le box n'est même pas encore initialisé.
    CHK(asem_wait(&vaccinodrome->asemMutex));

    // On veut un nouveau numero de medecin (de 0 à m-1)
    int numMedecin = vaccinodrome->currMedecins++;

    adebug(0, "nouveau medecin id=%d", numMedecin);

    // On initialise un nouveau box
    box_t* box = &vaccinodrome->boxes[numMedecin];
    box->status = 0;
    box->medecin = numMedecin;

    CHK(asem_init(&box->demandeVaccin, "demVacc", 1, 0));
    CHK(asem_init(&box->termineVaccin, "terVacc", 1, 0));

    adebug(0, "init demandeVaccin nom=%s", box->demandeVaccin.nom);

    int asemVal;
    CHK(asem_post(&vaccinodrome->medecinDisponibles));

    CHK(asem_post(&vaccinodrome->asemMutex));

    while (1) {
        TCHK(asem_wait(&box->demandeVaccin));

        adebug(0, "vaccindorome status = %d", vaccinodrome->statut);

        // On verifie si le vaccinodrome est ferme
        if (vaccinodrome->statut == 1) {
            // On recupere la place disponible dans le vaccinodrome
            CHK(asem_getvalue(&vaccinodrome->waitingRoom, &asemVal));
            adebug(0, "asemVal(%d) == vaccindrome->sieges(%d)", asemVal, vaccinodrome->sieges);
            if (asemVal == vaccinodrome->sieges) {
                adebug(0, "Le medecin %d est parti.", numMedecin);
                // Le medecin peut partir, il n'y a plus aucun patient
                break;
            }

            // Si il y'a encore un client on s'en occupe.
        }

        fprintf(stdout, "medecin %d vaccine %s\n", numMedecin, box->patient);

        adebug(0, "Usleep : %d ms", vaccinodrome->temps);

        CHK(usleep(vaccinodrome->temps * 1000));

        asem_post(&box->termineVaccin);
        //snprintf(box->patient, 10, "");

        CHK(asem_wait(&vaccinodrome->asemMutex));
        box->status = 0; // Box a nouveau disponible
        CHK(asem_post(&vaccinodrome->asemMutex));

        asem_post(&vaccinodrome->medecinDisponibles);
    }


    return 0;
}
