#ifndef __DEFS_H__
#define __DEFS_H__

/* Essa biblioteca implementa definicoes que sao usadas pelo programa. */
/* ATENÇÃO: NÃO APAGUE OU EDITE O NOMES FORNECIDOS */

#include <pthread.h>
#include <semaphore.h>

#define TRUE \
  1  // Em C nao temos True ou False (como no Python). Usamos macro TRUE para
     // True (como no Python).
#define FALSE 0  // e FALSE para False (como no Python).

#define MAX_CAPACITY_TOY 10  // Capacidade maxima dos brinquedos.
#define MIN_CAPACITY_TOY 5   // Capacidade minima dos brinquedos.
#define MAX_COINS 20
// #define MAX_COINS 20         // Maximo de moedas que um cliente pode comprar

#define DEBUG \
  1  //  Alterne (0 or 1) essa macro se voce espera desabilitar todas as
     //  mensagens de debug.

/* Adicione as estruturas de sincronização que achar necessárias */
typedef struct ticket {
  pthread_t thread;  // A thread do funcionário que fica na bilheteria.
  int id;            // O id do funcionario que fica na bilheteria.
} ticket_t;

/* Adicione as estruturas de sincronização que achar necessárias */

struct toy_timer;
typedef struct toy {
  int id;                  // O id de um brinquedo.
  int capacity;            // A capacidade total de um brinquedo.
  pthread_t thread;        // A thread de um brinquedo.
  pthread_t timer_thread;  // Thread do timer do brinquedo.
  struct toy_timer *timer;
  pthread_mutex_t mutex;  // Mutex para proteger a região crítica.
  sem_t queue_sem;        // Semáforo para controlar a fila do brinquedo.
  sem_t call_sem;         // Semáforo para avisar o brinquedo que alguém entrou nele.
  unsigned int
      p_in_toy_queue;     // Número de pessoas esperando para entrar no brinquedo.
  unsigned int should_play;
  unsigned int ride_time;
  unsigned int max_wait_time;
} toy_t;

typedef struct toy_timer {
  toy_t *toy;
  unsigned int canceled;
} toy_timer_t;

/* Adicione as estruturas de sincronização que achar necessárias */
typedef struct client {
  pthread_t thread;          // A thread do cliente.
  int id;                    // O id do cliente.
  int coins;                 // Quantidade de moedas do cliente.
  sem_t wait_for_something;  // Semáforo para esperar algo.
} client_t;

/* Adicione as estruturas de sincronização que achar necessárias */
typedef struct {
  ticket_t **tickets;  // Array de funcionarios.
  int n;               // Quantidade de funcionários da bilheteria.
} tickets_args;

/* Adicione as estruturas de sincronização que achar necessárias */
typedef struct {
  toy_t **toys;  // Array de brinquedos.
  int n;         // Quantidade de brinquedos.
} toy_args;

/* Adicione as estruturas de sincronização que achar necessárias */
typedef struct {
  client_t **clients;  // Array de clientes.
  int n;               // Quantidade de clientes.
} client_args;

/**************************************************
 *                    ATENÇÃO                     *
 *   NÃO EDITAR ESSE ARQUIVO A PARTIR DESTE PONTO *
 *                    ATENÇÃO                     *
 *************************************************/

// Estrutura da fila
typedef struct node {
  int data;           // Dado da fila.
  struct node *next;  // Objeto do proximo item da fila.
} Node;

// Fila
typedef struct queue {
  Node *front;  // Primeiro elemento da fila.
  Node *rear;   // Ultimo elemento da fila.
} Queue;

#if DEBUG
#define debug(...) printf(__VA_ARGS__);
#else
#define debug(...) ;
#endif

#endif