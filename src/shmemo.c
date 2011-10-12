
#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <shmemo.h>

#define SHM_SIZE        (sizeof(shm_data))
#define KEY_PATH_NAME   "src/passageiro.c"
#define KEY_PROJ_ID     'M'

/* Dados compartilhados. */
typedef struct {
    int current_margin; /* Os semaforos vão ficar aqui tbm né? */
    /* Os semáforos são obtidos com semget, não precisamos
     * compartilhar eles. */
} shm_data;

/* Memória compartilhada. */
static struct {
    int         id;
    shm_data    *data;
} shm = { -1, NULL };

/* Obtém a chave para recuperar ou criar a memória compartilhada. */
static int getMemKey() {
    return ftok(KEY_PATH_NAME, KEY_PROJ_ID);
}

void shmInit() {
    int id, memkey = getMemKey();
    printf("SHM key: %x\n", memkey);
    if ((id = shmget(memkey, SHM_SIZE, IPC_CREAT | IPC_EXCL | 0666)) == -1)
        shm.id = shmget(memkey, SHM_SIZE, 0666);
    else
        shm.id = id;
    shm.data = (shm_data*)shmat(shm.id, 0, 0);
}

char* getMemo(int memkey, int size) {
    /*shm.id = shmget(memokey, size, IPC_CREAT | S_IRUSR | S_IWUSR);*/
    return (char *)shmat(shm.id, 0, 0);
}

void shmDetach() {
    shmdt((void*)shm.data);
}

void shmRemove() {
    shmctl(shm.id, IPC_RMID, 0);
}





