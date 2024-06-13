#ifndef GESTOR_H_
#define GESTOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <inttypes.h>
#include <semaphore.h>
#include <sys/socket.h>
#include <readline/readline.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>

#include "../../utils/include/sockets_server.h"
#include "../../utils/include/sockets_client.h"
#include "../../utils/include/sockets_utils.h"
#include "../../utils/include/memoria.h"
#include "../../utils/include/IO.h"



typedef struct
{
	uint32_t numeroMarco;
	uint32_t pid;
} t_marco;
typedef struct
{
    t_list *tabla;
    pthread_mutex_t *mutex;
} t_tabla_paginas;
typedef struct
{
    uint32_t pid;
    char *path;
    uint32_t tamanio;
    t_list *instrucciones;
    t_list *tabla_paginas;
    // t_tabla_paginas *tabla_paginas;
} t_proceso_memoria;

extern char *PUERTO_ESCUCHA_MEMORIA;
extern char *IP_MEMORIA;
extern int TAM_MEMORIA;
extern int TAM_PAGINA;
extern char *PATH_INSTRUCCIONES;
extern int RETARDO_RESPUESTA;
extern int CLIENTE_CPU, CLIENTE_KERNEL, CLIENTE_IN_OU;

extern t_log *LOGGER_MEMORIA;
extern t_config *CONFIG_MEMORIA;
extern uint32_t valorGlobalDescritura;

extern int fd_memoria;

extern pthread_mutex_t mutex_procesos;
extern pthread_mutex_t mutex_comunicacion_procesos;
extern pthread_mutex_t mutex_memoria_usuario;
extern t_proceso_memoria *proceso_memoria;

extern void *memoriaUsuario;
extern uint32_t tamanioMemoria;
extern t_list *marcosPaginas;
extern t_list *procesos_totales;

#endif /* GESTOR_H_ */