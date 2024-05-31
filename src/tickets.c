/**
 * Esse arquivo tem como objetivo a implementação da bilheteria em um parque.
 * Você pode criar novas funções dentro desse arquivo caso julgue necessário.
 * ATENÇÃO: NÃO APAGUE OU EDITE O NOME OU O(S) ARGUMENTO(S) DAS FUNÇÕES FORNECIDAS.
*/
#include <stdio.h>
#include <stdlib.h>
#include <tickets.h>
#include <unistd.h>

#include <queue.h>
#include "shared.h"

extern Queue *gate_queue; // Fila única de entrada no parque.
extern sem_t sem_bilheteria; // Semaforo para controlar o atendimento na bilheteria
extern int* clientes_em_atendimento; // Vetor que guarda os clientes que estão sendo atendidos
extern int n_atendentes; // Número de atendentes

tickets_args tickets_shared; // Estrutura que guarda informações sobre a bilheteria para a função close()
sem_t sem_mutex_fila_saida; // Semaforo binário para proteger região crítica (saída da fila)

// Thread que implementa uma bilheteria
void *sell(void *args){
    int client_id;
    debug("[INFO] - Bilheteria Abriu!\n");
    ticket_t *funcionario = (ticket_t *) args;

    // Enquanto houver clientes na fila
    while(!is_queue_empty(gate_queue)){
        sem_wait(&sem_mutex_fila_saida); // Entra na região crítica (saída da fila)
        client_id = dequeue(gate_queue); // Tira o cliente da frente da fila
        sem_post(&sem_mutex_fila_saida); // Sai da região crítica (saída da fila)

        // Sinaliza para o cliente que ele pode comprar suas moedas
        clientes_em_atendimento[funcionario->id] = client_id;
        sem_post(&sem_bilheteria);

        debug("[SELL] - Cliente [%d] chamado pelo funcionário [%d]\n", client_id, funcionario->id);
        debug("Fila está vazia? %d\n", is_queue_empty(gate_queue));
    }

    pthread_exit(NULL);
}

// Essa função recebe como argumento informações sobre a bilheteria e deve iniciar os atendentes.
void open_tickets(tickets_args *args){
    // Aloca e inicializa vetor de clientes em atendimento
    clientes_em_atendimento = malloc(args->n * sizeof(int));
    for (int i = 0; i < args->n; i++){
        clientes_em_atendimento[i] = -1;
    }
    n_atendentes = args->n; // Guarda o número de atendentes em uma variável global
    sem_init(&sem_mutex_fila_saida, 0, 1); // Inicializa o semáforo binário para proteger a região crítica (saída da fila)

    tickets_shared = *args;
    for (int i = 0; i < args->n; i++){
        pthread_create(&args->tickets[i]->thread, NULL, sell, (void *) args->tickets[i]);
    }
}

// Essa função deve finalizar a bilheteria
void close_tickets(){
    for (int i = 0; i < tickets_shared.n; i++){
        pthread_join(tickets_shared.tickets[i]->thread, NULL);
    }
    debug("[INFO] - Bilheteria Fechou!\n");
    
}