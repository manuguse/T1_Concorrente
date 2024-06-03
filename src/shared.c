#include "shared.h"

#include <queue.h>


// Você pode declarar novas funções (ou variaveis compartilhadas) aqui
int n_clientes; // Número total de clientes
int n_funcionarios; // Número de atendentes
int *clientes_em_atendimento; // Lista de clientes em atendimento
sem_t sem_saida_fila; // Semáforo binário para a saída da fila
sem_t clientes_na_fila; // Semáforo binário que indica a chegada de clientes

/**********************************
 *          ATENÇÃO               *
 *   NÃO EDITAR ESSAS VARIAVEIS   *
 *          ATENÇÃO               *
 *********************************/
Queue *gate_queue = NULL;