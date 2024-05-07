#ifndef GESTOR_H_
#define GESTOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <readline/readline.h>

#include "../../utils/include/sockets_server.h"
#include "../../utils/include/sockets_client.h"
#include "../../utils/include/sockets_utils.h"

typedef struct
{
    uint32_t pid;
    char *path;
    t_list *instrucciones;
} t_proceso_memoria;

extern t_log *LOGGER_MEMORIA;
extern t_config *CONFIG_MEMORIA;
extern char *PUERTO_ESCUCHA_MEMORIA;
extern char *IP_MEMORIA;
extern int TAM_MEMORIA;
extern int TAM_PAGINA;
extern char *PATH_INSTRUCCIONES;
extern int RETARDO_RESPUESTA;
extern int CLIENTE_CPU, CLIENTE_KERNEL, CLIENTE_IN_OU;

extern t_list *procesos_totales;
extern pthread_mutex_t mutex_procesos;
extern t_proceso_memoria *proceso_memoria;

#endif /* GESTOR_H_ */