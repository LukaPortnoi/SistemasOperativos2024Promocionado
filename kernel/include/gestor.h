#ifndef GESTOR_H_
#define GESTOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/collections/queue.h>

#include "../../utils/include/sockets_server.h"
#include "../../utils/include/sockets_client.h"
#include "../../utils/include/sockets_utils.h"
#include "../../utils/include/sockets_common.h"
#include "../../utils/include/squeue.h"
#include "../../utils/include/pcb.h"
#include "../../utils/include/IO.h"

typedef struct
{
    char *nombre_interfaz_recibida;
    t_tipo_interfaz tipo_interfaz_recibida;
    int socket_interfaz_recibida;
    t_squeue *cola_procesos_bloqueados;
} t_interfaz_recibida;

extern t_log *LOGGER_KERNEL;
extern t_config *CONFIG_KERNEL;

extern char *PUERTO_ESCUCHA;
extern char *IP_MEMORIA;
extern char *PUERTO_MEMORIA;
extern char *IP_CPU;
extern char *PUERTO_CPU_DISPATCH;
extern char *PUERTO_CPU_INTERRUPT;
extern char *ALGORITMO_PLANIFICACION;
extern uint32_t QUANTUM;
extern char **RECURSOS;
extern char **INSTANCIAS_RECURSOS;
extern int GRADO_MULTIPROGRAMACION;
extern char *IP_KERNEL;

extern int fd_kernel;
extern int fd_kernel_memoria;
extern int fd_kernel_cpu_dispatch;
extern int fd_kernel_cpu_interrupt;

extern uint32_t PID_GLOBAL;

extern t_list *interfaces_conectadas;
extern char *nombre_interfaz;
extern int unidades_de_trabajo;
extern nombre_instruccion instruccion_de_IO_a_ejecutar;

extern t_pcb *pcb_ejecutandose;

extern sem_t semMultiprogramacion;
extern sem_t semNew;
extern sem_t semReady;
extern sem_t semExec;
extern sem_t semDetener;
extern sem_t semBlocked;
extern sem_t semFinalizado;
extern sem_t semExit;

extern t_list *procesosEnSistema;

extern pthread_mutex_t procesoMutex;
extern pthread_mutex_t procesosEnSistemaMutex;
extern pthread_mutex_t mutex_pid;
extern pthread_mutex_t mutex_lista_interfaces;
extern pthread_mutex_t mutex_lista_blocked;

extern t_squeue *squeue_new;
extern t_squeue *squeue_ready;
extern t_squeue *squeue_exec;
extern t_list *list_blocked;
extern t_squeue *squeue_exit;

extern pthread_t hilo_quantum;

#endif /* GESTOR_H_ */