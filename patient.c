#include <stdio.h>
#include <string.h>
#include "shm.h"

int main(int argc, char *argv[])
{
    ainit(argv[0]);

    if (argc != 2)
    {
        fprintf(stderr, "usage: %s : %d args\n", argv[0], argc - 1);
        return 1;
    }

    char *nom = argv[1];
    size_t len = strlen(nom);

    if (len == 0)
    {
        fprintf(stderr, "usage: %s : nom vide\n", argv[0]);
        return 1;
    }

    if (len > 10)
    {
        fprintf(stderr, "usage: %s : nom = %zu octets\n", argv[0], len);
        return 1;
    }

    // On charge le vaccinodrome de la memoire partagee
    int ret = 1; // On souhaite utiliser raler()
    vaccinodrome_t* vaccinodrome = get_vaccinodrome(&ret);

    if(ret < 0 || vaccinodrome == NULL)
        return -1;

    // On attend qu'une place se libere dans la salle d'attente
    asem_wait(&vaccinodrome->waitingRoom);

    // Une place s'est liberee, on entre dans la salle d'attente

    // On attend un medecin disponible
    asem_wait(&vaccinodrome->medecinDisponibles);

    // Un medecin est disponible !
    box_t box;
    int found = -1;

    for (int i = 0; i < vaccinodrome->currMedecins; ++i)
    {
        box = vaccinodrome->boxes[i];

        // On verifie si le box est disponible
        if (box.status == 0)
        {
            found = i;
            break;
        }

    }

    if (found == -1)
    {
        adebug(2, "Aucun medecin n'a ete trouve, alors qu'il y'avait une place libre.");
        raler("Impossible.\n");
    }

    // Maintenant qu'on a un medecin, on lui ordonne de nous vacciner
    box.status = 1; // Le box n'est plus disponible
    box.patient = nom;
    asem_post(&box.demandeVaccin);

    // On attend que le medecin nous vaccine
    asem_wait(&box.termineVaccin);

    // Le client est vaccine !

    fprintf(stdout, "Patient %s vaccine par le medecin %d.\n", nom, found);
    adebug(1, "Client vaccine!");

    // Une place est disponible dans la salle d'attente
    asem_post(&vaccinodrome->waitingRoom);

    return 0;
}
