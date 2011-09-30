#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>

#define ESQUERDA 0
#define DIREITA 1

union semun {
    int val; struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};


char* getMemo(int memokey, int size) {
    int shmid;
    shmid = shmget(memokey, SHM_SIZE, IPC_CREAT | S_IRUSR | S_IWUSR);
    return (char *)shmat(shmid, 0, 0);
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
    int margem, memokey = 1;
    char* saddr;
    union semun arg;
    struct sembuf wait={0, -1, 0};
    struct sembuf signal={0, 1, 0};

    saddr = getMemo(memokey, 10*sizeof(int) );
    if ( saddr[0] == 0 ) {};


    if ( argc != 2 ) printf("Passar margem. 0 = Esquerda, 1 = Direita. \n");
    else {
        margem = atoi(argv[1]);
        embarca(margem);
        atravessa(margem);
        desembarca(margem);
    }
    /* imprime passageiro saiu do pier */
    exit(0);
}

