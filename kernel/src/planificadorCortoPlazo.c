#include "../include/planificador.h"

void iniciar_planificador_corto_plazo()
{
    log_debug(LOGGER_KERNEL, "Inicia Planificador Corto Plazo");
    pthread_t hilo_corto_plazo, hilo_quantum;

    pthread_create(&hilo_corto_plazo, NULL, (void *)ejecutar_PCB, NULL);
    pthread_detach(hilo_corto_plazo);
    pthread_create(&hilo_quantum, NULL, (void *)interrupcion_quantum, NULL);
    pthread_detach(hilo_quantum);
}

/*void planificar_proceso(t_pcb* pcb) //que le llegue el ALGORITMO DE PLANIFICACION, no me lo reconoce
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
}*/


void ejecutar_PCB(t_pcb* pcb){}
/*{
    int conexionDispatch = crear_conexion(IP_CPU, PUERTO_CPU_DISPATCH);

    meter_pcb_en_ejecucion(pcb);
    enviar_pcb(pcb, conexionDispatch);
    log_info(LOGGER_KERNEL, "El PCB con ID %d se envio a  CPU", pcb->pid);
    recibir_pcb_de_CPU(conexionDispatch); 
    close(conexionDispatch);
}*/

void interrupcion_quantum(){}
/* {
    while (1)
    {
        sleep(QUANTUM);
        if (pcb_en_ejecucion != NULL)
        {
            log_info(logger_kernel, "Se interrumpio el proceso con ID %d por fin de quantum", pcb_en_ejecucion->pid);
            enviar_pcb_a_ready(pcb_en_ejecucion);
        }
    }
} */

void planificar_proceso_fifo(t_pcb* pcb){}
void planificar_proceso_rr(t_pcb* pcb){}
void planificar_proceso_vrr(t_pcb* pcb){}


