
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>

#include "shmemo.h"

#define SHM_SIZE            (sizeof(shm_data))
#define KEY_PATH_NAME       "src/passageiro.c"
#define KEY_PROJ_ID         'M'

#define MAX_SHIP_CAPACITY   (3)
#define RANDOM_MARGIN       (rand()%2)

/* Dados compartilhados. */
typedef struct {
    /* Os semaforos vão ficar aqui tbm né? */
    /* Os semáforos são obtidos com semget, não precisamos
     * compartilhar eles. */
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

char* getMemo(int memkey, int size) {
    /*shm.id = shmget(memokey, size, IPC_CREAT | S_IRUSR | S_IWUSR);*/
    return (char *)shmat(shm.id, 0, 0);
}

void shmDetach() {
    shmdt((void*)shm.data);
}

void shmRemove() {
    shmctl(shm.id, IPC_RMID, 0);
}





