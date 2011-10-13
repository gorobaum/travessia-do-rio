
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <semaphore.h>
#include <sys/sem.h>
#include <unistd.h>

#include "shmemo.h"
#include "semaf.h"

/* Informações sobre este passageiroesso. */
static struct {
    int id;
    int margem;
} passageiro = { 0, 0 };

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

    if ( argc != 2 ) printf("Passar margem. 0 = Esquerda, 1 = Direita. \n");
    else {
        /* Semente para o RNG. */
        srand(time(NULL));

        /* Incialização dos semáforos. */
        semInit();

        /* Inicialização da memória compartilhada. */
        shmInit();

        passageiro.id = getpid();
        printf("PID = %d\n", passageiro.id);

        passageiro.margem = atoi(argv[1]);
        embarca(passageiro.margem);
        atravessa(passageiro.margem);
        desembarca(passageiro.margem);

        shmDetach();
        shmRemove(); 
        semCleanUp();
    }

    /* imprime passageiro saiu do pier */
    return EXIT_SUCCESS;
}

