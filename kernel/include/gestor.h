#ifndef GESTOR_H_
#define GESTOR_H_

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

extern int PID_GLOBAL;

extern pthread_mutex_t mutex_pid;

#endif /* GESTOR_H_ */