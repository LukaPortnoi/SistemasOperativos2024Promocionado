#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include "./gestor.h"

// PLANIFICADOR LARGO PLAZO
void iniciar_planificador_largo_plazo(void);
t_pcb *crear_proceso(void);
void chequear_grado_de_multiprogramacion(void);

void enviar_proceso_a_memoria(int pid_nuevo, char *path_proceso);
void serializar_inicializar_proceso(t_paquete *paquete, int pid_nuevo, char *path_proceso);

// PLANIFICADOR CORTO PLAZO
void iniciar_planificador_corto_plazo(void);
void planificar_PCB_cortoPlazo(void);

void ejecutar_PCB(t_pcb *pcb);
t_pcb *recibir_pcb_CPU(int socket);
void admitir_pcb(t_pcb *pcb);
void interrupcion_quantum(void);

// MANEJO DE SQUEUES
t_squeue *squeue_create();
void squeue_destroy(t_squeue *squeue);
void *squeue_pop(t_squeue *squeue);
void squeue_push(t_squeue *squeue, void *elemento);
void *squeue_peek(t_squeue *squeue);
t_pcb *squeue_pop_pcb(t_squeue *squeue);

// OTRAS FUNCIONES
void iniciar_colas_y_semaforos(void);
uint32_t asignar_pid(void);

void cambiar_estado_pcb(t_pcb *pcb, t_estado_proceso estado);

#endif // PLANIFICADOR_H_