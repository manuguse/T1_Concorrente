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

extern Queue *gate_queue; //Fila única de entrada no parque.
extern sem_t sem_bilheteria; // Semaforo para controlar o atendimento na bilheteria
extern int* clientes_em_atendimento; // Vetor que guarda os clientes que estão sendo atendidos
extern int n_atendentes; // Número de atendentes

pthread_t *client_threads; //Threads dos clientes guardadas para close()
int n_clients; //Número de clientes guardado para close()
sem_t sem_mutex_fila; //Semaforo binário para proteger região crítica (entrada da fila)

// Funcao onde o cliente compra as moedas para usar os brinquedos
void buy_coins(client_t *self){
    // Sua lógica aqui
    debug("[CASH] - Turista [%d] comprou [%d] moedas.\n", self->id, self->coins);
}

// Função onde o cliente espera a liberacao da bilheteria para adentrar ao parque.
void wait_ticket(client_t *self){
    int success = 0;
    while (!success){
        sem_wait(&sem_bilheteria); // Espera a bilheteria para ser liberado
        for(int i = 0; i < n_atendentes; i++){
            if(clientes_em_atendimento[i] == self->id){
                success = 1;
                break;
            }
        }
    }
}

// Funcao onde o cliente entra na fila da bilheteria
void queue_enter(client_t *self){
    sem_wait(&sem_mutex_fila); // Entra na região crítica (entrada da fila)
    enqueue(gate_queue, self->id); // Cliente efetivamente entra na fila do portão principal.
    sem_post(&sem_mutex_fila); // Sai da região crítica (entrada da fila)
    debug("[WAITING] - Turista [%d] entrou na fila do portao principal\n", self->id);
}

// Thread que implementa o fluxo do cliente no parque.
void *enjoy(void *arg){
    client_t *self = (client_t *) arg;
    
    // NESSA IMPLEMENTAÇÃO, O CLIENTE ENTRA NA FILA ANTES DA BILHETERIA ESTAR PRONTA, MAS SÓ SAI DA FILA DEPOIS QUE A BILHETERIA ESTIVER PRONTA.

    queue_enter(self); //Função para entrar na fila única de espera

    wait_ticket(self); // Aqui vem um mecanismo de sincronização que faz o cliente esperar um funcionário chamar ele para comprar suas moedas.

    buy_coins(self); // Função para cliente comprar moedas

    debug("[EXIT] - O turista saiu do parque.\n");
    pthread_exit(NULL);
}

// Essa função recebe como argumento informações sobre o cliente e deve iniciar os clientes.
void open_gate(client_args *args){
    sem_init(&sem_mutex_fila, 0, 1); // Inicializa o semáforo binário para proteger a região crítica (entrada da fila

    n_clients = args->n; // Guarda a quantidade de clientes.
    client_threads = (pthread_t *) malloc(n_clients * sizeof(pthread_t)); // Abre espaço para guardar as threads dos clientes.
    // Inicializa as threads dos clientes.
    for (int i = 0; i < args->n; i++){
        pthread_create(&client_threads[i], NULL, enjoy, (void *) args->clients[i]);
    }
}

// Essa função deve finalizar os clientes
void close_gate(){
   // Finaliza as thredas dos clientes
   for (int i = 0; i < n_clients; i++){
        pthread_join(client_threads[i], NULL);
    }
    free(client_threads); // Libera a memória alocada para as threads dos clientes.
    debug("[EXIT] - Todos os turistas sairam do parque.\n");
}
