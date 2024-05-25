#ifndef CONSOLA_H_
#define CONSOLA_H_

#include "./gestor.h"
#include "./planificador.h"

void iniciar_consola_interactiva();

bool validar_comando(char *comando);
void ejecutar_comando(char *comando);
char *comando_autocompletar(const char *text, int state);
char **consola_completar(const char *text, int start, int end);
void inicializar_readline();

void ejecutar_script(char *path);
void iniciar_proceso(char *path_proceso);
void finalizar_proceso_consola(char *pid_string);
t_pcb *buscar_proceso_en_colas(int pid);
t_pcb *buscar_proceso_en_cola(t_squeue *squeue, int pid);

void iniciar_planificacion(void);
void detener_planificacion(void);

void cambiar_multiprogramacion(char *grado_multiprogramacion_string);
void mostrar_listado_estados_procesos(void);
void mostrar_procesos_en_cola(t_squeue *squeue, const char *nombre_cola);
void mostrar_procesos_en_lista(t_list *lista, const char *nombre_cola);

#endif /* CONSOLA_H_ */