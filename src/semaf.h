#ifndef SEMAF_H_
#define SEMAF_H_

#define MAX_NOPS            (8)

#define OP_WAIT             (-1)
#define OP_SIGNAL           (1)

/* Acho que vamos precisar de quatro semáforos. */
#define SEM_NUM             (3)
/* Semáforo MUTEX da memória compartilhada. */
#define SHM_MUTEX           (0)
/* Semáforos MUTEX para cada margem do rio. */
#define SHIP_MUTEX(margin)  ((margin%2)+1)

#define ESQUERDA            (0)
#define DIREITA             (1)

/* Union usada como quarto argumento da função semctl. */
union semun {
    int             val;
    struct semid_ds *buf;
    unsigned short  *array;
    struct seminfo  *__buf;
};

/* Inicializa os semáforos para esse proccesso.
 * Se os semáforos ainda não existirem, cria eles. */
void semInit();

void semAddOp(int semaph, int op);

void semExecOps();

void semWait(int semaph);

void semSignal(int semaph);

void semCtl(int id, union semun arg);

/* Remove os semáforos do sistema. */
void semRemove();

#endif

