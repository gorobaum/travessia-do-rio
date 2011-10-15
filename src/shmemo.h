
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

/**** !!! AS FUNÇÕES ABAIXO EXIGEM EXCLUSÃO MÚTUA !!! *****/

/* Inicializa a memória compartilhada, verificando se ela
 * já não foi criada. Se já foi, recupera ela. Se não,
 * cria ela. */
void shmInit();

/* Devolve a capacidade atual do barco. */
size_t shmGetShipCapacity();

/* Atualiza a capacidade atual do barco, somando o valor de
 * delta, que pode ser negativo. Devolve a nova capacidade
 * do barco. */
size_t shmUpdateShipCapacity(int delta);

/* Verifica se a margem em que o barco se encontra no
 * momento é a mesma que a indicada. */
int shmCheckShipMargin(int margin);

/* Muda a margem em que o barco está no momento para a
 * margem indicada. */
void shmSetShipMargin(int margin);


#endif
