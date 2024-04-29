#ifndef MAIN_H_
#define MAIN_H_

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>
#include <commons/collections/queue.h>
#include <semaphore.h>

#include "../../utils/include/hello.h"
#include "../../utils/include/sockets_server.h"
#include "../../utils/include/sockets_client.h"
#include "../../utils/include/sockets_utils.h"
#include "./comunicaciones.h"
#include "./consola.h"
#include "./planificador.h"
#include "./gestor.h"

void inicializar_config(void);
void iniciar_conexiones();
void iniciar_listas_y_semaforos(void);

#endif // MAIN_H_