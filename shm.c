#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "shm.h"

void raler(const char *msg)                 // Une fonction simple pour les erreurs
{
    perror(msg);
}

vaccinodrome_t *create_vaccinodrome(int *err, int medecins, int sieges)
{
    int fd;
    vaccinodrome_t *vaccinodrome;
    const int debug = *err == 0; // Si err != 0, alors on utilise raler() sinon adebug()

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

    ssize_t totalSize = sizeof(vaccinodrome_t) + medecins * sizeof(box_t) + sieges * sizeof(siege_t);

    adebug(99, "total size = %d", totalSize);

    if (ftruncate(fd, totalSize) == -1)
    {
        PERR("Imppossible de ftruncate", debug);
        return NULL;
    }

    vaccinodrome = mmap(NULL, totalSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

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
    const int debug = *err == 0; // Si err != 0, alors on utilise raler() sinon adebug()

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


    // On récupère la taille de la memoire partagee avec fstat
    // Car elle est dynamique.
    if (fstat(fd, &stat) == -1)
    {
        PERR("Impossible de fstat", debug);
        return NULL;
    }

    adebug(99, "fstat size = %d", stat.st_size);

    vaccinodrome = mmap(NULL, stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (vaccinodrome == MAP_FAILED)
    {
        PERR("Impossible de mmap", debug);
        return NULL;
    }

//    vaccinodrome->boxes = (box_t*)(vaccinodrome + 1);

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

    // mmap cleanup
    if (munmap(vaccinodrome, sizeof(vaccinodrome_t)) == -1)
    {
        adebug(2, "munmap");
    }

    // shm_open cleanup
    int fd = shm_unlink(TEMP_FILE_NAME);
    if (fd == -1)
    {
        adebug(2, "unlink");
    }

}