/**
 * Esse arquivo tem como objetivo a implementação do cliente em um parque.
 * Você pode criar novas funções dentro desse arquivo caso julgue necessário.
 * ATENÇÃO: NÃO APAGUE OU EDITE O NOME OU O(S) ARGUMENTO(S) DAS FUNÇÕES FORNECIDAS
*/

#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "client.h"
#include "queue.h"
#include "shared.h"

client_args *clients = NULL;

extern sem_t sem_mutex_queue;
extern sem_t sem_gate;

// Thread que implementa o fluxo do cliente no parque.
void *enjoy(void *arg){

    client_t *self = (client_t *) arg;
    debug("[ENTER] - O turista [%d] está curtindo o parque.\n", self->id);
    while (self->coins > 0){
        
        // escolhe um brinquedo aleatório
        toy_t *toy = self->toys[rand() % self->number_toys];

        // gasta uma moeda para brincar
        self->coins--;

        // espera a liberação do brinquedo

    }

    debug("[EXIT] - O turista saiu do parque.\n");
    pthread_exit(NULL);
}

// Funcao onde o cliente compra as moedas para usar os brinquedos
void buy_coins(client_t *self){
    // todas as moedas são compradas de uma vez na entrada
    self->coins = rand() % MAX_COINS + 1; // o +1 é para garantir que o cliente compre pelo menos uma moeda
                                          // se rand for max-1, ele compra max-1+1 = max moedas
    debug("[CASH] - Turista [%d] comprou [%d] moedas.\n", self->id, self->coins)
}

// Função onde o cliente espera a liberacao da bilheteria para adentrar ao parque.
void wait_ticket(client_t *self){
    sem_wait(); // TODO - semaforo para esperar a liberacao da bilheteria
}

// Funcao onde o cliente entra na fila da bilheteria
void queue_enter(client_t *self){

    sem_wait(&sem_mutex_queue);  // faz o lock da fila
    enqueue(gate_queue, self);   // adiciona o cliente na fila
    sem_post(&sem_mutex_queue);  // faz o unlock da fila

    debug("[WAITING] - Turista [%d] entrou na fila do portao principal\n", self->id);

    sem_post(&sem_gate);  // avisa a bilheteria que alguem entrou na fila
    wait_ticket(self);    // espera a liberacao da bilheteria
    
    buy_coins(self);
    debug("[CASH] - Turista [%d] comprou [%d] moedas.\n", self->id, self->coins);
}

// Essa função recebe como argumento informações sobre o cliente e deve iniciar os clientes.
void open_gate(client_args *args){
    clients = args->clients;
    // TODO 
}

// Essa função deve finalizar os clientes
void close_gate(){
   //Sua lógica aqui
   for (int i = 0; i < clients->n; i++){
        pthread_join(clients->clients[i]->thread, NULL);
    }
    free(clients->clients);
    free(clients);
}
