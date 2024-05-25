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
void atender_quantum(void* arg);
void atender_interrupcionBloqueo(t_pcb *pcb);

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
void mostrar_procesos_en_squeue(t_squeue *squeue);
void finalizar_proceso(t_pcb *pcb);

// INTERFACES
bool consultar_existencia_instruccion(int socket_interfaz, nombre_instruccion instruccion);
void ejecutar_intruccion_io(t_pcb *pcb_recibido);
void enviarInterfazGenerica(int socket, int unidades_trabajo);
t_paquete *crear_paquete_InterfazGenerica(int unidades_trabajo);
t_buffer *crear_buffer_InterfazGenerica(int unidades_trabajo);
bool deserializar_respuesta_consultar_existencia_instruccion(t_paquete *paquete);
t_paquete *crear_paquete_solo_codigo_de_operacion(op_cod codigo);

#endif // PLANIFICADOR_H_