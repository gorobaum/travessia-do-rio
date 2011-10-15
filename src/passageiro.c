
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <semaphore.h>
#include <sys/sem.h>

#include "defs.h"
#include "shmemo.h"
#include "semaf.h"

/* ====================================================== */

/* Número de tentativas necessárias para que o passageiro
 * desista da viagem. */
#define MAX_TIMEOUT 10

/* ====================================================== */

/* Informações sobre este passageiro. */
static struct {
    int     id;
    size_t  countdown;
} passageiro = { 0, MAX_TIMEOUT };

/* Declarado no defs.h */
const char *margins[2] = { "ESQUERDA", "DIREITA" };

/* ====================================================== */

/* Inicializa os recursos para esse processo. */
static void init() {
    semInit();
    /* Para o caso de os semáforos serem destruídos nesse
     * meio tempo, usamos o wait seguro. */
    semSafeWait(SHM_MUTEX);
    shmInit();
    semSignal(SHM_MUTEX);
}

/* Verifica se o número de tentativas máximas foi excedido,
 * caso em que o passageiro desiste da viagem. */
static void checkTime() {
    if (!passageiro.countdown) {
        printf("[PASSAGEIRO %d] Desistiu da viagem.\n", passageiro.id);
        exit(EXIT_SUCCESS);
    }
}

/* Espera o barco chegar nessa margem e tentar embarcar.
 * Desiste se tentar entrar no barco MAX_TIMEOUT vezes.*/
static void tryToEmbark(int margem) {
    for (; passageiro.countdown; passageiro.countdown--) {
        semWait(SHM_MUTEX);
        /* Verifica se o barco está nessa margem e se tem
         * espaço para o passageiro entrar. */
        if (shmCheckShipMargin(margem) && shmGetShipCapacity() > 0) {
            /* Decrementa capacidade do barco e verifica se
             * foi o último. */
            if (shmUpdateShipCapacity(-1) == 0) {
                /* Prepara as barreiras. */
                semAddOp(EMBARK, 3*OP_SIGNAL);
                semAddOp(DESEMBARK, 3*OP_SIGNAL);
                semExecOps();
            }
            semFinishingSignal(SHM_MUTEX);
            break;
        } else {
            /* Tenta de novo. */
            semSignal(SHM_MUTEX);
            sleep(1);
        }
    }
    checkTime();
}

/* Uma vez dentro do barco, espera pelos demais passageiros
 * para que a viagem se inicie. Desiste se tentar partir
 * REMAINING_TIMEOUT vezes, que vale:
 *      MAX_TIMEOUT - #(tentativas feitas para embarcar) */
static void continueOrGiveUp() {
    for (; passageiro.countdown; passageiro.countdown--) {
        semWait(SHM_MUTEX);
        /* Verifica se o barco está cheio e pronto para
         * partir. */
        if (shmGetShipCapacity() == 0) {
            semSignal(SHM_MUTEX);
            break;
        } else if (passageiro.countdown == 1) {
            /* Acabou o número de tentativar. Desiste e
             * libera a vaga para outro. */
            shmUpdateShipCapacity(1);
            semSignal(SHM_MUTEX);
        } else {
            /* Tenta de novo. */
            semSignal(SHM_MUTEX);
            sleep(1);
        }
    }
    checkTime();
}

/* Libera os recursos usados por esse processo.
 * Se nenhum outro processo está declaradamente usando
 * os recursos compartilhados, também libera eles. */
static void cleanUp() {
    shmCleanUp();
}

/* ====================================================== */

/* Aqui o passageiro embarca na margem especificada se
 * possível, ou espera o barco chegar à sua margem do
 * rio. */
void embarca(int margem) {
    printf("[PASSAGEIRO %d] Esperando na margem %s.\n",
            passageiro.id,
            margins[margem]);
    
    /* O passageiro tenta embarcar no braco. */
    tryToEmbark(margem);

    /* Já dentro do barco, o passageiro decide se continua
     * ou desiste da viagem. */
    continueOrGiveUp();

    printf("[PASSAGEIRO %d] Embarcando na margem %s.\n",
           passageiro.id,
           margins[margem]);

    /* O passageiro espera os demais terminarem de embarcar. */
    semWait(EMBARK);
    semSync(EMBARK);
}

/* Aqui o passageiro desembarca do barco vindo da margem
 * especificada e realiza quaisquer outras tarefas para dar
 * continuidade a viagem de outros passageiros */
void desembarca(int margem) {
    semWait(SHM_MUTEX);
    /* Libera a vaga no barco e verifica se foi o último a
     * descer. */
    if (shmUpdateShipCapacity(1) == 3) {
        /* O último a descer atualiza a margem do barco. */
        printf("[BARCO] Agora na margem %s.\n", margins[!margem]);
        shmSetShipMargin(!margem);
    }
    semSignal(SHM_MUTEX);

    /* Espera todos terminarem de descer. */
    semWait(DESEMBARK);
    semSync(DESEMBARK);
}

/* O barco atravessa o rio a partir da margem
 * especificada. */
void atravessa(int margem) {
    printf("[PASSAGEIRO %d] Atravessando da margem %s para a margem %s.\n",
           passageiro.id,
           margins[margem],
           margins[!margem]);
}

int main(int argc, char *argv[]) {
    int margem;

    if ( argc != 2 ) printf("Passar margem. 0 = Esquerda, 1 = Direita. \n");
    else {
        /* Semente para o RNG. */
        srand(time(NULL));

        /* Inicializa recursos para esse processo. */
        init();

        /* Trata a desistência dos passageiros. */
        atexit(cleanUp);

        /* Carrega as informações do passageiro.*/
        passageiro.id = getpid();
        margem = atoi(argv[1]);
        
        /* Tenta fazer a travessia. */
        embarca(margem);
        atravessa(margem);
        desembarca(margem);

        /* imprime passageiro saiu do pier */
        printf("[PASSAGEIRO %d] Desembarcou na margem %s.\n",
               passageiro.id,
               margins[!margem]);
    }

    return EXIT_SUCCESS;
}

