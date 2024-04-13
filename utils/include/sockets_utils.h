#ifndef SOCKETS_UTILS_H_
#define SOCKETS_UTILS_H_

#include <stdlib.h>
#include <stdio.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>

#include "./hello.h"
#include "./sockets_server.h"
#include "./sockets_client.h"

t_log *iniciar_logger(char *file_name, char *name);
t_config *iniciar_config(char *file_name, char *name);
void paquete(int conexion, t_log *logger);
void terminar_programa(int conexion, t_log *logger, t_config *config);

#endif