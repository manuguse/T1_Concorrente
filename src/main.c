/*
 * Você pode criar novas funções dentro desse arquivo caso julgue necessário.
 * ATENÇÃO: NÃO APAGUE OU EDITE O NOME OU O(S) ARGUMENTO(S) DAS FUNÇÕES
 * FORNECIDAS
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "client.h"
#include "config.h"
#include "queue.h"
#include "shared.h"
#include "tickets.h"
#include "toy.h"

// Inicia a fila e inicializa mecanismos de sincronização globais
void init_main_queue() {
  pthread_mutex_init(&gate_queue_mutex, NULL);
  pthread_mutex_init(&clientes_no_parque_mutex, NULL);
  sem_init(&clientes_na_fila, 0, 0);
  gate_queue = create_queue();
}

// Destroi a fila e finaliza mecanismos de sincronização globais
void destroy_main_queue() {
  pthread_mutex_destroy(&gate_queue_mutex);
  pthread_mutex_destroy(&clientes_no_parque_mutex);
  sem_destroy(&clientes_na_fila);
  destroy_queue(gate_queue);
}

// Inicia a instância dos clientes
client_t **init_clients(int number, int toy_number, toy_t **toys) {
  n_clientes_total = number;
  n_clientes_no_parque = number;
  clients = malloc(number * sizeof(client_t *));
  for (int i = 0; i < number; i++) {
    clients[i] = (client_t *)malloc(sizeof(client_t));
    clients[i]->id = i + 1;
    clients[i]->coins = 0;
    sem_init(&clients[i]->wait_for_something, 0, 0);
  }
  return clients;
}

// Inicia a instância dos brinquedos
toy_t **init_toys(int number) {
  n_brinquedos_total = number;
  toys = malloc(number * sizeof(toy_t));
  for (int i = 0; i < number; i++) {
    toys[i] = (toy_t *)malloc(sizeof(toy_t));
    toys[i]->id = i + 1;
    toys[i]->capacity = 2;
    // toys[i]->capacity = (rand() % (MAX_CAPACITY_TOY - 1)) + MIN_CAPACITY_TOY;
    pthread_mutex_init(&toys[i]->mutex, NULL);
    sem_init(&toys[i]->queue_sem, 0, 0);
    sem_init(&toys[i]->call_sem, 0, 0);
    toys[i]->timer_started = FALSE;
    toys[i]->p_in_toy_queue = 0;
    toys[i]->ride_time = ((rand() % 200) + 1)/1000.0;
    toys[i]->max_wait_time = ((rand() % 300) + 1)/1000.0;
  }
  return toys;
}

// Inicia a instância dos funcionarios
ticket_t **init_tickets(int number) {
  n_funcionarios_total = number;
  ticket_t **tickets = malloc(number * sizeof(ticket_t));
  for (int i = 0; i < number; i++) {
    tickets[i] = (ticket_t *)malloc(sizeof(ticket_t));
    tickets[i]->id = i + 1;
  }
  return tickets;
}

// Desaloca os clientes
void finish_clients(client_t **clients, int number_clients) { free(clients); }

// Desaloca os brinquedos
void finish_toys(toy_t **toys, int number_toys) { free(toys); }

// Desaloca os funcionarios da bilheteria
void finish_tickets(ticket_t **tickets, int number_clients) { free(tickets); }

/**************************************************
 *                    ATENÇÃO                     *
 *   NÃO EDITAR ESSE ARQUIVO A PARTIR DESTE PONTO *
 *                    ATENÇÃO                     *
 *************************************************/

int main(int argc, char *argv[]) {
  client_args *cli_args = (client_args *)malloc(sizeof(client_args));
  tickets_args *ticket_args = (tickets_args *)malloc(sizeof(tickets_args));
  toy_args *toys_args = (toy_args *)malloc(sizeof(toy_args));

  config_t _config =
      parse(argc, argv);  // ./program -p (pessoas) -t (brinquedos) -g
                          // (bilheterias) -s (semente) -h (ajuda)
  srand(_config.seed);    // Alimentando o gerador pseudo-aleatorio.

  debug("[STARTED] - O parque abriu suas portas.\n");

  // Iniciando a fila.
  init_main_queue();

  // Inicializa os brinquedos.
  toy_t **toys = init_toys(_config.toys);

  // Inicializa os clientes.
  client_t **clients = init_clients(_config.clients, _config.toys, toys);
  cli_args->clients = clients;
  cli_args->n = _config.clients;

  // Inicializa os funcionarios da bilheteria.
  ticket_t **tickets = init_tickets(_config.tickets);
  ticket_args->tickets = tickets;
  ticket_args->n = _config.tickets;

  // Recebe os argumentos para os brinquedos.
  toys_args->toys = toys;
  toys_args->n = _config.toys;

  // Ligando os brinquedos.
  open_toys(toys_args);
  // Os turistas entram no parque.
  open_gate(cli_args);
  // A bilheteria abre.
  open_tickets(ticket_args);

  // Os turistas saem do parque.
  close_gate();

  // A bilheteria fecha.
  close_tickets();

  // Desligam os brinquedos.
  close_toys();

  // Desalocando funcionarios
  finish_tickets(tickets, _config.tickets);

  // Desalocando clientes.
  finish_clients(clients, _config.clients);

  // Desalocando brinquedos.
  // finish_toys(toys, _config.toys);

  /********************************************************************************
   *                                       EXCEÇÃO * SINCRONIZE O FINAL DA
   *EXECUÇÃO DO PROGRAMA PARTIR DESTE PONTO       * EXCEÇÃO *
   *********************************************************************************/

  // Sincronize aqui

  // Desalocando argumentos.
  free(cli_args);
  free(ticket_args);
  free(toys_args);

  // Destruindo filas
  destroy_main_queue();

  debug("[BYE] - O parque fechou suas portas.\n");
  return 0; /* "Houston, Tranquility Base here. The Eagle has landed." - Buzz
               Aldrin (July, 1969) */
}