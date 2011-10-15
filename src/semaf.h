
#ifndef SEMAF_H_
#define SEMAF_H_

/* Número máximo de operações bufferizáveis. */
#define MAX_NOPS            (8)

/* Operação wait. */
#define OP_WAIT             (-1)
/* Operação signal. */
#define OP_SIGNAL           (1)
/* Operação synchronize. (age como uma barreira) */
#define OP_SYNC             (0)

/* Nós usamos seis semáforos. */
#define SEM_NUM             (6)
/* Semáforo MUTEX da memória compartilhada. */
#define SHM_MUTEX           (0)
/* Semáforos MUTEX de embarcar em cada margem do rio. */
#define EMBARK_MUTEX(margin)  ((margin%2)+1)
/* Semáforos MUTEX de desembarcar em cada margem do rio. */
#define DESEMBARK_MUTEX(margin)  ((margin%2)+3)
/* Semáforo de barreira para a travessia. */
#define PASSAGE_BARRIER     (5)

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

int semExecTimedOps(size_t secs);

/* Operação wait sobre o semáforo indicado. */
void semWait(int semaph);

/* Faz o mesmo que a função acima, exceto que caso os
 * semáforos tenham sido removidos, recria eles. */
void semSafeWait(int semaph);

/* Também faz o mesmo, só que devolve TRUE se o semáforo
 * indicado foi liberado antes de timeout ou FALSE caso
 * contrário. */
int semTimedWait(int semaph, size_t secs);

/* Operação signal sobre o semáforo passado. */
void semSignal(int semaph);

/* Fas o mesmo que a função acima, só que executa as
 * operações adicionadas ao buffer junto com o signal,
 * de maneira atômica. */
void semFinishingSignal(int semaph);

/* Remove os semáforos do sistema. */
void semCleanUp();

#endif

