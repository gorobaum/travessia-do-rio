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


void embarca(int margem)
{
    /* Aqui o passageiro embarca na margem especificada se possivel
    ou espera o barco chegar a sua margem do rio */
}
void desembarca(int margem)
{
    /* Aqui o passageiro desembarca do barco vindo da margem especificada
    e realiza quaisquer outras tarefas para dar continuidade a viagem
    de outros passageiros */
}
void atravessa(int margem)
{
    /* O barco atravessa o rio a partir da margem especificada */
}
int main(int argc, char *argv[])
{
    int margem;
    if ( argc != 2 ) printf("Passar margem. 0 = Esquerda, 1 = Direita. \n");
    else {
        margem = atoi(argv[1]);
        embarca(1);
        atravessa(1);
        desembarca(1);
    }
    /* imprime passageiro saiu do pier */
    exit(0);
}

