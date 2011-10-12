#ifndef SEMAF_H_
#define SEMAF_H_

/* Union usada como quarto argumento da função semctl. */
union semun {
    int             val;
    struct semid_ds *buf;
    unsigned short  *array;
    struct seminfo  *__buf;
};

void semInit();

void semCtl(int id, union semun arg);

void semRemove();

#endif

