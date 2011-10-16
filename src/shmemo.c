
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>

#include "defs.h"
#include "shmemo.h"
#include "semaf.h"

/* ====================================================== */

/* Tamanho da memória compartilhada. */
#define SHM_SIZE        (sizeof(shm_data))

/* Usados para gerar a chave da memória compartilhada. */
#define KEY_PATH_NAME   "src/passageiro.c"
#define KEY_PROJ_ID     'M'

/* Gera uma margem aleatoriamente. */
#define RANDOM_MARGIN   (rand()%2)

/* ====================================================== */

/* Dados compartilhados. */
typedef struct {
    size_t  passenger_num;
    int     ship_current_position;
    size_t  ship_capacity;
} shm_data;

/* Memória compartilhada. */
static struct {
    int         id;
    shm_data    *data;
} shm = { -1, NULL };

/* ====================================================== */

/* Obtém a chave para recuperar ou criar a memória
 * compartilhada. */
static int getMemKey() {
    return ftok(KEY_PATH_NAME, KEY_PROJ_ID);
}

/* Carrega os valores padrão na memória compartilhada. */
static void loadDefaultValues() {
    shm.data->passenger_num = 0;
    shm.data->ship_current_position = RANDOM_MARGIN;
    shm.data->ship_capacity = MAX_CAP;
    printf("[BARCO] Inicializado na margem %s\n",
           margins[shm.data->ship_current_position]);
}

/* ====================================================== */

void shmInit() {
    int do_init = FALSE,
        memkey = getMemKey();

    if ((shm.id = shmget(memkey, SHM_SIZE, 0666)) == -1) {
        shm.id = shmget(memkey, SHM_SIZE, IPC_CREAT | 0666);
        do_init = TRUE; /* precisa inicializar a memória */
    }
    shm.data = (shm_data*)shmat(shm.id, 0, 0);
    if (do_init) {
        printf("[INFO] Memória compartilhada inicializada com chave %x.\n",
               memkey);
        loadDefaultValues();
    }
    shm.data->passenger_num++;
}

size_t shmGetShipCapacity() {
    return shm.data->ship_capacity;
}

size_t shmUpdateShipCapacity(int delta) {
    return (shm.data->ship_capacity += delta);
}

int shmCheckShipPosition(int margin) {
    return shm.data->ship_current_position == margin;
}

void shmSetShipPosition(int margin) {
    shm.data->ship_current_position = margin;
}

void shmCleanUp() {
    int passenger_num;
    
    semWait(SHM_MUTEX);
    passenger_num = --shm.data->passenger_num;
    shmdt((void*)shm.data);
    shm.data = NULL;
    if (passenger_num == 0) {
        /* Ninguém mais está declaradamente usando os
         * recursos. */
        puts("[INFO] Removendo recursos do sistema.");
        shmctl(shm.id, IPC_RMID, 0);
        semCleanUp();
    }
    else semSignal(SHM_MUTEX);
}





