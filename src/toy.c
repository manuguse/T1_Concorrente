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

void *timer_function(void *arg) {
  toy_t *toy = (toy_t *)arg;
  sleep(toy->max_wait_time);

  pthread_mutex_lock(&toy->mutex);
  if (toy->p_in_toy_queue >= toy->capacity || toy->p_in_toy_queue == 0) {
    debug("ZZZZZ brinquedo [%d]\n", toy->id);
    pthread_mutex_unlock(&toy->mutex);
    pthread_exit(NULL);
  }
  debug("Sleeping brinquedo [%d]\n", toy->id);
  debug("AAAAA [%d]\n", toy->id);
  debug(
      "[TOY] - O brinquedo [%d] está rodando sem lotação total [%d/%d] por "
      "[%.2f]s.\n",
      toy->id, toy->p_in_toy_queue, toy->capacity, toy->ride_time);
  pthread_mutex_unlock(&toy->mutex);
  sleep(toy->ride_time);
  pthread_mutex_lock(&toy->mutex);
  debug("Wake me up inside brinquedo [%d]\n", toy->id);
  toy->timer_started = FALSE;
  unsigned int queue = toy->p_in_toy_queue;
  toy->p_in_toy_queue = 0;
  debug("[RIDE] - O brinquedo [%d] acabou de rodar. Fila: [%d]\n", toy->id,
        toy->p_in_toy_queue);
  for (int i = 0; i < queue; i++) {
    sem_post(&toy->queue_sem);  // Avisa X clientes que eles brincaram
  }
  pthread_mutex_unlock(&toy->mutex);
  pthread_exit(NULL);
}

// Thread que o brinquedo vai usar durante toda a simulacao do sistema
void *turn_on(void *args) {
  toy_t *toy = (toy_t *)args;

  debug("[ON] - O brinquedo  [%d] foi ligado.\n",
        toy->id);  // Altere para o id do brinquedo

  while (TRUE) {
    sem_wait(&toy->call_sem);  // Esperando alguém chamar o brinquedo

    pthread_mutex_lock(&toy->mutex);

    pthread_mutex_lock(&clientes_no_parque_mutex);
    if (n_clientes_no_parque == 0) {
      pthread_mutex_unlock(&toy->mutex);
      pthread_mutex_unlock(&clientes_no_parque_mutex);
      break;
    }
    pthread_mutex_unlock(&clientes_no_parque_mutex);

    if (toy->p_in_toy_queue >= toy->capacity) {
      if (toy->timer_started == TRUE) {
        debug("[RIDE] - Matando o timer do brinquedo [%d]\n", toy->id);
        pthread_cancel(toy->timer_thread);
        toy->timer_started = FALSE;
      }
      debug(
          "[TOY] - O brinquedo [%d] está rodando com sua capacidade máxima por "
          "[%.2f]s.\n",
          toy->id, toy->ride_time);
      sleep(toy->ride_time);
      unsigned int queue = toy->capacity;
      toy->p_in_toy_queue -= toy->capacity;
      debug("[RIDE] - O brinquedo [%d] acabou de rodar. Fila: [%d]\n", toy->id,
            toy->p_in_toy_queue);
      for (int i = 0; i < queue; i++) {
        sem_post(&toy->queue_sem);  // Avisa X clientes que eles brincaram
      }

    } else if (toy->p_in_toy_queue > 0 && toy->timer_started == FALSE) {
      debug("[TIMEOUT] - Brinquedo [%d]: Timer iniciado por [%.2f]s\n", toy->id,
            toy->max_wait_time);
      toy->timer_started = TRUE;
      pthread_create(&toy->timer_thread, NULL, timer_function, toy);
      pthread_detach(toy->timer_thread);
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