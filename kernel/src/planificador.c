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

pthread_mutex_t procesosNuevosMutex;
pthread_mutex_t procesosListosMutex;
pthread_mutex_t procesosBloqueadosMutex;
pthread_mutex_t procesosBloqueadosFileSystemMutex;
pthread_mutex_t procesoAEjecutarMutex;
pthread_mutex_t procesosTerminadosMutex;
pthread_mutex_t procesoMutex;
pthread_mutex_t puedeIniciarCompactacionMutex;

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

    /*
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
    sem_init(&semProcesoListo, 0, 0); */
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