#ifndef GESTOR_H_
#define GESTOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <stdbool.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <readline/readline.h>

#include "../../utils/include/sockets_server.h"
#include "../../utils/include/sockets_client.h"
#include "../../utils/include/sockets_utils.h"
#include "../../utils/include/sockets_common.h"
#include "../../utils/include/pcb.h"
#include "../../utils/include/contexto.h"

extern char *IP_MEMORIA;
extern char *IP_CPU;
extern char *PUERTO_MEMORIA;
extern char *PUERTO_ESCUCHA_DISPATCH;
extern char *PUERTO_ESCUCHA_INTERRUPT;
extern char *CANTIDAD_ENTRADAS_TLB;
extern char *ALGORITMO_TLB;

extern t_log *LOGGER_CPU;
extern t_config *CONFIG;

extern int fd_cpu_dispatch;
extern int fd_cpu_interrupt;
extern int fd_cpu_memoria;

extern t_pcb *pcb_actual;
extern pthread_mutex_t mutex_interrupt;

#endif /* GESTOR_H_ */