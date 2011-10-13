#ifndef SEMAF_H_
#define SEMAF_H_

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

void semCtl(int id, union semun arg);

/* Remove os semáforos do sistema. */
void semRemove();

#endif

