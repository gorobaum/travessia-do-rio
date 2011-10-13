
#include <stdio.h>
#include <stdlib.h>
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

/* Dados compartilhados. */
typedef struct {
    size_t  passenger_num;
    int     ship_current_margin;
    size_t  ship_capacity;
} shm_data;

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
    shm.data->ship_current_margin = RANDOM_MARGIN;
    shm.data->ship_capacity = MAX_SHIP_CAPACITY;
    if(shm.data->ship_current_margin == ESQUERDA) semSignal(EMBLEFT);
    else semSignal(EMBRIGHT);
}

void shmInit() {
    int do_init = 0,
        memkey = getMemKey();
    printf("SHM key: %x\n", memkey);
    semWait(SHM_MUTEX); 
    if ((shm.id = shmget(memkey, SHM_SIZE, 0666)) == -1) {
        shm.id = shmget(memkey, SHM_SIZE, IPC_CREAT | 0666);
        do_init = 1;
    }
    shm.data = (shm_data*)shmat(shm.id, 0, 0);
    if (do_init) loadDefaultValues();
    shm.data->passenger_num++;
    semSignal(SHM_MUTEX);
}

void shmCleanUp() {
    int passenger_num;
    
    semWait(SHM_MUTEX);
    passenger_num = shm.data->passenger_num--;
    shmDetach();
    if ( passenger_num == 0 ) shmRemove();
    semSignal(SHM_MUTEX);
}

void shmDetach() {
    shmdt((void*)shm.data);
}

void shmRemove() {
    shmctl(shm.id, IPC_RMID, 0);
}

void shmShipCapacityUpdate(int delta) {
    semWait(SHM_MUTEX);
    shm.data->ship_capacity += delta;
    semSignal(SHM_MUTEX);
}

