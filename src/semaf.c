
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <semaphore.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>

#include "semaf.h"

#define SEMKEY_PATH_NAME    "Makefile"
#define SEMKEY_PROJ_ID      'A'

#define SEMOP(op, sem) { sem, op, 0 }

/* Union usada como quarto argumento da função semctl. */
union semun {
    int             val;
    struct semid_ds *buf;
    unsigned short  *array;
    struct seminfo  *__buf;
};

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

static void safe_semop(struct sembuf *ops, size_t nops) {
    if (semop(sem.id, ops, nops) == -1)
        switch(errno) {
            case EIDRM:
                semInit();
                break;
            default:
                puts("Unknown error.");
                exit(EXIT_FAILURE);
                break;
        }
}

/* Espera até que a primeira operação seja executado sobre
 * os semáforos, garantindo que eles estejam inicializados
 * após essa função retornar. */
static int waitFirstOp() {
    struct semid_ds buf;
    union semun arg;

    arg.buf = &buf;
    do if (semctl(sem.id, 0, IPC_STAT, arg) == -1) {
        if (errno == EIDRM) return 0;
        else {
            puts("Unknown error.");
            exit(EXIT_FAILURE);
        }
    }
    while (buf.sem_otime == 0);
    return 1;
}

void semInit() {
    union semun     arg;
    int             semkey = getSemKey();
    unsigned short  initial_values[] = { 0, 0, 0 };
    struct sembuf   signal_shm = SEMOP(OP_SIGNAL, SHM_MUTEX);

    arg.array = initial_values;
    printf("SEM key: 0x%x\n", semkey);
    do {
        sem.id = semget(semkey, SEM_NUM, IPC_CREAT | IPC_EXCL | 0666);
        if (sem.id != -1) {
            semctl(sem.id, 0, SETALL, arg);
            semop(sem.id, &signal_shm, 1);
            break;
        }
        else sem.id = semget(semkey, SEM_NUM, 0666);
    } while ((sem.id == -1 && errno == ENOENT) || !waitFirstOp());
    sem.nops = 0;
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

int semTimedWait(int semaph, size_t secs) {
    struct sembuf   signal = SEMOP(OP_WAIT, 0);
    struct timespec timeout = { 0, 0 };
    signal.sem_num = semaph;
    timeout.tv_sec = secs;
    if (semtimedop(sem.id, &signal, 1, &timeout) == -1)
        if (errno == EAGAIN) return 0;
    return 1;
}

void semSignal(int semaph) {
    struct sembuf signal = SEMOP(OP_SIGNAL, 0);
    signal.sem_num = semaph;
    semop(sem.id, &signal, 1);
}

void semCleanUp() {
    semctl(sem.id, 0, IPC_RMID, 0);
}

