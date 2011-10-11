#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <shmemo.h>

#define SHM_SIZE    (sizeof(shm_data))

/* Inicializa a memória compartilhada, verificando se ela já não foi criada.
 * Se já foi, recupera ela.
 * Se não, cria ela.
 * Em ambos os casos, a estrutura shm guarda o resultado. */
void SHMinit(int memkey, size_t size) {
    int id;
    if ((id = shmget(memkey, size, IPC_CREAT | IPC_EXCL | 0666)) == -1)
        shm.id = shmget(memkey, size, 0666);
    else
        shm.id = id;
    shm.data = (shm_data*)shmat(shm.id, 0, 0);
}

char* getMemo(int memkey, int size) {
    shm.id = shmget(memokey, size, IPC_CREAT | S_IRUSR | S_IWUSR);
    return (char *)shmat(shm.id, 0, 0);
}
