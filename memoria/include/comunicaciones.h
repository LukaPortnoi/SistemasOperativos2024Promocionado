#ifndef COMUNICACIONES_H_
#define COMUNICACIONES_H_

#include <stdlib.h>
#include <stdio.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <readline/readline.h>

#include "../../utils/include/sockets_client.h"
#include "../../utils/include/sockets_server.h"
#include "../../utils/include/contexto.h"
#include "../include/gestor.h"
#include "./utils_memoria.h"

typedef struct
{
    t_log *log;
    int fd;
    char *server_name;
} t_procesar_conexion_args;

int server_escuchar(t_log *logger, char *server_name, int server_socket);

#endif