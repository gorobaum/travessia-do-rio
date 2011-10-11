#ifndef SHMEMO_H_
#define SHMEMO_H_

/* Dados compartilhados. */
typedef struct {
    int current_margin; /* Os semaforos vão ficar aqui tbm né? */
} shm_data;

/* Memória compartilhada. */
static struct {
    int         id;
    shm_data    *data;
} shm = { -1, NULL };

void SHMinit(int memkey, size_t size);
char* getMemo(int memkey, int size);
int getMemKey() { return ftok( "src/passageiro.c", 'M' ); }
void shmDetach(int shaddr) { shmdt(shaddr); }
void shmCtl(int id) { shmctl(id, IPC_RMID, 0); } 

#endif
