
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <semaphore.h>
#include <sys/sem.h>
#include <unistd.h>

#include "defs.h"
#include "shmemo.h"
#include "semaf.h"

/* Informações sobre este passageiroesso. */
static struct {
    int id;
    int margem;
} passageiro = { 0, 0 };

char *margens[2] = { "ESQUERDA", "DIREITA" };

static void init() {
    semInit();
    semSafeWait(SHM_MUTEX);
    shmInit();
    semSignal(SHM_MUTEX);
}

void embarca(int margem) {
    shm_data *data;
    if(!semTimedWait(EMBARK_MUTEX(margem), 10)) {
        printf("Passageiro 0x%x desistiu da viagem.\n", passageiro.id);
        exit(EXIT_SUCCESS);
    }
    /*shmUpdateShipCapacity(-1);  */
    semWait(SHM_MUTEX);
    data = shmGet();
    if (--data->ship_capacity)
        semAddOp(EMBARK_MUTEX(margem), OP_SIGNAL);
        /*semSignal(EMBARK_MUTEX(margem));*/
    else {
        data->ship_current_margin = !margem;
        semAddOp(PASSAGE_BARRIER, 3*OP_SIGNAL);
        semAddOp(DESEMBARK_MUTEX(!margem), OP_SIGNAL);
    }
    semFinishingSignal(SHM_MUTEX);
}

void desembarca(int margem) {
    /* Aqui o passageiro desembarca do barco vindo da margem especificada
    e realiza quaisquer outras tarefas para dar continuidade a viagem
    de outros passageiros */
    shm_data *data;
    semAddOp(PASSAGE_BARRIER, OP_WAIT);
    semAddOp(DESEMBARK_MUTEX(!margem), OP_WAIT);
    semAddOp(SHM_MUTEX, OP_WAIT);
    semExecOps();
    data = shmGet();
    if (++data->ship_capacity < 3)
        semAddOp(DESEMBARK_MUTEX(!margem), OP_SIGNAL);
    else
        semAddOp(EMBARK_MUTEX(!margem), OP_SIGNAL);
    semFinishingSignal(SHM_MUTEX);
    passageiro.margem = !margem;
}

void atravessa(int margem) {
    /* O barco atravessa o rio a partir da margem especificada */
    printf("Passageiro 0x%x atravessando da margem %s para a margem %s.\n",
           passageiro.id,
           margens[passageiro.margem],
           margens[!passageiro.margem]);
}

void cleanUp() {
    shmCleanUp();
}

int main(int argc, char *argv[]) {

    if ( argc != 2 ) printf("Passar margem. 0 = Esquerda, 1 = Direita. \n");
    else {
        /* Semente para o RNG. */
        srand(time(NULL));

        /* Inicializa recursos para esse processo. */
        init();

        atexit(cleanUp);

        passageiro.id = getpid();
        printf("PID = %d\n", passageiro.id);

        passageiro.margem = atoi(argv[1]);
        
        embarca(passageiro.margem);
        atravessa(passageiro.margem);
        desembarca(passageiro.margem);
    }

    /*cleanUp();*/

    /* imprime passageiro saiu do pier */
    return EXIT_SUCCESS;
}

