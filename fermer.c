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

    adebug(99, "ret=1");

    if (ret < 0 || vaccinodrome == NULL)
        return -1;

    vaccinodrome->statut = 1;

    adebug(99, "vaccinodrome->status = 1");

    // TODO: Attendre que tous les clients soient sortis.

    int medecinsRestants;

    do
    {

        CHK(asem_wait(&vaccinodrome->asemMutex));
        medecinsRestants = vaccinodrome->medecinsRestants;
        CHK(asem_post(&vaccinodrome->asemMutex));

        if(medecinsRestants == 0)
            break;

        CHK(asem_wait(&vaccinodrome->fermer)); // wait initialisée a 1 dans tous les cas.

        adebug(99, "LANCEMENT FERMETURE NOUVEAU PATIENT");

        CHK(asem_post(&vaccinodrome->nouveauPatient));
    } while(1);

    destroy_vaccinodrome(vaccinodrome);

    adebug(99, "Fermeture reussie!");

    return 0;
}
