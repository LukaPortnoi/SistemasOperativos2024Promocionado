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

t_log *iniciar_logger(void);
t_config *iniciar_config(void);
void paquete(int conexion, t_log *logger);
void terminar_programa(int conexion, t_log *logger, t_config *config);

#endif