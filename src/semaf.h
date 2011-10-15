
#ifndef SEMAF_H_
#define SEMAF_H_

/* Tamanho do buffer de operações de semáforos. */
#define MAX_NOPS                (8)

/* Operação wait. */
#define OP_WAIT                 (-1)
/* Operação signal. */
#define OP_SIGNAL               (1)
/* Operação synchronize. (age como uma barreira) */
#define OP_SYNC                 (0)

/* Nós usamos três semáforos. */
#define SEM_NUM                 (3)
/* Semáforo MUTEX da memória compartilhada. */
#define SHM_MUTEX               (0)
/* Semáforo de barreira para embarcar. */
#define EMBARK                  (1)
/* Semáforo de barreira para desembarcar. */
#define DESEMBARK               (2)

/* Inicializa os semáforos para esse proccesso.
 * Se os semáforos ainda não existirem, cria eles. */
void semInit();

/* Adiciona a operação indicada no semáforo específicado.
 * Para executar as operações adicionadas, basta chamar a
 * função semExecOps. Só podem ser adicionadas até MAX_NOPS
 * operações. */
void semAddOp(int semaph, int op);

/* Executa seqüencialmente e atomicamente as operações
 * que foram adicionadas ao buffer, e depois limpa o
 * buffer. */
void semExecOps();

/* Operação wait sobre o semáforo indicado. */
void semWait(int semaph);

/* Faz o mesmo que a função acima, exceto que caso os
 * semáforos tenham sido removidos, recria eles. */
void semSafeWait(int semaph);

/* Operação signal sobre o semáforo passado. */
void semSignal(int semaph);

/* Operação wait-zero sobre o semáforo passado.
 * Funciona como uma barreira. */
void semSync(int semaph);

/* Faz o mesmo que a função acima, só que executa as
 * operações adicionadas ao buffer e depois faz o signal,
 * tudo de maneira atômica. */
void semFinishingSignal(int semaph);

/* Remove os semáforos do sistema. */
void semCleanUp();

#endif

