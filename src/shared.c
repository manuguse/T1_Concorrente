#include "shared.h"

#include <queue.h>


// Você pode declarar novas funções (ou variaveis compartilhadas) aqui

sem_t sem_mutex_queue;
sem_t sem_gate;

/**********************************
 *          ATENÇÃO               *
 *   NÃO EDITAR ESSAS VARIAVEIS   *
 *          ATENÇÃO               *
 *********************************/
Queue *gate_queue = NULL;