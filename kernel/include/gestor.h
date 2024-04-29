#ifndef GESTOR_H_
#define GESTOR_H_

#include <commons/collections/queue.h>
#include <semaphore.h>

extern t_log *LOGGER_KERNEL;
extern t_config *CONFIG_KERNEL;

extern char *PUERTO_ESCUCHA;
extern char *IP_MEMORIA;
extern char *PUERTO_MEMORIA;
extern char *IP_CPU;
extern char *PUERTO_CPU_DISPATCH;
extern char *PUERTO_CPU_INTERRUPT;
extern char *ALGORITMO_PLANIFICACION;
extern char *QUANTUM;
extern char **RECURSOS;
extern char **INSTANCIAS_RECURSOS;
extern int GRADO_MULTIPROGRAMACION;
extern char *IP_KERNEL;

extern int fd_kernel;
extern int fd_kernel_memoria;
extern int fd_kernel_cpu_dispatch;
extern int fd_kernel_cpu_interrupt;

extern int PID_GLOBAL;

extern sem_t semMultiprogramacion;
extern sem_t semNuevo;
extern sem_t semExit;
extern sem_t semListos_Ready;
extern sem_t semReady;
extern sem_t semExec;
extern sem_t semDetener;
extern sem_t semBloqueado;
extern sem_t semFinalizado;

extern t_queue *colaNuevos;
extern t_queue *colaListos;
extern t_queue *colaBloqueados;
extern t_queue *colaTerminados;
extern t_queue *procesosEnSistema;

extern pthread_mutex_t procesosNuevosMutex;
extern pthread_mutex_t procesosListosMutex;
extern pthread_mutex_t procesosBloqueadosMutex;
extern pthread_mutex_t procesoAEjecutarMutex;
extern pthread_mutex_t procesosFinalizadosMutex;
extern pthread_mutex_t procesoMutex;
extern pthread_mutex_t procesosEnSistemaMutex;
extern pthread_mutex_t mutex_pid;

#endif /* GESTOR_H_ */