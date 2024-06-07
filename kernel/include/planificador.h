#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include "./gestor.h"

// PLANIFICADOR LARGO PLAZO
void iniciar_planificador_largo_plazo(void);
void crear_proceso(char *path_proceso);
void chequear_grado_de_multiprogramacion(void);

void enviar_proceso_a_memoria(int pid_nuevo, char *path_proceso);
void serializar_inicializar_proceso(t_paquete *paquete, int pid_nuevo, char *path_proceso);

// PLANIFICADOR CORTO PLAZO
void iniciar_planificador_corto_plazo(void);
void planificar_PCB_cortoPlazo(void);

void ejecutar_PCB(t_pcb *pcb);
void desalojo_cpu(t_pcb *pcb, pthread_t hilo_quantum);
t_pcb *recibir_pcb_CPU(int socket);
void atender_quantum(void *arg);

// OTRAS FUNCIONES
void detener_planificadores();
void iniciar_planificadores();
void iniciar_colas_y_semaforos(void);
void inicializar_recursos();
uint32_t asignar_pid(void);
void cambiar_estado_pcb(t_pcb *pcb, t_estado_proceso estado);
void proceso_listo(t_pcb *pcb, bool es_ready_plus);
void finalizar_proceso(t_pcb *pcb);
void bloquear_proceso(t_pcb *pcb, char *motivo);
void desbloquear_proceso(uint32_t pid);

// RECURSOS
void asignar_recurso(t_pcb *pcb, char *recurso);
//int encontrar_recurso(char *recurso);
t_recurso *encontrar_recurso(char *recurso);



// INTERFACES
void ejecutar_intruccion_io(t_pcb *pcb_recibido);
t_interfaz_recibida *buscar_interfaz_por_nombre(char *nombre_interfaz);

#endif // PLANIFICADOR_H_