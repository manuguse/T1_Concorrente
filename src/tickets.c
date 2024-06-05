/**
 * Esse arquivo tem como objetivo a implementação da bilheteria em um parque.
 * Você pode criar novas funções dentro desse arquivo caso julgue necessário.
 * ATENÇÃO: NÃO APAGUE OU EDITE O NOME OU O(S) ARGUMENTO(S) DAS FUNÇÕES
 * FORNECIDAS.
 */
#include <queue.h>
#include <stdio.h>
#include <stdlib.h>
#include <tickets.h>
#include <unistd.h>

#include "shared.h"

// VARIÁVEIS COMPARTILHADAS NESTE ARQUIVO
ticket_t **tickets_shared;  // Estrutura que guarda informações sobre a
                            // bilheteria para a função close()
unsigned int n_clientes_atentidos = 0;     // Contador de clientes atendidos
pthread_mutex_t clientes_atendidos_mutex;  // Mutex para o contador de clientes
                                           // atendidos

// Thread que implementa uma bilheteria
void *sell(void *args) {
  ticket_t *funcionario = (ticket_t *)args;

  debug("[INFO] - Bilheteria [%d] Abriu!\n",
        funcionario->id);  // PRINT ORIGINAL

  while (TRUE) {
    sem_wait(&clientes_na_fila);  // espera ter clientes na fila
    if (n_clientes_atentidos == n_clientes_total) {
      break;  // evita que o atendente espere para sempre quando há menos
              // clientes do que atendentes faltando
    }

    pthread_mutex_lock(&gate_queue_mutex);  // trava a fila para dar dequeue
    client_t *cliente = clients[dequeue(gate_queue) - 1];
    pthread_mutex_unlock(&gate_queue_mutex);

    debug("[INFO] - Bilheteria [%d] atendeu o turista [%d]\n", funcionario->id,
          cliente->id);
    sem_post(&cliente->wait_for_something);  // Atende o cliente

    pthread_mutex_lock(&clientes_atendidos_mutex);
    n_clientes_atentidos++;
    if (n_clientes_atentidos == n_clientes_total) {
      pthread_mutex_unlock(&clientes_atendidos_mutex);
      for (int i = 0; i < n_funcionarios_total; i++) {
        sem_post(&clientes_na_fila);  // libera todos os atendentes
      }
      break;
    }
    pthread_mutex_unlock(&clientes_atendidos_mutex);
  }

  pthread_exit(NULL);
}

// Essa função recebe como argumento informações sobre a bilheteria e deve
// iniciar os atendentes.
void open_tickets(tickets_args *args) {
  pthread_mutex_init(&clientes_atendidos_mutex,
                     NULL);  // Inicia o mutex para o contador de clientes
  // Guarda os atendentes para a função close() e inicia as threads
  tickets_shared = args->tickets;
  for (int i = 0; i < n_funcionarios_total; i++) {
    pthread_create(&(tickets_shared[i]->thread), NULL, sell,
                   (void *)tickets_shared[i]);
  }
}

// Essa função deve finalizar a bilheteria
void close_tickets() {
  pthread_mutex_destroy(&clientes_atendidos_mutex);  // Destrói o mutex da fila

  for (int i = 0; i < n_funcionarios_total; i++) {
    pthread_join(tickets_shared[i]->thread, NULL);
    // debug("[INFO] - Bilheteria %d Fechou!\n", tickets_shared[i]->id); //NÃO
    // ORIGINAL
  }
  debug("[INFO] - Todas as Bilheterias Fecharam!\n");  // NÃO ORIGINAL
}