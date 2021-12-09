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

    if(vaccinodrome->statut == 1)
    {
        fprintf(stderr, "Vaccinodrome ferme.\n");
        return -1;
    }

    if(vaccinodrome->currMedecins + 1 > vaccinodrome->nbMedecins)
    {
        fprintf(stderr, "Capacitee de medecins depassee.\n");
        return -1;
    }

    // On veut un nouveau numero de medecin (de 0 à m-1)
    int numMedecin = vaccinodrome->currMedecins++;

    CHK(asem_wait(&vaccinodrome->asemMutex));
    vaccinodrome->medecinsRestants++;
    CHK(asem_post(&vaccinodrome->asemMutex));

    adebug(0, "nouveau medecin id=%d", numMedecin);

    // On initialise un nouveau box
    box_t* box = &vaccinodrome->medecins[numMedecin];
    box->medecin = numMedecin;

    CHK(asem_init(&box->attentePatient, "attentePatient", 1, 0));
    CHK(asem_init(&box->termineVaccin, "termineVaccin", 1, 0));

    siege_t* siege;

    while (1) {
        TCHK(asem_wait(&vaccinodrome->nouveauPatient));

        adebug(0, "vaccindorome status = %d", vaccinodrome->statut);

        CHK(asem_wait(&vaccinodrome->siegeMutex));
        siege = find_siege(vaccinodrome, 1);
        CHK(asem_post(&vaccinodrome->siegeMutex));

        adebug(99, "Siege found ?");

        // On verifie si le vaccinodrome est ferme
        if (siege == NULL && vaccinodrome->statut == 1) {
            adebug(99, "Le medecin %d est parti.", numMedecin);
            // Le medecin peut partir, il n'y a plus aucun patient
            break;
        }

        if(siege == NULL) {
            adebug(99, "Erreur iconnue");
            continue;
        }

        snprintf(box->patient, 10, "%s", siege->patient);

        CHK(asem_post(&siege->attenteMedecin));

        CHK(asem_wait(&box->attentePatient));

        fprintf(stdout, "medecin %d vaccine %s\n", numMedecin, box->patient);

        adebug(0, "Usleep : %d ms", vaccinodrome->temps);

        CHK(usleep(vaccinodrome->temps * 1000));

        CHK(asem_post(&box->termineVaccin));
    }

    CHK(asem_wait(&vaccinodrome->asemMutex));
    vaccinodrome->medecinsRestants--;
    CHK(asem_post(&vaccinodrome->asemMutex));

    CHK(asem_post(&vaccinodrome->fermer));

    return 0;
}
