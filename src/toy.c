/**
 * Esse arquivo tem como objetivo a implementação de um brinquedo em um parque.
 * Você pode criar novas funções dentro desse arquivo caso julgue necessário.
 * ATENÇÃO: NÃO APAGUE OU EDITE O NOME OU O(S) ARGUMENTO(S) DAS FUNÇÕES FORNECIDAS
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "toy.h"
#include "shared.h"

int n_brinquedos; // Número de brinquedos
toy_t **toys_shared; // Estrutura que guarda informações sobre o brinquedo para a função close()

// Thread que o brinquedo vai usar durante toda a simulacao do sistema
void *turn_on(void *args){

    toy_t *toy = (toy_t *) args;

    debug("[ON] - O brinquedo  [%d] foi ligado.\n", toy->id); // Altere para o id do brinquedo

    debug("[OFF] - O brinquedo [%d] foi desligado.\n", toy->id); // Altere para o id do brinquedo

    pthread_exit(NULL);
}


// Essa função recebe como argumento informações e deve iniciar os brinquedos.
void open_toys(toy_args *args){
    n_brinquedos = args->n; // Guarda o número de atendentes em uma variável global
    toys_shared = args->toys;
    for (int i = 0; i < args->n; i++){
        debug("[INFO] - Brinquedo %d Abriu!\n", toys_shared[i]->id);
        pthread_create(&(toys_shared[i]->thread), NULL, turn_on, (void *) toys_shared[i]);
    }
}

// Desligando os brinquedos
void close_toys(){
    for (int i = 0; i < n_brinquedos; i++){
        pthread_join(toys_shared[i]->thread, NULL);
        debug("[INFO] - Brinquedo %d Fechou!\n", toys_shared[i]->id);
    }
    debug("[INFO] - Todos os Brinquedos Fecharam!\n");
}