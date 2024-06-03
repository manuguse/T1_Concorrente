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

pthread_t *client_threads; // Lista de threads dos clientes
int n_clientes; // Número de clientes

// Thread que implementa o fluxo do cliente no parque.
void *enjoy(void *arg){

    //Sua lógica aqui


    debug("[EXIT] - O turista saiu do parque.\n");
    pthread_exit(NULL);
}

// Funcao onde o cliente compra as moedas para usar os brinquedos
void buy_coins(client_t *self){
    // Sua lógica aqui
}

// Função onde o cliente espera a liberacao da bilheteria para adentrar ao parque.
void wait_ticket(client_t *self){
    // Sua lógica aqui
}

// Funcao onde o cliente entra na fila da bilheteria
void queue_enter(client_t *self){
    // Sua lógica aqui.
    debug("[WAITING] - Turista [%d] entrou na fila do portao principal\n", self->id);

    // Sua lógica aqui.
    buy_coins(self);

    // Sua lógica aqui.
    debug("[CASH] - Turista [%d] comprou [%d] moedas.\n", self->id, self->coins);
}

// Essa função recebe como argumento informações sobre o cliente e deve iniciar os clientes.
void open_gate(client_args *args){
    // Sua lógica aqui
    n_clientes = args->n; // Guarda o número de clientes em uma variável global
    client_threads = (pthread_t *) malloc(n_clientes * sizeof(pthread_t)); // Libera espaço para threads dos clientes
    // Inicializa as threads dos clientes
    for (int i = 0; i < n_clientes; i++){
        pthread_create(&client_threads[i], NULL, enjoy, (void *) args->clients[i]); // Inicializa a thread do cliente
    }
}

// Essa função deve finalizar os clientes
void close_gate(){
    // Sua lógica aqui
    for (int i = 0; i < n_clientes; i++){
        pthread_join(client_threads[i], NULL); // Espera a thread do cliente terminar
        free(&client_threads[i]); // Libera a memória da thread do cliente
    }
    //free(client_threads); // Libera a memória da lista de threads dos clientes
    debug("[INFO] - Todos os turistas sairam do parque.\n"); // NÃO ORIGINAL
}
