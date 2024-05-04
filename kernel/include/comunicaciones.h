#ifndef COMUNICACION_H
#define COMUNICACION_H

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
#include "./gestor.h"

typedef struct
{
	t_log *log;
	int fd;
	char *server_name;
} t_procesar_conexion_args;

void procesar_conexion_kernel(void *void_args);
int server_escuchar(t_log *logger, char *server_name, int server_socket);

#endif
