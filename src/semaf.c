
#include <stdio.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>

#include "semaf.h"

#define SEMKEY_PATH_NAME    "Makefile"
#define SEMKEY_PROJ_ID      'A'

#define OP_NUM              (2)
#define OP_WAIT             (-1)
#define OP_SIGNAL           (1)

/* Acho que vamos precisar de quatro semáforos. */
#define SEM_NUM             (3)
/* Semáforo MUTEX da memória compartilhada. */
#define SHM_MUTEX           (0)
/* Semáforos MUTEX para entrar no barco em cada margem do rio. */
#define SHIP_MUTEX(margin)  ((margin%2)+1)

#define SEMOP(op, sem) { sem, op, 0 }

/* Semáforos. */
static struct {
    int id;
    int lock;
} sem = { -1, -1 };

static int getSemKey() {
    return ftok( SEMKEY_PATH_NAME, SEMKEY_PROJ_ID);
}

void signalInit(int semkey) {
    if ((sem.lock = shmget(semkey, 4, IPC_CREAT | 0666)) == -1)
        puts("OH NOES");
}

void waitInit(int semkey) {
    if (sem.lock == -1)
        while ((sem.lock = shmget(semkey, 0, 0666)) == -1);
}

void semInit() {
    union semun     arg;
    int             semkey = getSemKey();
    unsigned short  initial_values[] = { 0, 0, 0 };
    struct sembuf   signal_shm = SEMOP(OP_SIGNAL, SHM_MUTEX);

    arg.array = initial_values;
    printf("SEM key: 0x%x\n", semkey);
    if ((sem.id = semget(semkey, SEM_NUM, IPC_CREAT | IPC_EXCL | 0666)) != -1) {
        semctl(sem.id, 0, SETALL, arg);
        semop(sem.id, &signal_shm, 1);
        signalInit(semkey);
    }
    else sem.id = semget(semkey, SEM_NUM, 0666 );
    waitInit(semkey);
}

void semCtl( int id, union semun arg ) {
    semctl(id, 0, SETVAL, arg);
}

void semRemove() {
    semctl(sem.id, 0, IPC_RMID, 0);
    shmctl(sem.lock, IPC_RMID, 0);
}

