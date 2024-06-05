#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <defs.h>

// Você pode declarar novas funções aqui

void *enjoy(void *arg);
void enter_park(client_t *self);
void queue_enter(client_t *self);
void wait_ticket(client_t *self);
void buy_coins(client_t *self);
void play(client_t *self);

/********************************
 *          ATENÇÃO             *
 *   NÃO EDITAR ESSAS FUNÇÕES   *
 *          ATENÇÃO             *
 *******************************/
void open_gate(client_args *args);
void close_gate();

#endif