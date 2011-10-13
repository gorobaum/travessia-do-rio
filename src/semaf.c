
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

#define SEMOP(op, sem) { sem, op, 0 }

/* Semáforos. */
static struct {
    int             id;
    size_t          nops;
    struct sembuf   ops[MAX_NOPS];
} sem;

/* Devolve a chave usada para identificar os semáforos. */
static int getSemKey() {
    return ftok( SEMKEY_PATH_NAME, SEMKEY_PROJ_ID);
}

/* Espera até que a primeira operação seja executado sobre
 * os semáforos, garantindo que eles estejam inicializados
 * após essa função retornar. */
static void waitFirstOp() {
    struct semid_ds buf;
    union semun arg;

    arg.buf = &buf;
    do semctl(sem.id, 0, IPC_STAT, arg);
    while (buf.sem_otime == 0);
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
    }
    else sem.id = semget(semkey, SEM_NUM, 0666 );
    sem.nops = 0;
    waitFirstOp();
}

void semAddOp(int semaph, int op) {
    if (sem.nops == MAX_NOPS)
        semExecOps();
    sem.ops[sem.nops].sem_num = semaph;
    sem.ops[sem.nops].sem_op = op;
    sem.ops[sem.nops].sem_flg = 0;
    sem.nops++;
}

void semExecOps() {
    if (sem.nops > 0) {
        semop(sem.id, sem.ops, sem.nops);
        sem.nops = 0;
    }
}

void semWait(int semaph) {
    struct sembuf signal = SEMOP(OP_WAIT, 0);
    signal.sem_num = semaph;
    semop(sem.id, &signal, 1);
}

void semSignal(int semaph) {
    struct sembuf signal = SEMOP(OP_SIGNAL, 0);
    signal.sem_num = semaph;
    semop(sem.id, &signal, 1);
}

void semCleanUp() {
    semctl(sem.id, 0, IPC_RMID, 0);
}

