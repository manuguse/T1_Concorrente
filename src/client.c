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

// VARIÁVEIS GLOBAIS EM TODO O PROGRAMA
extern Queue *gate_queue; // Fila de espera do portão principal
extern int n_clientes; // Número total de clientes
extern int n_funcionarios; // Número de atendentes
extern int *clientes_em_atendimento; // Lista de clientes em atendimento
extern sem_t sem_saida_fila; // Semáforo binário para a saída da fila
extern sem_t clientes_na_fila; // Semáforo binário que indica a chegada de clientes
extern sem_t *sem_bins_funcs; // Lista de semáforos binários que fazem cada funcionario esperar um atendimento antes de começar o próximo

// VARIÁVEIS COMPARTILHADAS NESTE ARQUIVO
sem_t sem_bin_queue; // Semáforo binário para a fila do portão principal
pthread_t *client_threads; // Lista de threads dos clientes

// TEMPORARIO PARA TESTES
sem_t sem_test;

// Funcao onde o cliente compra as moedas para usar os brinquedos
void buy_coins(client_t *self){
    // Sua lógica aqui.
    int coins = rand() % (MAX_COINS - 1) + 1; // Gera um número aleatório de moedas (entre 1 e MAX_COINS)
    self->coins = coins; // Guarda o número de moedas compradas pelo cliente
    debug("[CASH] - Turista [%d] comprou [%d] moedas.\n", self->id, self->coins);
}

// Função onde o cliente espera a liberacao da bilheteria para adentrar ao parque.
void wait_ticket(client_t *self){
    int sera_atendido = 0; // Variável que indica se o cliente será atendido
    while(sera_atendido == 0){ // Enquanto o cliente não for atendido
        sem_wait(&sem_saida_fila); // Espera a liberação da fila
        // Verifica se é a sua vez de ser atendido
        for (int i = 0; i < n_funcionarios; i++){
            if (clientes_em_atendimento[i] == self->id){
                sem_post(&sem_bins_funcs[i]); // Libera o funcionário para atender outro cliente
                sera_atendido = 1; // Se for a sua vez, será atendido
                break;
            }
        }
        sem_post(&sem_saida_fila); // Independente de ser atendido, deixa outro cliente ser atendido

        if (sera_atendido == 0){
            sem_post(&clientes_na_fila); // Avisa que não saiu da fila
        }
    }
}

// Funcao onde o cliente entra na fila da bilheteria
void queue_enter(client_t *self){

    sem_wait(&sem_bin_queue); // Entra na região crítica
    sem_post(&clientes_na_fila); // Avisa que chegou na fila
    enqueue(gate_queue, self->id); // Entra na fila do portão principal
    sem_post(&sem_bin_queue); // Sai da região crítica

    debug("[WAITING] - Turista [%d] entrou na fila do portao principal\n", self->id);
}

// Thread que implementa o fluxo do cliente no parque.
void *enjoy(void *arg){

    //Sua lógica aqui
    client_t *self = (client_t *) arg;

    queue_enter(self); // ENTRA NA FILA

    wait_ticket(self); // ESPERA A LIBERACAO DA BILHETERIA

    buy_coins(self); // COMPRA MOEDAS E ENTRA NO PARQUE

    //debug("[EXIT] - O turista %d saiu do parque.\n", self->id);
    pthread_exit(NULL);
}

// Essa função recebe como argumento informações sobre o cliente e deve iniciar os clientes.
void open_gate(client_args *args){
    // Área de inicialização de mecanismos de sincronização do cliente
    sem_init(&sem_bin_queue, 0, 1); // Inicializa o semáforo binário para a fila do portão principal
    sem_init(&sem_test, 0, 0); // TEMPORARIO PARA TESTES

    n_clientes = args->n; // Guarda o número de clientes em uma variável compartilhada
    client_threads = (pthread_t *) malloc(n_clientes * sizeof(pthread_t)); // Libera espaço para threads dos clientes

    // Inicializa as threads dos clientes
    for (int i = 0; i < n_clientes; i++){
        pthread_create(&client_threads[i], NULL, enjoy, (void *) args->clients[i]); // Inicializa a thread do cliente
    }
}

// Essa função deve finalizar os clientes
void close_gate(){
    sem_destroy(&sem_bin_queue); // Destrói o semáforo binário da fila do portão principal

    for (int i = 0; i < n_clientes; i++){
        pthread_join(client_threads[i], NULL); // Espera a thread do cliente terminar
    }
    free(client_threads); // Libera a memória da lista de threads dos clientes
    debug("[INFO] - Todos os turistas sairam do parque.\n"); // NÃO ORIGINAL
}
