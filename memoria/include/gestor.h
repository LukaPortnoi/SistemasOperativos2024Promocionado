#ifndef GESTOR_H_
#define GESTOR_H_



typedef struct
{
    int pid;
    char *path;
    t_list *instrucciones;
} t_proceso_memoria;

extern t_log* LOGGER_MEMORIA;
extern t_config* CONFIG_MEMORIA;
extern char* PUERTO_ESCUCHA_MEMORIA;
extern char* IP_MEMORIA;
extern int TAM_MEMORIA;
extern int TAM_PAGINA;
extern char *PATH_INSTRUCCIONES;
extern int RETARDO_RESPUESTA;
extern int CLIENTE_CPU, CLIENTE_KERNEL, CLIENTE_IN_OU;

extern t_list *procesos_totales;
extern pthread_mutex_t mutex_procesos;
extern t_proceso_memoria *proceso_memoria;

#endif /* GESTOR_H_ */