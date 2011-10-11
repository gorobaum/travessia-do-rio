#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>

#define ESQUERDA 0
#define DIREITA 1

/* Union usada como quarto argumento da função semctl. */
union semun {
    int             val;
    struct semid_ds *buf;
    unsigned short  *array;
    struct seminfo  *__buf;
};

/* Dados compartilhados. */
typedef struct {

} shm_data;

/* Informações sobreeste processo. */
static struct {
    int id;
} proc = { 0 };

/* Memória compartilhada. */
static struct {
    int         id;
    shm_data    *data;
} shm = { -1, NULL };

/* Semáforos. */
static struct {
    int id;
} sem = { -1 };

/*
static int shmid;
static int semid;
static int pid = 0;
*/

/* Inicializa a memória compartilhada, verificando se ela já foi criada.
 * Se já foi, adquire o ID dela.
 * Se não, cria ela e armazena o ID dela.
 * Em ambos os casos, shm.id recebe o ID. */
void SHMinit(int memkey, size_t size) {
    int id;
    if ((id = shmget(memkey, size, IPC_CREAT | IPC_EXCL | 0666)) == -1)
        return shmget(memkey, size, 0666);
    else
        return id;
}

char* getMemo(int memokey, int size) {
    shm.id = shmget(memokey, size, IPC_CREAT | S_IRUSR | S_IWUSR);
    return (char *)shmat(shm.id, 0, 0);
}

void semInit(int semkey) {
    sem.id = semget( semkey, 1, IPC_CREAT | S_IRUSR | S_IWUSR );
}

void embarca(int margem) {
    /* Aqui o passageiro embarca na margem especificada se possivel
    ou espera o barco chegar a sua margem do rio */
}

void desembarca(int margem) {
    /* Aqui o passageiro desembarca do barco vindo da margem especificada
    e realiza quaisquer outras tarefas para dar continuidade a viagem
    de outros passageiros */
}

void atravessa(int margem) {
    /* O barco atravessa o rio a partir da margem especificada */
}

int main(int argc, char *argv[]) {
    int margem, memokey, /*pid = 0,*/ semkey;
    char* saddr;
    union semun arg;
    struct sembuf wait={0, -1, 0};
    struct sembuf signal={0, 1, 0};

    arg.val = 1;
    memokey = ftok( "src/passageiro.c", 'M' );
    semkey = ftok( "Makefile", 'A');
    saddr = getMemo(memokey, 10*sizeof(int) );
    semInit(semkey);
    semctl(sem.id, 0, SETVAL, arg);

    proc.id = getpid();
    printf("SADDR = %d PID = %d\n", (int)saddr, proc.id);
    /*sleep(100);*/

    if ( argc != 2 ) printf("Passar margem. 0 = Esquerda, 1 = Direita. \n");
    else {
        margem = atoi(argv[1]);
        embarca(margem);
        atravessa(margem);
        desembarca(margem);
    }
    shmdt(saddr);
    shmctl(shm.id, IPC_RMID, 0);
    /* imprime passageiro saiu do pier */
    exit(0);
}

