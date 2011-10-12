
#ifndef SHMEMO_H_
#define SHMEMO_H_

/* OBS: essas funções exigem regiões críticas. */

/* Inicializa a memória compartilhada, verificando se ela
 * já não foi criada. Se já foi, recupera ela. Se não,
 * cria ela. */
void shmInit();

char* getMemo(int memkey, int size);

void shmDetach();

void shmRemove();

#endif
