#include "../include/planificador.h"

pthread_mutex_t procesoMutex;
pthread_mutex_t procesosEnSistemaMutex;
pthread_mutex_t mutex_pid;

/* sem_t semMultiprogramacion;
sem_t semNuevo;
sem_t semExit;
sem_t semListos_Ready;
sem_t semReady;
sem_t semExec;
sem_t semDetener;
sem_t semBloqueado;
sem_t semFinalizado; */

t_queue *procesosEnSistema;

t_squeue *squeue_new;
t_squeue *squeue_ready;
t_squeue *squeue_block;
t_squeue *squeue_exit;

// PLANIFICADOR LARGO PLAZO
void iniciar_planificador_largo_plazo() {}
/* {
    pthread_t hilo_largo_plazo;
    pthread_create(&hilo_largo_plazo, NULL, (void*)grado_multiprogamacion, NULL);
    pthread_detach(hilo_largo_plazo);
} */

t_pcb *crear_proceso()
{
    int pid_nuevo = asignar_pid();
    t_pcb *pcb = crear_pcb(pid_nuevo, LISTO, QUANTUM); // POR AHORA ES A LISTOS. PARA CHECK3 ES A NUEVOS
    squeue_push(squeue_ready, pcb);                    // PARA CHECK2 ES EN READY, PARA EL CHECK3 ES EN NEW YA QUE HAY QUE DESARROLLAR EL PLANIFICADOR LARGO PLAZO
    log_debug(LOGGER_KERNEL, "Se crea el proceso %d en NEW", pid_nuevo);
    return pcb;
}

/*void chequear_grado_de_multiprogramacion()
{
    while (1) {
        sem_wait(&semMultiprogramacion);
        if (queue_size(procesosEnSistema) < GRADO_MULTIPROGRAMACION) {
            sem_post(&semNuevo);
        }
    }
}*/

void enviar_proceso_a_memoria(int pid_nuevo, char *path_proceso)
{
    t_paquete *paquete_nuevo_proceso = crear_paquete_con_codigo_de_operacion(INICIALIZAR_PROCESO);

    agregar_a_paquete(paquete_nuevo_proceso, &pid_nuevo, sizeof(int));
    agregar_a_paquete(paquete_nuevo_proceso, path_proceso, strlen(path_proceso) + 1);

    enviar_paquete(paquete_nuevo_proceso, fd_kernel_memoria);

    eliminar_paquete(paquete_nuevo_proceso);
    
    log_info(LOGGER_KERNEL, "Se envio el proceso con PID %d a MEMORIA", pid_nuevo);
}

// PLANIFICADOR CORTO PLAZO
void iniciar_planificador_corto_plazo()
{
    log_debug(LOGGER_KERNEL, "Inicia Planificador Corto Plazo");
    pthread_t hilo_corto_plazo;
    //    pthread_t hilo_quantum;

    pthread_create(&hilo_corto_plazo, NULL, (void *)planificar_PCB_cortoPlazo, NULL);
    pthread_detach(hilo_corto_plazo);
    //    pthread_create(&hilo_quantum, NULL, (void *)interrupcion_quantum, NULL);
    //    pthread_detach(hilo_quantum);
}

void planificar_PCB_cortoPlazo()
{
    while (1)
    {
        sem_wait(&semListos_Ready);
        t_pcb *pcb = squeue_pop(squeue_ready);
        if (pcb != NULL)
        {
            log_info(LOGGER_KERNEL, "Se planifica el PCB con PID %d", pcb->pid);
            ejecutar_PCB(pcb);
        }
    }
}

void ejecutar_PCB(t_pcb *pcb)
{

    enviar_pcb(pcb, fd_kernel_cpu_dispatch);
    log_info(LOGGER_KERNEL, "El PCB con ID %d se envio a  CPU", pcb->pid);

    recibir_pcb_CPU(fd_kernel_cpu_dispatch);

    close(fd_kernel_cpu_dispatch);
}

void interrupcion_quantum() {}

// MANEJO DE SQUEUES

t_squeue *squeue_create()
{
    t_squeue *squeue = malloc(sizeof(t_squeue));
    squeue->cola = queue_create();
    squeue->mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(squeue->mutex, NULL);
    return squeue;
}

void squeue_destroy(t_squeue *squeue)
{
    queue_destroy(squeue->cola);
    pthread_mutex_destroy(squeue->mutex);
    free(squeue);
}

void *squeue_pop(t_squeue *squeue)
{
    void *elemento;
    pthread_mutex_lock(squeue->mutex);
    elemento = queue_pop(squeue->cola);
    pthread_mutex_unlock(squeue->mutex);
    return elemento;
}

void squeue_push(t_squeue *squeue, void *elemento)
{
    pthread_mutex_lock(squeue->mutex);
    queue_push(squeue->cola, elemento);
    pthread_mutex_unlock(squeue->mutex);
}

void *squeue_peek(t_squeue *squeue)
{
    void *elemento;
    pthread_mutex_lock(squeue->mutex);
    elemento = queue_peek(squeue->cola);
    pthread_mutex_unlock(squeue->mutex);
    return elemento;
}

// OTRAS FUNCIONES

void iniciar_colas_y_semaforos()
{
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

    procesosEnSistema = queue_create();

    squeue_new = squeue_create();
    squeue_ready = squeue_create();
    squeue_block = squeue_create();
    squeue_exit = squeue_create();
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
        return "ERROR";
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

void recibir_pcb_CPU(int fd_cpu)
{
    t_pcb *pcbDeCPU = recibir_pcb(fd_cpu);

    // ACA HAY QUE VER QUE TIENE QUE HACER EL KERNEL CUANDO RECIBE EL PCB DE LA CPU
}