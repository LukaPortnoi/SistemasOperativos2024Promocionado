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
#include "../../utils/include/contexto.h"
#include "../include/gestor.h"

t_proceso_memoria *proceso_memoria;

int server_escuchar(t_log *logger, char *server_name, int server_socket);
t_proceso_memoria *recibir_proceso_memoria(int socket);
void extraer_de_paquete(t_paquete *paquete, void *destino, int size);


#endif