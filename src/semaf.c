
#include <stdio.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>

#include "semaf.h"

/* Acho que vamos precisar de quatro semáforos. */
#define SEM_NUM             (3)
/* Semáforo MUTEX da memória compartilhada. */
#define SHM_MUTEX           (0)
/* Semáforos MUTEX para entrar no barco em cada margem do rio. */
#define SHIP_MUTEX(margin)  ((margin%2)+1)

/* Semáforos. */
static struct {
    int id;
} sem = { -1 };

static int getSemKey() {
    return ftok( "Makefile", 'A');
}

void semInit() {
    union semun arg;
    int semkey = getSemKey();

    printf("SEM key: %x\n", semkey);
    sem.id = semget( semkey, SEM_NUM, IPC_CREAT | 0666 );
    arg.val = 1;
    semctl(sem.id, 0, SETVAL, arg);
}

void semCtl( int id, union semun arg ) {
    semctl(id, 0, SETVAL, arg);
}

void semRemove() {
    semctl(sem.id, 0, IPC_RMID, 0);
}

