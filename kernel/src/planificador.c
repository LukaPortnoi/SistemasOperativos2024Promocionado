#include "../include/planificador.h"

t_list *colaNuevos;
t_list *colaListos;
t_list *colaBloqueados;
t_pcb *pcbAEjecutar;
t_list *colaTerminados;
t_list *procesosEnSistema; 
t_list *colaBloqueadosFileSystem;
t_list *colaBloqueadosRecursos;
t_list *tabla_global_archivos_abiertos;

static pthread_mutex_t procesosNuevosMutex;
static pthread_mutex_t procesosListosMutex;
static pthread_mutex_t procesosBloqueadosMutex;
static pthread_mutex_t procesosBloqueadosFileSystemMutex;
static pthread_mutex_t procesoAEjecutarMutex;
static pthread_mutex_t procesosTerminadosMutex;
static pthread_mutex_t procesoMutex;
static pthread_mutex_t procesosEnSistemaMutex;
static pthread_mutex_t puedeIniciarCompactacionMutex;

void iniciar_listas_y_semaforos() 
{
    colaNuevos = list_create();
    colaListos = list_create();
    colaBloqueados = list_create();
    pcbAEjecutar = NULL;
    colaTerminados = list_create();
    procesosEnSistema = list_create();
    colaBloqueadosFileSystem = list_create();
    colaBloqueadosRecursos = list_create();
    tabla_global_archivos_abiertos = list_create();

    pthread_mutex_init(&procesosNuevosMutex, NULL);
    pthread_mutex_init(&procesosListosMutex, NULL);
    pthread_mutex_init(&procesosBloqueadosMutex, NULL);
    pthread_mutex_init(&procesoAEjecutarMutex, NULL);
    pthread_mutex_init(&procesosTerminadosMutex, NULL);
    pthread_mutex_init(&procesoMutex, NULL);
    pthread_mutex_init(&procesosEnSistemaMutex, NULL);
    pthread_mutex_init(&procesosBloqueadosFileSystemMutex, NULL);
    pthread_mutex_init(&puedeIniciarCompactacionMutex, NULL);

    sem_init(&compactacion, 0, 0);
    sem_init(&semFileSystem, 0, 0);
    sem_init(&semMultiprogramacion, 0, grado_multiprogramacion);
    sem_init(&semProcesoNuevo, 0, 0);
    sem_init(&semProcesoListo, 0, 0);
}

char* estado_to_string(t_estado_proceso estado)
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

void cambiar_estado_pcb(t_pcb* pcb, t_estado_proceso estado) 
{
    if(estado!=pcb->estado){
        log_info(logger_kernel, "PID: %d - Estado Anterior: %s - Estado Actual: %s", pcb->pid, estado_to_string(pcb->estado), estado_to_string(estado));
    }
    pthread_mutex_lock(&procesoMutex);
    pcb->estado = estado;
    pthread_mutex_unlock(&procesoMutex);
}

void resume_all_timers()
{
    for(int i = 0; i < list_size(colaListos); i++){
        t_pcb* pcb = list_get(colaListos, i);
        temporal_resume(pcb->llegada_a_listo);
    }
}

void ordenarPorFIFO(t_pcb* proceso)
{
    pthread_mutex_lock(&procesosListosMutex);
    stop_all_timers();
    list_add(colaListos, proceso)
    list_sort(colaListos, (void *)agregar_pcb_en_cola_listos);
    resume_all_timers();
    pthread_mutex_unlock(&procesosListosMutex);
}



// ------------------------------------------------------------------------------------------
// -- Cola LISTOS --
// ------------------------------------------------------------------------------------------

void loggear_cola_listos()
{
    char * stringLog = string_new();
    for(int i = 0; i < list_size(colaListos); i++){
        t_pcb* pcbReady = list_get(colaListos, i);
        char* pid = string_itoa(pcbReady->pid);
        string_append(&stringLog, pid);
        string_append(&stringLog, " ");
    }
    log_info(logger_kernel, "Cola Ready %s: [ %s]", ALGORITMO_PLANIFICACION, stringLog);
}

t_pcb* sacar_pcb_cola_listos() 
{
    pthread_mutex_lock(&procesosListosMutex);
    t_pcb* proceso = list_remove(colaListos, 0);
    temporal_stop(proceso->llegada_a_listo);
    sacar_pcb_cola_procesos_en_sistema(proceso);  
    pthread_mutex_unlock(&procesosListosMutex); 
    return proceso;
}


void agregar_pcb_en_cola_listos(t_pcb* proceso) 
{
    pthread_mutex_lock(&procesosListosMutex);
    list_add(colaListos, proceso); 
    cambiar_estado_pcb(proceso, LISTO);
    loggear_cola_listos();
    agregar_pcb_en_cola_procesos_en_sistema(proceso);  
    pthread_mutex_unlock(&procesosListosMutex);
    //sem_post(&semProcesoListo);
}

void sacar_pcb_cola_procesos_en_sistema(t_pcb* proceso)
{
    pthread_mutex_lock(&procesosEnSistemaMutex);
    list_remove_element(procesosEnSistema, proceso);
    pthread_mutex_unlock(&procesosEnSistemaMutex);
    return;
}


void agregar_pcb_en_cola_procesos_en_sistema(t_pcb* proceso) 
{
    pthread_mutex_lock(&procesosEnSistemaMutex);
    list_add(procesosEnSistema, proceso); 
    pthread_mutex_unlock(&procesosEnSistemaMutex);
}


// ------------------------------------------------------------------------------------------
// -- Cola NUEVOS --
// ------------------------------------------------------------------------------------------

void agregar_pcb_cola_nuevos(t_pcb* proceso) 
{
    pthread_mutex_lock(&procesosNuevosMutex);
    list_add(colaNuevos, proceso);
    agregar_pcb_en_cola_procesos_en_sistema(proceso);
    pthread_mutex_unlock(&procesosNuevosMutex);
    sem_post(&semProcesoNuevo);
}


t_pcb* sacar_pcb_de_cola_nuevo() 
{
    pthread_mutex_lock(&procesosNuevosMutex);
    t_pcb* proceso = list_remove(colaNuevos, 0);
    sacar_pcb_cola_procesos_en_sistema(proceso);
    pthread_mutex_unlock(&procesosNuevosMutex);
    return proceso;
}


// ------------------------------------------------------------------------------------------
// -- En ejecucion --
// ------------------------------------------------------------------------------------------