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
void iniciar_planificador_corto_plazo(void);
void planificador_corto_plazo(void);
void planificar_proceso(t_pcb* pcb);
void planificar_proceso_fifo(t_pcb* pcb);
void planificar_proceso_rr(t_pcb* pcb);
void planificar_proceso_vrr(t_pcb* pcb);
void ejecutar_PCB(t_pcb* pcb);
void meter_pcb_en_ejecucion(t_pcb* pcb);
void sacar_pcb_ejecucion(void);
void ordenarPorFIFO(void);
t_pcb* sacar_pcb_cola_listos(void);



void iniciar_planificador_largo_plazo(void);
void grado_multiprogamacion(void);


#endif