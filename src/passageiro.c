#include <stdio.h>
#include <stdlib.h>
#include <shmemo.h>
#include <semaf.h>

#define ESQUERDA 0
#define DIREITA 1

/* Informações sobre este processo. */
static struct {
    int id;
} proc = { 0 };

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
    int margem, memkey, semkey;
    union semun arg;
    struct sembuf wait={0, -1, 0};
    struct sembuf signal={0, 1, 0};

    /* Inicialização da memória compartilhada. */
    memkey = getMemKey();
    SHMinit(memokey, SHM_SIZE);
    
    /* Incialização dos semáforos. */
    arg.val = 1;
    semkey = getSemKey();
    semInit(semkey);
    semCtl(sem.id, arg);
    

    proc.id = getpid();
    printf("PID = %d\n", proc.id);
    /*sleep(100);*/

    if ( argc != 2 ) printf("Passar margem. 0 = Esquerda, 1 = Direita. \n");
    else {
        margem = atoi(argv[1]);
        embarca(margem);
        atravessa(margem);
        desembarca(margem);
    }

    shmDetach(shm.data);
    shmCtl(shm.id); 
    /* imprime passageiro saiu do pier */
    exit(0);
}

