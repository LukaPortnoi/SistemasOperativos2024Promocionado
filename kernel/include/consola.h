#ifndef CONSOLA_H_
#define CONSOLA_H_

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

void iniciar_consola_interactiva(t_log *logger);
bool validar_comando(char *comando);
void ejecutar_comando(char *comando, t_log *logger);

#endif /* CONSOLA_H_ */