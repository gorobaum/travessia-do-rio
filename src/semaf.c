
#include <stdio.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>

#include "semaf.h"

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
    arg.val = 1;
    sem.id = semget( semkey, 1, IPC_CREAT | 0666 );
    semctl(sem.id, 0, SETVAL, arg);
}

void semCtl( int id, union semun arg ) {
    semctl(id, 0, SETVAL, arg);
}

void semRemove() {
    semctl(sem.id, 0, IPC_RMID, 0);
}

