/**
 * Esse arquivo tem como objetivo a implementação de um brinquedo em um parque.
 * Você pode criar novas funções dentro desse arquivo caso julgue necessário.
 * ATENÇÃO: NÃO APAGUE OU EDITE O NOME OU O(S) ARGUMENTO(S) DAS FUNÇÕES
 * FORNECIDAS
 */

#include "toy.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "shared.h"

double RIDETIME = 0.1;

void *timer_function(void *arg) {
  toy_t *toy = (toy_t *)arg;
  sleep(0.2);

  //   pthread_mutex_lock(&toy->timeout_mutex);
  pthread_mutex_lock(&toy->mutex);
  debug(
      "[TIMER] - O brinquedo [%d] está iniciando com [%d] devido ao timeout.\n",
      toy->id, toy->toy_queue);
  sleep(RIDETIME);
  unsigned int toy_queue = toy->toy_queue;
  toy->toy_queue = 0;
  for (int i = 0; i < toy_queue; i++) {
    sem_post(&toy->ride_sem);
  }
  toy->timer_started = FALSE;
  int value;
  sem_getvalue(&toy->queue_sem, &value);
  debug("[RIDE] - O brinquedo [%d] acabou de rodar [%d].\n", toy->id, value);
  pthread_mutex_unlock(&toy->mutex);
  //   pthread_mutex_unlock(&toy->timeout_mutex);
  pthread_exit(NULL);
}

// Thread que o brinquedo vai usar durante toda a simulacao do sistema
void *turn_on(void *args) {
  toy_t *toy = (toy_t *)args;

  debug("[ON] - O brinquedo  [%d] foi ligado.\n",
        toy->id);  // Altere para o id do brinquedo

  while (TRUE) {
    sem_wait(&toy->call_sem);  // Esperando alguém chamar o brinquedo

    pthread_mutex_lock(&clientes_no_parque_mutex);
    if (n_clientes_no_parque == 0) {
      pthread_mutex_unlock(&clientes_no_parque_mutex);
      break;
    }
    pthread_mutex_unlock(&clientes_no_parque_mutex);
    // pthread_t timer_thread;

    pthread_mutex_lock(&toy->mutex);

    if (toy->toy_queue == toy->capacity) {
      debug("[ON] - O brinquedo [%d] está rodando com sua capacidade máxima.\n",
            toy->id);
      sleep(2);
      for (int i = 0; i < toy->toy_queue; i++) {
        sem_post(&toy->queue_sem);  // Avisa X clientes que eles brincaram
      }
      // if (toy->timer_started == TRUE) {
      //   pthread_cancel(timer_thread);
      //   toy->timer_started = FALSE;
      // }
    } else if (!toy->timer_started) {
      // debug("[TIMEOUT] - Brinquedo [%d]: Timer iniciado\n", toy->id);
      // toy->timer_started = TRUE;
      // pthread_create(&timer_thread, NULL, timer_function, toy);
      // pthread_detach(timer_thread);
    }
    pthread_mutex_unlock(&toy->mutex);
  }

  debug("[OFF] - O brinquedo [%d] foi desligado.\n",
        toy->id);  // Altere para o id do brinquedo

  pthread_exit(NULL);
}

// Essa função recebe como argumento informações e deve iniciar os brinquedos.
void open_toys(toy_args *args) {
  for (int i = 0; i < n_brinquedos_total; i++) {
    debug("[INFO] - Brinquedo %d Abriu!\n", toys[i]->id);
    pthread_create(&(toys[i]->thread), NULL, turn_on, (void *)toys[i]);
  }
}

// Desligando os brinquedos
void close_toys() {
  for (int i = 0; i < n_brinquedos_total; i++) {
    pthread_join(toys[i]->thread, NULL);
    debug("[INFO] - Brinquedo %d Fechou!\n", toys[i]->id);
  }
  debug("[INFO] - Todos os Brinquedos Fecharam!\n");
}