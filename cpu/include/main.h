#ifndef MAIN_H_
#define MAIN_H_

#include <stdlib.h>
#include <stdio.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>

#include "../../utils/include/sockets_server.h"
#include "../../utils/include/sockets_client.h"
#include "../../utils/include/sockets_utils.h"
#include "../../utils/include/sockets_common.h"
#include "../../utils/include/pcb.h"
#include "../../utils/include/contexto.h"
#include "./comunicaciones.h"

void inicializar_config(void);
void iniciar_conexiones(void);
void escuchar_interrupt(void);
t_instruccion *fetch(int pid, int pc);
void decode(t_instruccion *instruccion);
void pedir_instruccion_memoria(int pid, int pc, int socket);
t_instruccion *deserializar_instruccion(int socket);

#endif