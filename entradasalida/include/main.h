#ifndef MAIN_H_
#define MAIN_H_

#include <stdlib.h>
#include <stdio.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>

#include "../../utils/include/hello.h"
#include "../../utils/include/sockets_server.h"
#include "../../utils/include/sockets_client.h"
#include "../../utils/include/sockets_utils.h"

char *TIPO_INTERFAZ;
int TIEMPO_UNIDAD_TRABAJO;
char *IP_KERNEL;
char *PUERTO_KERNEL;
char *IP_MEMORIA;
char *PUERTO_MEMORIA;
char *PATH_BASE_DIALFS;
int BLOCK_SIZE;
int BLOCK_COUNT;
t_log *LOGGER_INPUT_OUTPUT;
t_config *CONFIG_INPUT_OUTPUT;

void inicializar_config(void);
void enviar_con_handshake(int socket_cliente, char* mensaje);

#endif