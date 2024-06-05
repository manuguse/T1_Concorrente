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

  debug("[TIMER] - Timer do brinquedo [%d] iniciado por [%d]s.\n", toy->id,
        toy->max_wait_time);
  sleep(toy->max_wait_time);  // espera o tempo máximo, em segundos

  pthread_mutex_lock(&toy->mutex);  // faz um lock para acessar a região crítica
  unsigned int canceled =
      timer->canceled;  // verifica se o timer foi cancelado (chegou na lotação
                        // máxima antes do tempo acabar)
  if (canceled) {
    debug("[TIMER] - Timer do brinquedo [%d] cancelado.\n", toy->id);
    free(timer);  // libera a memória alocada para o timer
    pthread_mutex_unlock(&toy->mutex);  // libera o lock
    pthread_exit(NULL);                 // encerra a thread
  }

  toy->should_play = TRUE;   // o brinquedo pode rodar
  sem_post(&toy->call_sem);  // avisa o brinquedo que o tempo acabou

  toy->timer = NULL;                  // reseta o timer
  free(timer);                        // libera a memória alocada para o timer
  pthread_mutex_unlock(&toy->mutex);  // libera o lock
  pthread_exit(NULL);                 // encerra a thread
}

// Thread que o brinquedo vai usar durante toda a simulacao do sistema
void *turn_on(void *args) {
  toy_t *toy = (toy_t *)args;

  debug("[ON] - O brinquedo  [%d] foi ligado.\n", toy->id);

  while (TRUE) {
    sem_wait(&toy->call_sem);  // espera alguém chamar o brinquedo

    pthread_mutex_lock(
        &toy->mutex);  // cada brinquedo tem seu mutex, sendo que o brinquedo só
                       // pode estar rodando uma vez por vez

    pthread_mutex_lock(&clientes_no_parque_mutex);
    if (n_clientes_no_parque ==
        0) {  // se não tiver mais clientes no parque, encerra o brinquedo
      pthread_mutex_unlock(&toy->mutex);
      pthread_mutex_unlock(&clientes_no_parque_mutex);
      break;
    }
    pthread_mutex_unlock(&clientes_no_parque_mutex);

    // verifica se o brinquedo está cheio ou se o tempo do timer acabou,
    // começando a rodar o brinquedo.
    if (toy->p_in_toy_queue >= toy->capacity || toy->should_play == TRUE) {
      if (toy->timer != NULL) {  // não vai ser null se o timer foi iniciado
        toy->timer->canceled = TRUE;  // cancela o timer
        toy->timer = NULL;            // reseta o timer
      }
      toy->should_play = FALSE;
      // define o tamanho da fila, limitado pela capacidade do brinquedo
      unsigned int queue = toy->p_in_toy_queue < toy->capacity
                               ? toy->p_in_toy_queue
                               : toy->capacity;
      toy->p_in_toy_queue -= queue;  // redefine o tamanho da fila do brinquedo
      if (queue == toy->capacity) {
        debug(
            "[TOY] - O brinquedo [%d] está rodando com sua capacidade máxima "
            "por "
            "[%d]s.\n",
            toy->id, toy->ride_time);
      } else {
        debug(
            "[TOY] - O brinquedo [%d] está rodando após timeout, com [%d/%d] "
            "da capacidade por "
            "[%d]s.\n",
            toy->id, queue, toy->capacity, toy->ride_time);
      }
      sleep(toy->ride_time); // tempo que o brinquedo vai rodar
      debug("[RIDE] - O brinquedo [%d] acabou de rodar. Pessoas na fila: [%d]\n", toy->id,
            toy->p_in_toy_queue);
      for (int i = 0; i < queue; i++) {
        sem_post(&toy->queue_sem);  // avisa [queue] clientes que podem entrar
      }

    } else if (toy->p_in_toy_queue > 0 && toy->timer == NULL) {

      toy_timer_t *timer = (toy_timer_t *)malloc(sizeof(toy_timer_t));
      timer->toy = toy;
      timer->canceled = FALSE;
      toy->timer = timer;
      pthread_create(&toy->timer_thread, NULL, timer_function, timer);

    }
    pthread_mutex_unlock(&toy->mutex);
  }

  debug("[OFF] - O brinquedo [%d] foi desligado.\n",
        toy->id);

  pthread_exit(NULL);
}

// Essa função recebe como argumento informações e deve iniciar os brinquedos.
void open_toys(toy_args *args) {
  // cria as threads dos brinquedos
  for (int i = 0; i < n_brinquedos_total; i++) {
    debug("[INFO] - Brinquedo %d Abriu!\n", toys[i]->id);
    pthread_create(&(toys[i]->thread), NULL, turn_on, (void *)toys[i]);
  }
}

// Desligando os brinquedos
void close_toys() {
  // espera todas as threads dos brinquedos terminarem
  for (int i = 0; i < n_brinquedos_total; i++) {
    pthread_join(toys[i]->thread, NULL);
    debug("[INFO] - Brinquedo %d Fechou!\n", toys[i]->id);
  }
  debug("[INFO] - Todos os Brinquedos Fecharam!\n");
}