#ifndef SEMAF_H_
#define SEMAF_H_

/* Union usada como quarto argumento da função semctl. */
union semun {
    int             val;
    struct semid_ds *buf;
    unsigned short  *array;
    struct seminfo  *__buf;
};

/* Semáforos. */
static struct {
    int id;
} sem = { -1 };

void semInit();
int getSemKey() { ftok( "Makefile", 'A'); }

#endif
