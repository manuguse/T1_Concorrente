#include "shared.h"

#include <queue.h>


// Você pode declarar novas funções (ou variaveis compartilhadas) aqui

pthread_mutex_t gate_queue_mutex;
sem_t clientes_na_fila;
unsigned int n_clientes_total;
unsigned int n_funcionarios_total;
client_t **clients;

/**********************************
 *          ATENÇÃO               *
 *   NÃO EDITAR ESSAS VARIAVEIS   *
 *          ATENÇÃO               *
 *********************************/
Queue *gate_queue = NULL;