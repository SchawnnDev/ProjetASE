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

    // vaccinodrome fermé
    if(vaccinodrome->statut == 1)
    {
        adebug(99, "Vaccinodrome ferme. Client ne peut pas rentrer.");
        return 1;
    }

    fprintf(stdout, "Le patient %s attend une place dans la salle d'attente.\n", nom);

    // On attend qu'une place se libere dans la salle d'attente
    asem_wait(&vaccinodrome->waitingRoom);

    // vaccinodrome fermé
    if(vaccinodrome->statut == 1)
    {
        adebug(99, "Vaccinodrome ferme. Client ne peut pas rentrer.");
        return 1;
    }

    // Une place s'est liberee, on entre dans la salle d'attente

    siege_t* siege;
    int found = -1;
    // on récupère le siege du patient dans la salle d'attente.

    CHK(asem_wait(&vaccinodrome->siegeMutex));

    for (int i = 0; i < vaccinodrome->sieges; ++i)
    {
        siege = &vaccinodrome->salleAttente[i];

        // On verifie si le siege est disponible

        if (siege->status == 0)
        {
            found = i;
            break;
        }
    }

    if (found == -1)
    {
        CHK(asem_post(&vaccinodrome->siegeMutex));
        adebug(2, "Aucun siege n'a ete trouve, alors qu'il y'avait une place libre.");
        raler("Impossible.");
    }

    siege->status = 1; // Le siege n'est plus disponible
    CHK(asem_post(&vaccinodrome->siegeMutex));

    fprintf(stdout, "patient %s siege %d\n", nom, siege->siege);

    // On attend un medecin disponible
    asem_wait(&vaccinodrome->medecinDisponibles);

    CHK(asem_wait(&vaccinodrome->siegeMutex));
    siege->status = 0; // Le siege est a nouveau disponible
    CHK(asem_post(&vaccinodrome->siegeMutex));

    // Un medecin est disponible !
    box_t* box;
    found = -1;

    CHK(asem_wait(&vaccinodrome->asemMutex));
    const int medecins = vaccinodrome->currMedecins;

    for (int i = 0; i < medecins; ++i)
    {
        box = &vaccinodrome->boxes[i];

        // On verifie si le box est disponible

        if (box->status == 0)
        {
            found = i;
            break;
        }

    }

    if (found == -1)
    {
        CHK(asem_post(&vaccinodrome->asemMutex));
        adebug(2, "Aucun medecin n'a ete trouve, alors qu'il y'avait une place libre.");
        raler("Impossible.");
    }

    // Maintenant qu'on a un medecin, on lui ordonne de nous vacciner
    box->status = 1; // Le box n'est plus disponible
    CHK(asem_post(&vaccinodrome->asemMutex));

    // Une place est disponible dans la salle d'attente
    asem_post(&vaccinodrome->waitingRoom);

   // fprintf(stdout, "medecin %d patient %s\n", box->medecin, nom);

    snprintf(box->patient, 10, "%s", nom);
    asem_post(&box->demandeVaccin);

    // On attend que le medecin nous vaccine
    asem_wait(&box->termineVaccin);

    // Le client est vaccine !

    fprintf(stdout, "patient %s medecin %d\n", nom, box->medecin);
    adebug(1, "Client vaccine!");

    return 0;
}
