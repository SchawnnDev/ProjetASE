#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "shm.h"

void raler(const char *msg) // Une fonction simple pour les erreurs
{
    perror(msg);
}

vaccinodrome_t *create_vaccinodrome(int *err, int medecins, int sieges)
{
    int fd;
    vaccinodrome_t *vaccinodrome;
    const int debug = *err == 0;
    // Si err != 0, alors on utilise raler() sinon adebug()

    *err = -1;
    errno = 0;
    fd = shm_open(TEMP_FILE_NAME, O_RDWR | O_CREAT | O_EXCL, 0666);

    if (fd == -1)
    {
        // error;
        if (errno == EEXIST)
        {
            PERR("Vaccinodrome deja existant", debug);
            *err = -2;
            return NULL;
        }

        PERR("Impossible de shm_open", debug);
        return NULL;
    }

    ssize_t totalSize = sizeof(vaccinodrome_t)
            + medecins * sizeof(box_t) + sieges * sizeof(siege_t);

    adebug(99, "total size = %d", totalSize);

    if (ftruncate(fd, totalSize) == -1)
    {
        PERR("Imppossible de ftruncate", debug);
        return NULL;
    }

    vaccinodrome = mmap(NULL, totalSize,
                        PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (vaccinodrome == MAP_FAILED)
    {
        PERR("Impossible de mmap", debug);
        return NULL;
    }

    if (close(fd) == -1)
    {
        PERR("close fd", debug);
        return NULL;
    }

    *err = 0;
    return vaccinodrome;
}

vaccinodrome_t *get_vaccinodrome(int *err)
{
    adebug(99, "get_vaccinodrome()");
    int fd;
    vaccinodrome_t *vaccinodrome;
    struct stat stat;
    const int debug = *err == 0;
    // Si err != 0, alors on utilise raler() sinon adebug()

    *err = -1;
    errno = 0;
    fd = shm_open(TEMP_FILE_NAME, O_RDWR, 0666);

    if (fd == -1)
    {
        if (errno == ENOENT)
        {
            PERR("Vaccinodrome n'est pas ouvert", debug);
            return NULL;
        }

        PERR("Impossible de shm_open", debug);
        return NULL;
    }


    // On r??cup??re la taille de la memoire partagee avec fstat
    // Car elle est dynamique.
    if (fstat(fd, &stat) == -1)
    {
        PERR("Impossible de fstat", debug);
        return NULL;
    }

    adebug(99, "fstat size = %d", stat.st_size);

    vaccinodrome = mmap(NULL, stat.st_size,
                        PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (vaccinodrome == MAP_FAILED)
    {
        PERR("Impossible de mmap", debug);
        return NULL;
    }

    if (close(fd) == -1)
    {
        PERR("close fd", debug);
        return NULL;
    }

    *err = 0;
    return vaccinodrome;
}

void destroy_vaccinodrome(vaccinodrome_t *vaccinodrome)
{
    // on d??truit toutes les s??maphores
    asem_destroy(&vaccinodrome->waitingRoom);
    asem_destroy(&vaccinodrome->medecinDisponibles);
    asem_destroy(&vaccinodrome->asemMutex);
    asem_destroy(&vaccinodrome->siegeMutex);
    asem_destroy(&vaccinodrome->waitingMutex);
    asem_destroy(&vaccinodrome->fermer);

    for (int i = 0; i < vaccinodrome->currMedecins; ++i)
    {
        box_t* box = &vaccinodrome->medecins[i];
        asem_destroy(&box->termineVaccin);
        asem_destroy(&box->demandeVaccin);
    }

    // mmap cleanup
    if (munmap(vaccinodrome, sizeof(vaccinodrome_t)) == -1)
        adebug(2, "munmap");

    // shm_open cleanup
    int fd = shm_unlink(TEMP_FILE_NAME);
    if (fd == -1)
        adebug(2, "unlink");

}

siege_t *find_siege(vaccinodrome_t *vaccinodrome, int statut)
{
    siege_t *siege = NULL;

    for (int i = 0; i < vaccinodrome->nbSieges; ++i)
    {
        siege = get_siege_at(vaccinodrome, i);
        if (siege->statut == statut) break;
        siege = NULL;
    }

    return siege;
}

siege_t *get_sieges(vaccinodrome_t *vaccinodrome)
{
    return (void *) (vaccinodrome->medecins + vaccinodrome->nbMedecins);
}

siege_t *get_siege_at(vaccinodrome_t *vaccinodrome, int i)
{
    return get_sieges(vaccinodrome) + i;
}

int count_sieges_occupes(vaccinodrome_t *vaccinodrome)
{
    int result = 0;

    for (int i = 0; i < vaccinodrome->nbSieges; ++i)
    {
        if (get_siege_at(vaccinodrome, i)->statut != 1)
            continue;
        result++;
    }

    return result;
}