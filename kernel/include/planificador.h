#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_


#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <commons/string.h>
#include "../../utils/include/pcb.h"
#include "./main.h"

void iniciar_listas_y_semaforos(void);

void ejecutar_PCB(t_pcb* pcb);
void meter_pcb_en_ejecucion(t_pcb* pcb);
void sacar_pcb_ejecucion(void);

t_pcb* sacar_pcb_cola_listos(void);

void iniciar_planificador_corto_plazo(void);
void interrupcion_quantum(void);
void planificar_proceso(t_pcb* pcb);
void planificar_proceso_fifo(t_pcb* pcb);
void planificar_proceso_rr(t_pcb* pcb);
void planificar_proceso_vrr(t_pcb* pcb);

void iniciar_planificador_largo_plazo(void);
void grado_multiprogamacion(void);

char* estado_to_string(t_estado_proceso estado);
void cambiar_estado_pcb(t_list* lista, t_estado_proceso estado);
void resume_all_timers(void);
void ordenarPorFIFO(void);
void compararPorFIFO(t_pcb* pcb1, t_pcb* pcb2);


#endif