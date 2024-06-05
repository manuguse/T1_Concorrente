#include "shared.h"

#include <queue.h>


// Você pode declarar novas funções (ou variaveis compartilhadas) aqui

pthread_mutex_t gate_queue_mutex;
pthread_mutex_t clientes_no_parque_mutex;
sem_t clientes_na_fila;
unsigned int n_clientes_total;
unsigned int n_clientes_no_parque;
unsigned int n_funcionarios_total;
unsigned int n_brinquedos_total;
client_t **clients;
toy_t **toys;

/**********************************
 *          ATENÇÃO               *
 *   NÃO EDITAR ESSAS VARIAVEIS   *
 *          ATENÇÃO               *
 *********************************/
Queue *gate_queue = NULL;