#include <semaphore.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>

void semInit(int semkey) {
    sem.id = semget( semkey, 1, IPC_CREAT | S_IRUSR | S_IWUSR );
}

void semCtl( int id, union semun arg ) {
    semctl(id, 0, SETVAL, arg);
}
