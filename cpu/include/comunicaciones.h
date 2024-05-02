#ifndef COMUNICACIONES_H_
#define COMUNICACIONES_H_

#include <stdlib.h>
#include <stdio.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <readline/readline.h>

#include "../../utils/include/sockets_client.h"
#include "../../utils/include/sockets_server.h"

static void procesar_conexion_dispatch(void *void_args);
static void procesar_conexion_interrupt(void *void_args);
void *recibir_interrupciones(void);
void finalizar_conexiones_cpu(void);

int server_escuchar(t_log *logger, char *server_name, int server_socket);

#endif