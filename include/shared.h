#ifndef __SHARED_H__
#define __SHARED_H__

#include <defs.h>

// Você pode declarar novas funções (ou variaveis compartilhadas) aqui


extern pthread_mutex_t gate_queue_mutex;
extern pthread_mutex_t clientes_no_parque_mutex;
extern sem_t clientes_na_fila;
extern unsigned int n_clientes_total;
extern unsigned int n_clientes_no_parque;
extern unsigned int n_funcionarios_total;
extern unsigned int n_brinquedos_total;
extern client_t **clients;
extern toy_t **toys;

/**********************************
 *          ATENÇÃO               *
 *   NÃO EDITAR ESSAS VARIAVEIS   *
 *          ATENÇÃO               *
 *********************************/
extern Queue *gate_queue;

#endif