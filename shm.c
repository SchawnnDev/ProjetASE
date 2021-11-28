// Fichier shm.c à rédiger
#include "shm.h"
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

vaccinodrome_t *create_vaccinodrome()
{
    int fd;
    vaccinodrome_t *vaccinodrome;

    errno = 0;
    fd = shm_open(TEMP_FILE_NAME, O_RDWR | O_CREAT | O_EXCL, 0666);

    if (fd == -1)
    {
        // error;
        if (errno == EEXIST)
        {
            raler("Vaccinodrome deja existant.\n");
        }

        raler("Impossible de shm_open\n");
    }

    if (ftruncate(fd, sizeof(vaccinodrome_t) == -1))
    {
        raler("Imppossible de ftruncate\n");
    }

    vaccinodrome = mmap(NULL, sizeof(vaccinodrome_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (vaccinodrome == MAP_FAILED)
    {
        raler("Impossible de mmap\n");
    }

    if(close(fd) == -1)
    {
        raler("close fd\n");
    }

    return vaccinodrome;
}

vaccinodrome_t *get_vaccinodrome()
{
    int fd;
    vaccinodrome_t *vaccinodrome;

    errno = 0;
    fd = shm_open(TEMP_FILE_NAME, O_RDWR, 0666);

    if (fd == -1)
    {
        // error;
        if (errno == ENOENT)
        {
            raler("Vaccinodrome n'est pas ouvert.\n");
        }

        raler("Impossible de shm_open\n");
    }

    vaccinodrome = mmap(NULL, sizeof(vaccinodrome_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (vaccinodrome == MAP_FAILED)
    {
        raler("Impossible de mmap\n");
    }

    if(close(fd) == -1)
    {
        raler("close fd\n");
    }

    return vaccinodrome;
}

void destroy_vaccinodrome()
{
    int fd;
    vaccinodrome_t *vaccinodrome;

    errno = 0;
    fd = shm_open(TEMP_FILE_NAME, O_RDWR, 0666);

    if (fd == -1)
    {
        // error;
        if (errno == ENOENT)
        {
            raler("Vaccinodrome n'est pas ouvert.\n");
        }

        raler("Impossible de shm_open\n");
    }

    vaccinodrome = mmap(NULL, sizeof(vaccinodrome_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (vaccinodrome == MAP_FAILED)
    {
        raler("Impossible de mmap\n");
    }

    if(close(fd) == -1)
    {
        raler("close fd\n");
    }


    // mmap cleanup
    if (munmap(vaccinodrome, sizeof(vaccinodrome_t)) == -1)
    {
        raler("munmap\n");
    }

    // shm_open cleanup
    fd = shm_unlink(TEMP_FILE_NAME);
    if (fd == -1)
    {
        raler("unlink\n");
    }

    return 1;
}