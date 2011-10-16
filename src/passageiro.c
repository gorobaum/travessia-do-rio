
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

/* Um segundo em milissegundos. */
#define ONE_SEC         1000

/* Um milissegundo e nanossegundos. */
#define ONE_MILISSEC    1000000

/* Tempo máximo (em segundos) que o passageiro espera antes
 * de desistir da viagem. */
#define TIMEOUT         10

/* Período de tempo (em milissegundos) que o passageiro
 * espera entre uma tentativa e outra de embarcar ou partir
 * com o barco. */
#ifndef USE_NANO
#define TIME_SLICE      ONE_SEC         /* 1s */
#else
#define TIME_SLICE      (ONE_SEC/10)    /* 0.1ms */
#endif

/* Número máximo de tentativas que o passageiro faz de
 * embarcar e/ou partir com o barco. */
#define MAX_COUNT       (TIMEOUT*ONE_SEC)/TIME_SLICE

/* Macro que deve ser usada para deixar o processo
 * dormindo. */
#ifndef USE_NANO
#define SLEEP(slice)    sleep(slice/1000)
#else
#define SLEEP(slice)    nanosleep(&slice, NULL);
#endif

/* ====================================================== */

/* Informações sobre este passageiro. */
static struct {
    int     id;
    size_t  countdown;
} passageiro = { 0, MAX_COUNT };

/* Declarado no defs.h */
const char *margins[2] = { "ESQUERDA", "DIREITA" };

/* Variável global que representa o período de tempo que
 * o passageiro dorme entre uma tentativa e outra. É usada
 * pela macro SLEEP. */
#ifndef USE_NANO
const static unsigned int slice = TIME_SLICE;
#else
const static struct timespec slice = { 0, ONE_MILISSEC*TIME_SLICE };
#endif

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
        if (shmCheckShipPosition(margem) && shmGetShipCapacity() > 0) {
            /* Decrementa capacidade do barco e verifica se
             * foi o último. */
            if (shmUpdateShipCapacity(-1) == 0) {
                /* Impede que outros passageiros embarquem
                 * enquanto que os atuais não tiverem
                 * desembarcado.*/
                shmSetShipPosition(NAVEGANDO);
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
            SLEEP(slice);
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
            SLEEP(slice);
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
        shmSetShipPosition(!margem);
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

