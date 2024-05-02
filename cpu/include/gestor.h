#ifndef GESTOR_H_
#define GESTOR_H_

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

#endif /* GESTOR_H_ */