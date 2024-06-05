/**
 * Esse arquivo tem como objetivo a implementação do cliente em um parque.
 * Você pode criar novas funções dentro desse arquivo caso julgue necessário.
 * ATENÇÃO: NÃO APAGUE OU EDITE O NOME OU O(S) ARGUMENTO(S) DAS FUNÇÕES
 * FORNECIDAS
 */

#include "client.h"

#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "queue.h"
#include "shared.h"

// Thread que implementa o fluxo do cliente no parque.
void *enjoy(void *arg) {
  client_t *client = (client_t *)arg;
  enter_park(client);  // entra no parque
  play(client);        // brinca

  pthread_mutex_lock(&clientes_no_parque_mutex);
  if (--n_clientes_no_parque == 0) {
    for (int i = 0; i < n_brinquedos_total; i++) {
      sem_post(&toys[i]->call_sem);
    }
  }
  pthread_mutex_unlock(&clientes_no_parque_mutex);
  debug("[EXIT] - O turista [%d] saiu do parque.\n", client->id);
  pthread_exit(NULL);
}

void enter_park(client_t *self) {
  queue_enter(self);
  wait_ticket(self);
  buy_coins(self);
}

// Funcao onde o cliente entra na fila da bilheteria
void queue_enter(client_t *self) {
  pthread_mutex_lock(&gate_queue_mutex);
  enqueue(gate_queue, self->id);
  debug("[WAITING] - Turista [%d] entrou na fila do portao principal\n",
        self->id);
  sem_post(&clientes_na_fila);  // avisa a bilheteria que entrou cliente na fila
  pthread_mutex_unlock(&gate_queue_mutex);
}

// Função onde o cliente espera a liberacao da bilheteria para adentrar ao
// parque.
void wait_ticket(client_t *self) {
  sem_wait(&self->wait_for_something);  // espera a bilheteria atender esse
  // cliente (post)
}

// Funcao onde o cliente compra as moedas para usar os brinquedos
void buy_coins(client_t *self) {
  self->coins = rand() % MAX_COINS + 1;
  debug("[CASH] - Turista [%d] comprou [%d] moedas.\n", self->id, self->coins);
}

void play(client_t *self) {
  while (TRUE) {
    toy_t *toy = toys[rand() % n_brinquedos_total];
    debug("[RIDE] - Turista [%d] na fila do brinquedo [%d].\n", self->id,
          toy->id);
          int value;
          sem_getvalue(&toy->queue_sem, &value);
          debug("[RIDE] - Turista [%d] antes de entrar na fila [%d], [%d].\n", self->id,
          toy->id, value);
    // TODO arrumar isso
    sem_wait(&toy->queue_sem);  // Entrou na fila do brinquedo

    debug("[RIDE] - Turista [%d] conseguiu entrar na fila [%d].\n", self->id,
          toy->id);
    pthread_mutex_lock(&toy->mutex);
    toy->in_ride++;
    pthread_mutex_unlock(&toy->mutex);
    sem_post(&toy->call_sem);   // Avisa que entrou no brinquedo
    sem_wait(&toy->ride_sem);   // Espera terminar de brincar no brinquedo
    sem_post(&toy->queue_sem);  // Libera a vaga na fila
    debug("[RIDE] - Turista [%d] brincou no brinquedo [%d]. Moedas: [%d]\n",
          self->id, toy->id, self->coins - 1);
    if (--self->coins == 0) {
      break;
    }
  }
}

// Essa função recebe como argumento informações sobre o cliente e deve iniciar
// os clientes.
void open_gate(client_args *args) {
  // Guarda os atendentes para a função close() e inicia as threads
  clients = args->clients;
  for (int i = 0; i < n_clientes_total; i++) {
    pthread_create(&(clients[i]->thread), NULL, enjoy, (void *)clients[i]);
  }
}

// Essa função deve finalizar os clientes
void close_gate() {
  for (int i = 0; i < n_clientes_total; i++) {
    pthread_join(clients[i]->thread, NULL);
  }
  debug("[INFO] - Todos os clientes sairam\n");  // NÃO ORIGINAL
}