#ifndef GESTOR_H_
#define GESTOR_H_

extern t_log* LOGGER_MEMORIA;
extern t_config* CONFIG_MEMORIA;
extern char* PUERTO_ESCUCHA_MEMORIA;
extern char* IP_MEMORIA;
extern int TAM_MEMORIA;
extern int TAM_PAGINA;
extern char *PATH_INSTRUCCIONES;
extern int RETARDO_RESPUESTA;
extern int CLIENTE_CPU, CLIENTE_KERNEL, CLIENTE_IN_OU;

extern t_list procesos_totales;

extern pthread_mutex_t mutex_procesos;

#endif /* GESTOR_H_ */