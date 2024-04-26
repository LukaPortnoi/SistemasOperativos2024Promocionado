#include "../include/planificador.h"

void iniciar_planificador_corto_plazo()
{
    pthread_t hilo_planificador;
    pthread_create(&hilo_planificador, NULL, (void*)planificador_corto_plazo, NULL);
    pthread_detach(hilo_planificador);
}

void planificador_corto_plazo()
{
    // TODO: Implementar planificador de corto plazo
}

void planificar_proceso(t_pcb* pcb)
{
    if (string_equals_ignore_case(ALGORITMO_PLANIFICACION, "FIFO"))
    {
        planificar_proceso_fifo(pcb);
    }
    else if (string_equals_ignore_case(ALGORITMO_PLANIFICACION, "RR"))
    {
        planificar_proceso_rr(pcb);
    }
    else if (string_equals_ignore_case(ALGORITMO_PLANIFICACION, "VRR"))
    {
        planificar_proceso_vrr(pcb);
    }
}

void planificar_proceso_fifo(t_pcb* pcb){}
void planificar_proceso_rr(t_pcb* pcb){}
void planificar_proceso_vrr(t_pcb* pcb){}


