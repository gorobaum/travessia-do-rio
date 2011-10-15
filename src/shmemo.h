#ifndef SHMEMO_H_
#define SHMEMO_H_

/* Dados compartilhados. */
typedef struct {
    size_t  passenger_num;
    int     ship_current_margin;
    size_t  ship_capacity;
} shm_data;

/* Desacopla a memória compartilhada desse processo.
 * Se nenhum outro processo estiver declaradamente usando
 * os recursos, estes são liberados. */
void shmCleanUp();

/******** !!! AS FUNÇÕES ABAIXO REQUEREM TRAVA !!! ********/

/* Inicializa a memória compartilhada, verificando se ela
 * já não foi criada. Se já foi, recupera ela. Se não,
 * cria ela. */
void shmInit();

/*  */
shm_data* shmGet();

size_t shmGetShipCapacity();

size_t shmUpdateShipCapacity(int delta);

int shmCheckShipMargin(int margin);

void shmSetShipMargin(int margin);


#endif
