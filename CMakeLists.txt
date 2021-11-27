cmake_minimum_required(VERSION 3.10)
project(ProjetASE C)

set(CMAKE_C_STANDARD 99)

file(GLOB OUVRIR_SRC ouvrir.c asem.c asem.h shm.c shm.h)
add_executable(Ouvrir ${OUVRIR_SRC})

file(GLOB NETTOYER_SRC nettoyer.c asem.c asem.h shm.c shm.h)
add_executable(Nettoyer ${NETTOYER_SRC})

file(GLOB MEDECIN_SRC medecin.c asem.c asem.h shm.c shm.h)
add_executable(Medecin ${MEDECIN_SRC})

file(GLOB PATIENT_SRC patient.c asem.c asem.h shm.c shm.h)
add_executable(Patient ${PATIENT_SRC})