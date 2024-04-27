#include "../include/planificador.h"
#include "../include/main.h"


void iniciar_planificador_corto_plazo()
{
    pthread_t hilo_planificador;
    pthread_create(&hilo_planificador, NULL, (void*)planificador_corto_plazo, NULL);
    pthread_detach(hilo_planificador);
}

void planificador_corto_plazo()
{
     log_debug(logger_kernel, "Inicia Planificador Corto Plazo");
    while (1) {
        sem_wait(&semProcesoListo);
        //log_debug(logger_kernel, "Planificador corto plazo notificado proceso listo");
        //int conexionDispatch = crear_conexion(ip_cpu, puerto_cpu, logger_kernel);
        if (!strcmp(ALGORITMO_PLANIFICACION, "RR")) { //cambiamos en el config
 //            ordenarPorFIFO(); ojo no esta hecho el de RR hacerlo!
        }
/*         t_pcb* pcb = sacar_pcb_cola_listos();           //esto no anda
        if ((pcb->llegada_a_listo != NULL)){
        temporal_destroy(pcb->llegada_a_listo); 
        }
        ejecutar_PCB(pcb); */
    }
    // TODO: Implementar planificador de corto plazo
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


void ejecutar_PCB(t_pcb* pcb)
{
    int conexionDispatch = crear_conexion(IP_CPU, PUERTO_CPU_DISPATCH, LOGGER_KERNEL);

    meter_pcb_en_ejecucion(pcb);
    enviar_pcb(pcb, conexionDispatch, logger_kernel);
    log_info(logger_kernel, "El PCB con ID %d se envio a  CPU", pcb->pid);
    recibir_pcb_de_CPU(conexionDispatch); 
    close(conexionDispatch);
}

void planificar_proceso_fifo(t_pcb* pcb){}
void planificar_proceso_rr(t_pcb* pcb){}
void planificar_proceso_vrr(t_pcb* pcb){}


