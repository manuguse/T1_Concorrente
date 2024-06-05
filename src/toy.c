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
  toy_timer_t *timer = (toy_timer_t *)arg;
  toy_t *toy = timer->toy;

  sleep(toy->max_wait_time); // Espera o tempo máximo, em segundos

  pthread_mutex_lock(&toy->mutex);
  unsigned int canceled = timer->canceled;
  if (canceled) {
    free(timer);
    pthread_mutex_unlock(&toy->mutex);
    pthread_exit(NULL);
  }

  // if (toy->p_in_toy_queue > 0) {
  toy->should_play = TRUE;
  sem_post(&toy->call_sem);
  // }
  toy->timer = NULL;
  free(timer);
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

    if (toy->p_in_toy_queue >= toy->capacity || toy->should_play == TRUE) {
      if (toy->timer != NULL) {
        toy->timer->canceled = TRUE;
        toy->timer = NULL;
      }
      toy->should_play = FALSE;
      unsigned int queue = toy->p_in_toy_queue < toy->capacity
                               ? toy->p_in_toy_queue
                               : toy->capacity;
      toy->p_in_toy_queue -= queue;
      if (queue == toy->capacity) {
        debug(
            "[TOY] - O brinquedo [%d] está rodando com sua capacidade máxima "
            "por "
            "[%.2f]s.\n",
            toy->id, (toy->ride_time/1e6));
      } else {
        debug(
            "[TOY] - O brinquedo [%d] está rodando após timeout [%d/%d] por "
            "[%.2f]s.\n",
            toy->id, queue, toy->capacity, (toy->ride_time/1e6));
      }
      usleep(toy->ride_time);
      debug("[RIDE] - O brinquedo [%d] acabou de rodar. Fila: [%d]\n", toy->id,
            toy->p_in_toy_queue);
      for (int i = 0; i < queue; i++) {
        sem_post(&toy->queue_sem);  // Avisa X clientes que eles brincaram
      }

    } else if (toy->p_in_toy_queue > 0 && toy->timer == NULL) {
      debug("[TIMEOUT] - Brinquedo [%d]: Timer iniciado por [%.2f]s\n", toy->id,
            (toy->max_wait_time/1e6));
      toy_timer_t *timer = (toy_timer_t *)malloc(sizeof(toy_timer_t));
      timer->toy = toy;
      timer->canceled = FALSE;
      toy->timer = timer;
      pthread_create(&toy->timer_thread, NULL, timer_function, timer);
      // pthread_detach(toy->timer_thread);
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