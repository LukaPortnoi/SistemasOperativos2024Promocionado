#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <commons/string.h>
#include <commons/collections/queue.h>
#include <semaphore.h>
#include <pthread.h>

#include "../../utils/include/sockets_client.h"
#include "../../utils/include/sockets_utils.h"
#include "../../utils/include/sockets_common.h"
#include "../../utils/include/pcb.h"

#include "./gestor.h"
#include "./comunicaciones.h"
#include "./planificador.h"

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
void interrupcion_quantum(void);

// SQUEUES
t_squeue *squeue_create();
void squeue_destroy(t_squeue *squeue);
void *squeue_pop(t_squeue *squeue);
void squeue_push(t_squeue *squeue, void *elemento);
void *squeue_peek(t_squeue *squeue);
t_pcb *squeue_pop_pcb(t_squeue *squeue);

void iniciar_colas_y_semaforos(void);
int asignar_pid(void);

char *estado_to_string(t_estado_proceso estado);
void cambiar_estado_pcb(t_pcb *pcb, t_estado_proceso estado);

#endif // PLANIFICADOR_H_