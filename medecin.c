#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "shm.h"

/**
 * Vérifier s'il est possible de fermer ce box ou pas
 * @param vaccinodrome Un pointeur vers la structure vaccinodrome
 * @return 1 si c'est possible, sinon 0
 */
int check_fermeture(vaccinodrome_t *vaccinodrome)
{
    // On verifie si le vaccinodrome est ferme
    if (vaccinodrome->statut != 1)
        return 0;

    CHK(asem_wait(&vaccinodrome->siegeMutex));
    int sieges = count_sieges_occupes(vaccinodrome);
    CHK(asem_post(&vaccinodrome->siegeMutex));

    // Si il y'a encore un patient on s'en occupe.
    return sieges == 0;
}

// Fichier medecin.c à rédiger
int main(int argc, char *argv[])
{
    ainit(argv[0]);

    if (argc != 1)
    {
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

    if (vaccinodrome->currMedecins + 1 > vaccinodrome->nbMedecins)
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
    box_t *box = &vaccinodrome->medecins[numMedecin];
    box->status = 0;
    box->medecin = numMedecin;

    CHK(asem_init(&box->demandeVaccin, "demVacc", 1, 0));
    CHK(asem_init(&box->termineVaccin, "terVacc", 1, 0));

    adebug(0, "init demandeVaccin nom=%s", box->demandeVaccin.nom);
    CHK(asem_post(&vaccinodrome->asemMutex));

    CHK(asem_post(&vaccinodrome->medecinDisponibles));

    while (1)
    {
        TCHK(asem_wait(&box->demandeVaccin));

        if (box->status == 2 && check_fermeture(vaccinodrome) == 1)
            // Si aucun siege n'est occupé. on arrête
        {
            adebug(99, "Le medecin %d est parti.", numMedecin);
            // Le medecin peut partir, il n'y a plus aucun patient
            break;
        }

        fprintf(stdout, "medecin %d vaccine %s\n",
                numMedecin, box->patient);

        adebug(0, "Usleep : %d ms", vaccinodrome->temps);

        CHK(usleep(vaccinodrome->temps * 1000));

        // Vaccin terminé, on peut lacher le patient
        CHK(asem_post(&box->termineVaccin));

        CHK(asem_wait(&vaccinodrome->asemMutex));
        box->status = 0; // Box a nouveau disponible
        memset(&box->patient, 0, MAX_NOM_PATIENT);
        CHK(asem_post(&vaccinodrome->asemMutex));

        if (check_fermeture(vaccinodrome) == 1)
            // Si aucun siege n'est occupé. on arrête
        {
            adebug(99, "Le medecin %d est parti.", numMedecin);
            // Le medecin peut partir, il n'y a plus aucun patient
            break;
        }

        CHK(asem_post(&vaccinodrome->medecinDisponibles));
    }

    CHK(asem_wait(&vaccinodrome->asemMutex));
    box->status = 2; // Medecin n'est plus
    CHK(asem_post(&vaccinodrome->asemMutex));

    // Le medecin est terminé
    CHK(asem_post(&vaccinodrome->fermer));

    return 0;
}
