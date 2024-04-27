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
#include "../../utils/include/pcb.h"
#include "./comunicaciones.h"
#include "./gestor.h"

void iniciar_consola_interactiva();
bool validar_comando(char *comando);
void ejecutar_comando(char *comando);

#endif /* CONSOLA_H_ */