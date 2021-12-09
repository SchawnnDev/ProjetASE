#include <stdio.h>
#include <string.h>
#include "shm.h"

box_t* trouver_patient(vaccinodrome_t* vaccinodrome, char* patient)
{
    box_t* box = NULL;

    for (int i = 0; i < vaccinodrome->nbMedecins; ++i)
    {
        box = &vaccinodrome->medecins[i];

        if (strncmp(box->patient, patient, MAX_NOMSEM) == 0)
            break;

        box = NULL;
    }

    return box;
}

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

    // on récupère le siege du patient dans la salle d'attente.
    CHK(asem_wait(&vaccinodrome->siegeMutex));

    // on récupère un siege disponible
    siege = find_siege(vaccinodrome, 0);

    if (siege == NULL)
    {
        CHK(asem_post(&vaccinodrome->siegeMutex));
        adebug(2, "Aucun siege n'a ete trouve, alors qu'il y'avait une place libre.");
        raler("Impossible.");
    }

    siege->statut = 1; // Le siege n'est plus disponible
    snprintf(siege->patient, 10, "%s", nom);
    CHK(asem_post(&vaccinodrome->siegeMutex));

    fprintf(stdout, "patient %s siege %d\n", nom, siege->siege);

    CHK(asem_post(&vaccinodrome->nouveauPatient));

    // On attend qu'un medecin nous debloque
    CHK(asem_wait(&siege->attenteMedecin));

    CHK(asem_wait(&vaccinodrome->siegeMutex));
    siege->statut = 0; // Le siege est à nouveau disponible
    memset(siege->patient, 0, MAX_NOMSEM);
    CHK(asem_post(&vaccinodrome->siegeMutex));

    // Une place est disponible dans la salle d'attente
    asem_post(&vaccinodrome->waitingRoom);

    box_t* box = trouver_patient(vaccinodrome, nom);

    if(box == NULL)
    {
        // Probleme
        raler("box");
    }

    fprintf(stdout, "patient %s medecin %d\n", nom, box->medecin);

    // On dit au medecin qu'il peut démarrer la vaccination
    CHK(asem_post(&box->attentePatient));

    // Le medecin a terminé la vaccination
    CHK(asem_wait(&box->termineVaccin));

    adebug(1, "Client vaccine!");

    return 0;
}
