cmake_minimum_required(VERSION 3.10)
project(ProjetASE C)

set(CMAKE_C_STANDARD 99)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_library(LIBRT rt)

file(GLOB OUVRIR_SRC ouvrir.c asem.c asem.h shm.c shm.h)
add_executable(Ouvrir ${OUVRIR_SRC})
target_link_libraries(Ouvrir PRIVATE Threads::Threads ${LIBRT})

file(GLOB NETTOYER_SRC nettoyer.c asem.c asem.h shm.c shm.h)
add_executable(Nettoyer ${NETTOYER_SRC})
target_link_libraries(Nettoyer PRIVATE Threads::Threads ${LIBRT})

file(GLOB MEDECIN_SRC medecin.c asem.c asem.h shm.c shm.h)
add_executable(Medecin ${MEDECIN_SRC})
target_link_libraries(Medecin PRIVATE Threads::Threads ${LIBRT})

file(GLOB PATIENT_SRC patient.c asem.c asem.h shm.c shm.h)
add_executable(Patient ${PATIENT_SRC})
target_link_libraries(Patient PRIVATE Threads::Threads ${LIBRT})

file(GLOB FERMER_SRC fermer.c asem.c asem.h shm.c shm.h)
add_executable(Fermer ${FERMER_SRC})
target_link_libraries(Fermer PRIVATE Threads::Threads ${LIBRT})