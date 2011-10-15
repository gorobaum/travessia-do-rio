
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>

#include "shmemo.h"
#include "semaf.h"

#define SHM_SIZE            (sizeof(shm_data))
#define KEY_PATH_NAME       "src/passageiro.c"
#define KEY_PROJ_ID         'M'

#define MAX_SHIP_CAPACITY   (3)
#define RANDOM_MARGIN       (rand()%2)

/* Memória compartilhada. */
static struct {
    int         id;
    shm_data    *data;
} shm = { -1, NULL };

/* Obtém a chave para recuperar ou criar a memória compartilhada. */
static int getMemKey() {
    return ftok(KEY_PATH_NAME, KEY_PROJ_ID);
}

static void loadDefaultValues() {
    shm.data->passenger_num = 0;
    /*shm.data->ship_current_margin = RANDOM_MARGIN;*/
    shm.data->ship_current_margin = 0;
    shm.data->ship_capacity = MAX_SHIP_CAPACITY;
    /* Libera o barco para os passageiros embarcarem.
    semAddOp(EMBARK_MUTEX(shm.data->ship_current_margin), 3*OP_SIGNAL);
    semExecOps();
    */
    printf("[BARCO] Inicializado na margem %d\n",
           shm.data->ship_current_margin);
}

void shmInit() {
    int do_init = 0,
        memkey = getMemKey();
    printf("SHM key: %x\n", memkey);
    if ((shm.id = shmget(memkey, SHM_SIZE, 0666)) == -1) {
        shm.id = shmget(memkey, SHM_SIZE, IPC_CREAT | 0666);
        do_init = 1;
    }
    shm.data = (shm_data*)shmat(shm.id, 0, 0);
    if (do_init) loadDefaultValues();
    shm.data->passenger_num++;
}

int shmCheck() {
    struct shmid_ds buf;
    return !(shmctl(shm.id, IPC_STAT, &buf) == -1 && errno == EIDRM);
}

void shmLock() {
    semWait(SHM_MUTEX);
}

void shmUnlock() {
    semSignal(SHM_MUTEX);
}

void shmUpdateShipCapacity(int delta) {
    semWait(SHM_MUTEX);
    shm.data->ship_capacity += delta;
    semSignal(SHM_MUTEX);
}

void shmCleanUp() {
    int passenger_num;
    
    semWait(SHM_MUTEX);
    passenger_num = --shm.data->passenger_num;
    shmdt((void*)shm.data);
    shm.data = NULL;
    if ( passenger_num == 0 ) { 
        puts("Removendo recursos do sistema.");
        shmctl(shm.id, IPC_RMID, 0);
        semCleanUp();
    }
    else semSignal(SHM_MUTEX);
}

shm_data* shmGet() {
    return shm.data;
}





