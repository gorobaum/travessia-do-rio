#ifndef SEMAF_H_
#define SEMAF_H_

/* Número máximo de operações bufferizáveis. */
#define MAX_NOPS            (8)

/* Operação wait. */
#define OP_WAIT             (-1)
/* Operação signal. */
#define OP_SIGNAL           (1)

/* Nós usamos seis semáforos. */
#define SEM_NUM             (6)
/* Semáforo MUTEX da memória compartilhada. */
#define SHM_MUTEX           (0)
#define EMBLEFT             (1)
#define EMBRIGHT            (2)
#define DESLEFT             (3)
#define DESRIGHT            (4)
#define T                   (5)

/* Semáforos MUTEX para cada margem do rio. */
#define SHIP_MUTEX(margin)  ((margin%2)+1)

/* Margens do rio. */
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

/* Adiciona uma operação op no semáforo específicado.
 * Para executar as operações adicionadas, chamar a função
 * semExecOps. Só podem ser adicionadas até MAX_NOPS
 * operações. */
void semAddOp(int semaph, int op);

/* Executa as operações que foram adicionadas de maneira
 * atômica, e depois limpa o buffer de operações. */
void semExecOps();

/* Operação wait sobre o semáforo indicado. */
void semWait(int semaph);

/* Operação signal sobre o semáforo passado. */
void semSignal(int semaph);

/* Depracated? */
void semCtl(int id, union semun arg);

/* Remove os semáforos do sistema. */
void semRemove();

#endif

