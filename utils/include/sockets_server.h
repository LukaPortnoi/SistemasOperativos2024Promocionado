#ifndef SOCKETS_SERVER_H_
#define SOCKETS_SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <string.h>
#include <assert.h>

#include "./sockets_common.h"
#include "./sockets_client.h"

typedef struct
{
    int pid;
    char *path;
} t_proceso_memoria;

extern t_log *logger;

void *recibir_buffer(int *, int);

int iniciar_servidor(t_log *logger, const char *name, char *ip, char *puerto);
int esperar_cliente(int, t_log *logger);
void iterator(char *value);
t_list *recibir_paquete(int);
t_paquete *recibir_paqueteTOP(int socket_cliente);
void recibir_mensaje(int, t_log *logger);
int recibir_operacion(int);

#endif