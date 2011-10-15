
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <semaphore.h>
#include <sys/sem.h>
#include <unistd.h>

#include "defs.h"
#include "shmemo.h"
#include "semaf.h"

#define MAX_TIMEOUT 10

/* Informações sobre este passageiroesso. */
static struct {
    int     id;
    int     margem;
    size_t  countdown;
} passageiro = { 0, 0, MAX_TIMEOUT };

static char *margens[2] = { "ESQUERDA", "DIREITA" };

static void init() {
    semInit();
    semSafeWait(SHM_MUTEX);
    shmInit();
    semSignal(SHM_MUTEX);
}

static void checkTime(size_t count) {
    if (!count) {
        printf("[PASSAGEIRO %d] Desistiu da viagem.\n", passageiro.id);
        exit(EXIT_SUCCESS);
    }
}

static size_t tryToEmbark(int margem, size_t count) {
    for (; count; count--) {
        semWait(SHM_MUTEX);
        if (shmCheckShipMargin(margem) && shmGetShipCapacity() > 0) {
            if (shmUpdateShipCapacity(-1) == 0) {
                semAddOp(EMBARK_MUTEX(margem), 3*OP_SIGNAL);
                semExecOps();
            }
            semSignal(SHM_MUTEX);
            break;
        } else {
            semSignal(SHM_MUTEX);
            sleep(1);
        }
    }
    checkTime(count);
    return count;
}

static void giveUp(size_t count) {
    for (; count; count--) {
        semWait(SHM_MUTEX);
        if (shmGetShipCapacity() == 0) {
            semSignal(SHM_MUTEX);
            break;
        } else if (count == 1) {
            shmUpdateShipCapacity(1);
            semSignal(SHM_MUTEX);
        } else {
            semSignal(SHM_MUTEX);
            sleep(1);
        }
    }
    checkTime(count);
}

void embarca(int margem) {
    shm_data    *data;
    size_t      count = MAX_TIMEOUT;

    printf("[PASSAGEIRO %d] Esperando na margem %s.\n",
            passageiro.id,
            margens[margem]);
    
    /* O passageiro tenta embarcar no braco. */
    count = tryToEmbark(margem, count);

    /* Já dentro do barco, o passageiro decide se continua ou desiste da viagem. */
    giveUp(count);

    printf("[PASSAGEIRO %d] Embarcando na margem %s.\n",
           passageiro.id,
           margens[margem]);

    /*printf("[INFO] Barreira com %d.\n", semCheck(EMBARK_MUTEX(margem)));*/
    semWait(EMBARK_MUTEX(margem));
    /*printf("[PASSAGEIRO %d] Na barreira.\n", passageiro.id);*/
    semAddOp(EMBARK_MUTEX(margem), OP_SYNC);
    semExecOps();

}

/* Aqui o passageiro desembarca do barco vindo da margem
 * especificada e realiza quaisquer outras tarefas para dar
 * continuidade a viagem de outros passageiros */
void desembarca(int margem) {
    shm_data *data;
    /*semAddOp(DESEMBARK_MUTEX(!margem), OP_WAIT);*/
    semAddOp(SHM_MUTEX, OP_WAIT);
    semExecOps();
    data = shmGet();
    if (++data->ship_capacity == 3)
        data->ship_current_margin = !margem;
    /*semFinishingSignal(SHM_MUTEX);*/
    passageiro.margem = !margem;
    printf("[PASSAGEIRO %d] Desembarcou na margem %s.\n",
            passageiro.id,
            margens[!margem]);
    semSignal(SHM_MUTEX);
}

/* O barco atravessa o rio a partir da margem
 * especificada. */
void atravessa(int margem) {
    printf("[PASSAGEIRO %d] Atravessando da margem %s para a margem %s.\n",
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

