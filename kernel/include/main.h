#ifndef MAIN_H_
#define MAIN_H_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>
#include <pthread.h>
#include <commons/collections/queue.h>
#include <semaphore.h>

#include "../../utils/include/sockets_server.h"
#include "../../utils/include/sockets_client.h"
#include "../../utils/include/sockets_utils.h"

#include "./gestor.h"
#include "./comunicaciones.h"
#include "./consola.h"
#include "./planificador.h"


void inicializar_config();
void iniciar_conexiones();

#endif // MAIN_H_