#include <stdio.h>
#include <string.h>
#include "shm.h"

/**
 * Cherche un siege et le rend occupé
 * @param vaccinodrome Un pointeur vers la structure vaccinodrome
 * @return Siège libre trouvé et passé a occupé
 */
siege_t* chercher_siege(vaccinodrome_t* vaccinodrome) {
    siege_t* siege = NULL;
    // on récupère le siege du patient dans la salle d'attente.

    CHK(asem_wait(&vaccinodrome->siegeMutex));

    siege = find_siege(vaccinodrome, 0);

    if (siege == NULL)
    {
        CHK(asem_post(&vaccinodrome->siegeMutex));
        adebug(2, "Aucun siege n'a ete trouve, alors qu'il y'avait une place libre.");
        raler("Impossible.");
    }

    siege->statut = 1; // Le siege n'est plus disponible
    CHK(asem_post(&vaccinodrome->siegeMutex));

    return siege;
}

/**
 * Cherche un box libre (status == 0)
 * @param vaccinodrome Un pointeur vers la structure vaccinodrome
 * @return Un box libre
 */
box_t* chercher_box(vaccinodrome_t* vaccinodrome)
{
    box_t* box = NULL;

    CHK(asem_wait(&vaccinodrome->asemMutex));
    const int medecins = vaccinodrome->currMedecins;

    for (int i = 0; i < medecins; ++i)
    {
        box = &vaccinodrome->medecins[i];

        // On verifie si le box est disponible

        if (box->status == 0)
            break;
        box = NULL;
    }

    if (box == NULL)
    {
        CHK(asem_post(&vaccinodrome->asemMutex));
        adebug(2, "Aucun medecin n'a ete trouve, alors qu'il y'avait une place libre.");
        raler("Impossible.");
    }

    // Maintenant qu'on a un medecin, on lui ordonne de nous vacciner
    box->status = 1; // Le box n'est plus disponible
    CHK(asem_post(&vaccinodrome->asemMutex));

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
        adebug(99, "Vaccinodrome ferme. Patient ne peut pas rentrer.1");
        return 1;
    }

    fprintf(stdout, "Le patient %s attend une place dans la salle d'attente.\n", nom);

    CHK(asem_wait(&vaccinodrome->waitingMutex));
    vaccinodrome->currPatientWaiting++;
    CHK(asem_post(&vaccinodrome->waitingMutex));

    // On attend qu'une place se libere dans la salle d'attente
    asem_wait(&vaccinodrome->waitingRoom);

    CHK(asem_wait(&vaccinodrome->waitingMutex));
    vaccinodrome->currPatientWaiting--;
    CHK(asem_post(&vaccinodrome->waitingMutex));

    // vaccinodrome fermé
    if(vaccinodrome->statut == 1)
    {
        adebug(99, "Vaccinodrome ferme. Patient ne peut pas rentrer.2");
        return 1;
    }

    // Une place s'est liberee, on entre dans la salle d'attente
    siege_t* siege = chercher_siege(vaccinodrome);

    fprintf(stdout, "patient %s siege %d\n", nom, siege->siege);

    // On attend un medecin disponible
    asem_wait(&vaccinodrome->medecinDisponibles);

    // Un medecin est disponible !
    box_t* box = chercher_box(vaccinodrome);

    CHK(asem_wait(&vaccinodrome->siegeMutex));
    siege->statut = 0; // Le siege est a nouveau disponible
    CHK(asem_post(&vaccinodrome->siegeMutex));

    // Une place est disponible dans la salle d'attente
    asem_post(&vaccinodrome->waitingRoom);

    snprintf(box->patient, MAX_NOM_PATIENT, "%s", nom);

    // On demande le vaccin au medecin
    asem_post(&box->demandeVaccin);

    // On attend que le medecin nous vaccine
    asem_wait(&box->termineVaccin);

    // Le client est vaccine !

    fprintf(stdout, "patient %s medecin %d\n", nom, box->medecin);
    adebug(1, "Client vaccine!");

    return 0;
}
