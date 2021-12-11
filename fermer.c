#include <stdio.h>
#include "shm.h"

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
        return -1;

    vaccinodrome->statut = 1;

    adebug(99, "vaccinodrome->status = 1");

    CHK(asem_wait(&vaccinodrome->siegeMutex));
    int siegesOccupes = count_sieges_occupes(vaccinodrome);
    CHK(asem_post(&vaccinodrome->siegeMutex));

    // S'il n'y a plus aucun patient on veut débloquer les médecins
    if (siegesOccupes == 0)
    {
        adebug(99, "ASEM_WAIT FERMER");
        // wait initialisée a 1 dans tous les cas il y'aura au moins un passage

        for (int i = 0; i < vaccinodrome->currMedecins; ++i)
        {
            box_t *box = &vaccinodrome->medecins[i];

            if (box->status != 0) // Le medecin est soit pas dispo soit terminé
                continue;

            CHK(asem_wait(&vaccinodrome->asemMutex));
            box->status = 2;
            CHK(asem_post(&vaccinodrome->asemMutex));

            // on debloque le medecin pour qu'il s'arrete
            CHK(asem_post(&box->demandeVaccin));
        }
    } else
    {
        for (int i = 0; i < vaccinodrome->currPatientWaiting; ++i)
            asem_post(&vaccinodrome->waitingRoom);
    }

    // On attend que tous les médecins se ferment
    for (int i = 0; i < vaccinodrome->currMedecins; ++i)
        CHK(asem_wait(&vaccinodrome->fermer));

    destroy_vaccinodrome(vaccinodrome);

    adebug(99, "Fermeture reussie!");

    return 0;
}
