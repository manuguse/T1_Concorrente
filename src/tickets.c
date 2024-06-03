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

int n_funcionarios; // Número de atendentes
ticket_t **tickets_shared; // Estrutura que guarda informações sobre a bilheteria para a função close()

// Thread que implementa uma bilheteria
void *sell(void *args){

     // debug("[INFO] - Bilheteria Abriu!\n"); // PRINT ORIGINAL

    pthread_exit(NULL);
}

// Essa função recebe como argumento informações sobre a bilheteria e deve iniciar os atendentes.
void open_tickets(tickets_args *args){
n_funcionarios = args->n; // Guarda o número de atendentes em uma variável global
    tickets_shared = args->tickets;
    for (int i = 0; i < args->n; i++){
        debug("[INFO] - Bilheteria %d Abriu!\n", tickets_shared[i]->id); // NÃO ORIGINAL
        pthread_create(&(tickets_shared[i]->thread), NULL, sell, (void *) tickets_shared[i]);
    }
}

// Essa função deve finalizar a bilheteria
void close_tickets(){
    for (int i = 0; i < n_funcionarios; i++){
        pthread_join(tickets_shared[i]->thread, NULL);
        debug("[INFO] - Bilheteria %d Fechou!\n", tickets_shared[i]->id); //NÃO ORIGINAL
    }
    debug("[INFO] - Todas as Bilheterias Fecharam!\n"); //NÃO ORIGINAL
}