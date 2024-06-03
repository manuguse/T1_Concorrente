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

// VARIÁVEIS GLOBAIS EM TODO O PROGRAMA
extern Queue *gate_queue; // Fila de espera do portão principal
extern int n_clientes; // Número total de clientes
extern int n_funcionarios; // Número de atendentes
extern int *clientes_em_atendimento; // Lista de clientes em atendimento
extern sem_t sem_saida_fila; // Semáforo binário para a saída da fila
extern sem_t clientes_na_fila; // Semáforo binário que indica a chegada de clientes
extern sem_t *sem_bins_funcs; // Lista de semáforos binários que fazem cada funcionario esperar um atendimento antes de começar o próximo

// VARIÁVEIS COMPARTILHADAS NESTE ARQUIVO
int n_clientes_atendidos = 0; // Número de clientes atendidos
ticket_t **tickets_shared; // Estrutura que guarda informações sobre a bilheteria para a função close()
sem_t sem_bin_tickets; // Semáforo binário para proteger região crítica dos funcionários

// Thread que implementa uma bilheteria
void *sell(void *args){
     // debug("[INFO] - Bilheteria Abriu!\n"); // PRINT ORIGINAL
    ticket_t *funcionario = (ticket_t *) args;
    while (n_clientes_atendidos < n_clientes){
        sem_wait(&sem_bins_funcs[funcionario->id - 1]); // Só atende um cliente novo se já terminou de atender o anterior

        sem_wait(&clientes_na_fila); // Espera a chegada de clientes
        sem_wait(&sem_bin_tickets); // Entra na região crítica
        n_clientes_atendidos++; // Incrementa o número de clientes atendidos
        int prox_cliente = dequeue(gate_queue); // Pega o próximo cliente da fila
        sem_post(&sem_bin_tickets); // Sai da região crítica

        if (prox_cliente == -1){
            break; // Se não houver mais clientes, a bilheteria fecha
        }

        clientes_em_atendimento[funcionario->id - 1] = prox_cliente; // Coloca o cliente em atendimento
        sem_post(&sem_saida_fila); // Libera o cliente para ser atendido
        debug("[TICKET] - Bilheteria [%d] atendendo turista [%d].\n", funcionario->id, prox_cliente); // PRINT ORIGINAL
    }
    pthread_exit(NULL);
}

// Essa função recebe como argumento informações sobre a bilheteria e deve iniciar os atendentes.
void open_tickets(tickets_args *args){

    sem_init(&sem_bin_tickets, 0, 1); // Inicializa o semáforo para saída da fila

    // Aloca e inicializa a lista de semáforos binários para os funcionários
    sem_bins_funcs = (sem_t *) malloc(sizeof(sem_t) * args->n); 
    for (int i = 0; i < args->n; i++){
        sem_init(&sem_bins_funcs[i], 0, 1); // Inicializa o semáforo para a saída da fila
    }

    n_funcionarios = args->n; // Guarda o número de atendentes em uma variável global
    
    // Aloca e inicializa a lista de clientes em atendimento
    clientes_em_atendimento = (int *) malloc(sizeof(int) * n_funcionarios); 
    for (int i = 0; i < n_funcionarios; i++){
        clientes_em_atendimento[i] = -1;
    }

    // Guarda os atendentes para a função close() e inicia as threads
    tickets_shared = args->tickets;
    for (int i = 0; i < args->n; i++){
        //debug("[INFO] - Bilheteria %d Abriu!\n", tickets_shared[i]->id); // NÃO ORIGINAL
        pthread_create(&(tickets_shared[i]->thread), NULL, sell, (void *) tickets_shared[i]);
    }
}

// Essa função deve finalizar a bilheteria
void close_tickets(){
    sem_destroy(&sem_bin_tickets); // Destrói o semáforo binário que protege a região crítica dos funcionários

    free (clientes_em_atendimento); // Libera a lista de clientes em atendimento
    free (sem_bins_funcs); // Libera a lista de semáforos binários dos funcionários

    for (int i = 0; i < n_funcionarios; i++){
        pthread_join(tickets_shared[i]->thread, NULL);
        //debug("[INFO] - Bilheteria %d Fechou!\n", tickets_shared[i]->id); //NÃO ORIGINAL
    }
    debug("[INFO] - Todas as Bilheterias Fecharam!\n"); //NÃO ORIGINAL
}