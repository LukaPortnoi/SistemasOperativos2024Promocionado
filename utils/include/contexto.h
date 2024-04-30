#ifndef CONTEXTO_H_
#define CONTEXTO_H_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/temporal.h>
#include <assert.h>
#include <pthread.h>

#include "sockets_client.h"
#include "sockets_server.h"
#include "sockets_utils.h"

typedef struct
{
    nombre_instruccion codigo;
    uint32_t pid;
    char *parametro1;
    uint32_t longitud_parametro1;
    char *parametro2;
    uint32_t longitud_parametro2;
} t_instruccion;

typedef struct
{
    uint32_t program_counter;
    uint8_t ax, bx, cx, dx;
    uint32_t eax, ebx, ecx, edx, si, di;
} t_registros;

typedef struct
{
    t_registros *registros;
    t_instruccion *instruccion_ejecutada;
} t_contexto_ejecucion;

void enviar_contexto(int socket, t_contexto_ejecucion *contexto_a_enviar);

void serializar_contexto(t_paquete *paquete, t_contexto_ejecucion *ctx);

t_contexto_ejecucion *recibir_contexto(int socket);

#endif // CONTEXTO_H_