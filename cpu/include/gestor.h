#ifndef GESTOR_H_
#define GESTOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <signal.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <commons/memory.h>
#include <readline/readline.h>

#include "../../utils/include/sockets_server.h"
#include "../../utils/include/sockets_client.h"
#include "../../utils/include/sockets_utils.h"
#include "../../utils/include/sockets_common.h"
#include "../../utils/include/pcb.h"
#include "../../utils/include/IO.h"
#include "../../utils/include/contexto.h"
#include "../../utils/include/memoria.h"

extern char *IP_MEMORIA;
extern char *IP_CPU;
extern char *PUERTO_MEMORIA;
extern char *PUERTO_ESCUCHA_DISPATCH;
extern char *PUERTO_ESCUCHA_INTERRUPT;
extern int CANTIDAD_ENTRADAS_TLB;
extern char *ALGORITMO_TLB;
extern uint32_t TAM_PAGINA;

extern t_log *LOGGER_CPU;
extern t_config *CONFIG;


extern int fd_cpu_dispatch;
extern int fd_cpu_interrupt;
extern int fd_cpu_memoria;

extern t_pcb *pcb_actual;
extern bool esSyscall;
extern bool envioPcb;

extern pthread_mutex_t mutex_pcb_actual;
extern pthread_mutex_t mutex_interrupt;

#endif /* GESTOR_H_ */