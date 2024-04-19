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
#include "./comunicaciones.h"

t_log* LOGGER_MEMORIA;
t_config* CONFIG_MEMORIA;
char* PUERTO_ESCUCHA_MEMORIA;
char* IP_MEMORIA;
int TAM_MEMORIA;
int TAM_PAGINA;
char *PATH_INSTRUCCIONES;
int RETARDO_RESPUESTA;
int CLIENTE_CPU, CLIENTE_KERNEL, CLIENTE_IN_OU;

void inicializar_config(void);

#endif