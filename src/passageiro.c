
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <semaphore.h>
#include <sys/sem.h>
#include <unistd.h>

#include "shmemo.h"
#include "semaf.h"

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

    srand(time(NULL));

    /* Incialização dos semáforos. */
    semInit();

    /* Inicialização da memória compartilhada. */
    /*memkey = getMemKey();*/
    shmInit();

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

    shmDetach();
    shmRemove(); 
    semRemove();
    /* imprime passageiro saiu do pier */
    return EXIT_SUCCESS;
}

