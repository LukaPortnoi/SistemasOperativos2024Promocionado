#include "../include/planificador.h"

//PLANIFICADOR LARGO PLAZO
void crear_hilo_largo_plazo()
{
    pthread_t hilo_largo_plazo;
    pthread_create(&hilo_largo_plazo, NULL, (void*)grado_multiprogamacion, NULL);
    pthread_detach(hilo_largo_plazo);
}

void enviar_proceso_a_memoria(int pid_nuevo, char *path_proceso)
{
    t_paquete *paquete_nuevo_proceso = crear_paquete_con_codigo_de_operacion(INICIALIZAR_PROCESO);
    enviar_paquete(paquete_nuevo_proceso, fd_kernel_memoria);
    eliminar_paquete(paquete_nuevo_proceso);
    log_info(LOGGER_KERNEL, "El PCB con ID %d se envio a MEMORIA", pcb->pid);
}

//PLANIFICADOR CORTO PLAZO
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

// OTRAS FUNCIONES

void iniciar_colas_y_semaforos(){

    pthread_mutex_init(&procesosNuevosMutex, NULL);
    pthread_mutex_init(&procesosListosMutex, NULL);
    pthread_mutex_init(&procesosBloqueadosMutex, NULL);
    pthread_mutex_init(&procesoAEjecutarMutex, NULL);
    pthread_mutex_init(&procesosFinalizadosMutex, NULL);
    pthread_mutex_init(&procesoMutex, NULL);
    pthread_mutex_init(&procesosEnSistemaMutex, NULL);
    pthread_mutex_init(&mutex_pid, NULL);

    sem_init(&semMultiprogramacion, 0, 0);
    sem_init(&semNuevo, 0, 0);
    sem_init(&semExit, 0, 0);
    sem_init(&semListos_Ready, 0, 0);
    sem_init(&semReady, 0, 0);
    sem_init(&semExec, 0, 0);
    sem_init(&semDetener, 0, 0);
    sem_init(&semBloqueado, 0, 0);
    sem_init(&semFinalizado, 0, 0);
   
    colaNuevos = queue_create();
    colaListos = queue_create();
    colaBloqueados = queue_create();
    colaTerminados = queue_create();
    procesosEnSistema = queue_create();
       
}

int asignar_pid()
{
    int valor_pid;

    pthread_mutex_lock(&mutex_pid);
    valor_pid = PID_GLOBAL;
    PID_GLOBAL++;
    pthread_mutex_unlock(&mutex_pid);

    return valor_pid;
}

char *estado_to_string(t_estado_proceso estado)
{
    switch (estado)
    {
    case NUEVO:
        return "NUEVO";
    case LISTO:
        return "LISTO";
    case EJECUTANDO:
        return "EJECUTANDO";
    case BLOQUEADO:
        return "BLOQUEADO";
    case FINALIZADO:
        return "TERMINADO";
    case ERROR:
        return "EJECUTANDO";
    default:
        return "ERROR";
    }
}

void cambiar_estado_pcb(t_pcb *pcb, t_estado_proceso estado)
{
    if (estado != pcb->estado)
    {
        log_info(LOGGER_KERNEL, "PID: %d - Estado Anterior: %s - Estado Actual: %s", pcb->pid, estado_to_string(pcb->estado), estado_to_string(estado));
    }
    pthread_mutex_lock(&procesoMutex);
    pcb->estado = estado;
    pthread_mutex_unlock(&procesoMutex);
}

void ordenarPorFIFO(t_pcb *proceso)
{
    pthread_mutex_lock(&procesosListosMutex);
    list_add(colaListos, proceso);
    list_sort(colaListos, (void *)compararPorFIFO);
    pthread_mutex_unlock(&procesosListosMutex);
}

void compararPorFIFO(t_pcb *proceso1, t_pcb *proceso2) {}