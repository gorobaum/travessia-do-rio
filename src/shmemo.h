#ifndef SHMEMO_H_
#define SHMEMO_H_

/* Dados compartilhados. */
typedef struct {
    size_t  passenger_num;
    int     ship_current_margin;
    size_t  ship_capacity;
} shm_data;

/* Inicializa a memória compartilhada, verificando se ela
 * já não foi criada. Se já foi, recupera ela. Se não,
 * cria ela. */
void shmInit();

void shmLock();

void shmUnlock();

void shmCleanUp();

/* OBS: essas funções exigem regiões críticas. */

shm_data* shmGet();

void shmUpdateShipCapacity(int delta);


#endif
