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

char *PUERTO_ESCUCHA;
char *IP_MEMORIA;
char *PUERTO_MEMORIA;
char *IP_CPU;
char *PUERTO_CPU_DISPATCH;
char *PUERTO_CPU_INTERRUPT;
char *ALGORITMO_PLANIFICACION;
char *QUANTUM;
char **RECURSOS;
char **INSTANCIAS_RECURSOS;
int GRADO_MULTIPROGRAMACION;
char *IP_KERNEL;

t_log *LOGGER_KERNEL;
t_config *CONFIG_KERNEL;

void inicializar_config(void);

#endif