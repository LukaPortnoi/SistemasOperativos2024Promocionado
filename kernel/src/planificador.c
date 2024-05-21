#include "../include/planificador.h"

pthread_mutex_t procesoMutex;
pthread_mutex_t procesosEnSistemaMutex;
pthread_mutex_t mutex_pid;

sem_t semMultiprogramacion;
sem_t semNew;
sem_t semReady;
sem_t semExec;
sem_t semDetener;
sem_t semBlocked;
sem_t semFinalizado;
sem_t semExit;

t_list *procesosEnSistema;

t_squeue *squeue_new;
t_squeue *squeue_ready;
t_squeue *squeue_exec;
t_squeue *squeue_blocked;
t_squeue *squeue_exit;

uint32_t PID_GLOBAL = 1;

pthread_t hilo_quantum;

// PLANIFICADOR LARGO PLAZO
void iniciar_planificador_largo_plazo()
{
    pthread_t hilo_largo_plazo;
    log_debug(LOGGER_KERNEL, "Inicia Planificador Largo Plazo");
    pthread_create(&hilo_largo_plazo, NULL, (void *)chequear_grado_de_multiprogramacion, NULL);
    pthread_detach(hilo_largo_plazo);
}

void crear_proceso(char *path_proceso)
{
    uint32_t pid_nuevo = asignar_pid();
    t_pcb *pcb = crear_pcb(pid_nuevo, NUEVO, QUANTUM);
    squeue_push(squeue_new, pcb);

    pthread_mutex_lock(&procesosEnSistemaMutex);
    list_add(procesosEnSistema, pcb);
    pthread_mutex_unlock(&procesosEnSistemaMutex);

    log_info(LOGGER_KERNEL, "Se crea el proceso %d en NEW", pcb->pid);

    enviar_proceso_a_memoria(pcb->pid, path_proceso);

    // sem_post(&semMultiprogramacion); // CAMBIAR A CONTADOR
    sem_post(&semNew);
}

void chequear_grado_de_multiprogramacion()
{
    while (1)
    {
        /* sem_wait(&semNew);
        sem_wait(&semMultiprogramacion); // ESTE SEMAFORO NO ES SUFICIENTE, REVISAR CHEQUEAR GRADO
        log_trace(LOGGER_KERNEL, "Chequeando grado de multiprogramacion!");
        if (list_size(procesosEnSistema) < GRADO_MULTIPROGRAMACION)
        {
            t_pcb *pcb_a_mover = squeue_pop(squeue_new); // CHEQUEAR TEMA BLOQUEADOS
            cambiar_estado_pcb(pcb_a_mover, LISTO);      // previamente tendria que hablar con memoria antes de pasar a ready, tengo que esperar respuesta de que memoria todo en orden
            squeue_push(squeue_ready, pcb_a_mover);

            sem_post(&semReady);
        } */

        sem_wait(&semNew);
        sem_wait(&semMultiprogramacion);

        int sem_value;
        if (sem_getvalue(&semMultiprogramacion, &sem_value) == 0)
        {
            log_trace(LOGGER_KERNEL, "Valor semaforo multiprogramacion: %d", sem_value);
        }

        t_pcb *pcb_a_mover = squeue_pop(squeue_new); // CHEQUEAR TEMA BLOQUEADOS
        cambiar_estado_pcb(pcb_a_mover, LISTO);      // previamente tendria que hablar con memoria antes de pasar a ready, tengo que esperar respuesta de que memoria todo en orden
        squeue_push(squeue_ready, pcb_a_mover);
        sem_post(&semReady);
    }
}

void enviar_proceso_a_memoria(int pid_nuevo, char *path_proceso)
{
    t_paquete *paquete_nuevo_proceso = crear_paquete_con_codigo_de_operacion(INICIALIZAR_PROCESO);
    serializar_inicializar_proceso(paquete_nuevo_proceso, pid_nuevo, path_proceso);
    enviar_paquete(paquete_nuevo_proceso, fd_kernel_memoria);

    log_debug(LOGGER_KERNEL, "El PID %d se envio a MEMORIA", pid_nuevo);

    eliminar_paquete(paquete_nuevo_proceso);
}

void serializar_inicializar_proceso(t_paquete *paquete, int pid_nuevo, char *path_proceso)
{
    int path_length = strlen(path_proceso) + 1;
    int buffer_size = sizeof(int) + sizeof(int) + path_length;

    void *stream = malloc(buffer_size);
    if (stream == NULL)
    {
        return;
    }

    int offset = 0;
    memcpy(stream + offset, &pid_nuevo, sizeof(int));
    offset += sizeof(int);

    memcpy(stream + offset, &path_length, sizeof(int));
    offset += sizeof(int);
    memcpy(stream + offset, path_proceso, path_length);

    t_buffer *buffer = malloc(sizeof(t_buffer));
    if (buffer == NULL)
    {
        free(stream);
        return;
    }

    buffer->size = buffer_size;
    buffer->stream = stream;

    paquete->buffer = buffer;
}

// PLANIFICADOR CORTO PLAZO
void iniciar_planificador_corto_plazo()
{
    log_debug(LOGGER_KERNEL, "Inicia Planificador Corto Plazo");
    pthread_t hilo_corto_plazo;

    pthread_create(&hilo_corto_plazo, NULL, (void *)planificar_PCB_cortoPlazo, NULL);
    pthread_detach(hilo_corto_plazo);
}

void planificar_PCB_cortoPlazo()
{
    while (1)
    {
        sem_wait(&semReady);
        t_pcb *pcb = squeue_pop(squeue_ready);
        log_debug(LOGGER_KERNEL, "Se planifica el PID %d por %s", pcb->pid, ALGORITMO_PLANIFICACION);
        ejecutar_PCB(pcb);
    }
}

void ejecutar_PCB(t_pcb *pcb) {

    cambiar_estado_pcb(pcb, EJECUTANDO);
    squeue_push(squeue_exec, pcb);

    enviar_pcb(pcb, fd_kernel_cpu_dispatch);
    log_debug(LOGGER_KERNEL, "El PCB con PID %d se envio a CPU", pcb->pid);
    pcb_ejecutandose = pcb;

    if (strcmp(ALGORITMO_PLANIFICACION, "RR") == 0)
    {
        pthread_create(&hilo_quantum, NULL, (void *)atender_quantum, NULL);
    }

    pcb = recibir_pcb_CPU(fd_kernel_cpu_dispatch); // Tener en cuenta la funcion para I/O

    if (pcb == NULL) {
        log_error(LOGGER_KERNEL, "Error al recibir PCB de CPU");
        desalojo_cpu(pcb, hilo_quantum);  // Manejar el desalojo en caso de error
        return;
    }

    desalojo_cpu(pcb, hilo_quantum);  // Manejar el desalojo y finalización del hilo de quantum
}

void desalojo_cpu(t_pcb *pcb, pthread_t hilo_quantum_id){

    if (strcmp(ALGORITMO_PLANIFICACION, "RR") == 0){
        pthread_cancel(hilo_quantum_id);
        pthread_join(hilo_quantum_id, NULL);
    }

    squeue_pop(squeue_exec);        

    t_motivo_desalojo *motivo_desalojo = malloc(sizeof(t_motivo_desalojo));
    *motivo_desalojo = pcb->contexto_ejecucion->motivo_desalojo;

    switch (*motivo_desalojo)
    {
    case INTERRUPCION_FIN_QUANTUM:
        log_info(LOGGER_KERNEL, "PID %d - Desalojado por fin de Quantum", pcb->pid);
        cambiar_estado_pcb(pcb, LISTO);
        squeue_push(squeue_ready, pcb);
        sem_post(&semReady);
        break;
    case INTERRUPCION_BLOQUEO:
        log_debug(LOGGER_KERNEL, "PID %d - Desalojado por bloqueo", pcb->pid);
        cambiar_estado_pcb(pcb, BLOQUEADO);
        squeue_push(squeue_blocked, pcb);
        break;
    case INTERRUPCION_FINALIZACION:
        // log_info(LOGGER_KERNEL, "Finaliza el proceso %d - Motivo: %s", pcb->pid, motivo_finalizacion_to_string(pcb->contexto_ejecucion->motivo_finalizacion));
        log_info(LOGGER_KERNEL, "Finaliza el proceso %d - Motivo: %s", pcb->pid, "Finalizacion");
        cambiar_estado_pcb(pcb, FINALIZADO);
        squeue_push(squeue_exit, pcb);
        sem_post(&semMultiprogramacion);

        int sem_value;
        if (sem_getvalue(&semMultiprogramacion, &sem_value) == 0)
        {
            log_trace(LOGGER_KERNEL, "Se libera un espacio de multiprogramacion, semaforo: %d", sem_value);
        }
        break;
    default:
        log_error(LOGGER_KERNEL, "PID %d - Desalojado por motivo desconocido", pcb->pid);
        break;
    }

    free(motivo_desalojo);

}

void atender_quantum(void *arg) {

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    sleep(QUANTUM / 1000);

    t_interrupcion *interrupcion = malloc(sizeof(t_interrupcion));
    interrupcion->motivo_interrupcion = INTERRUPCION_FIN_QUANTUM;
    interrupcion->pid = pcb_ejecutandose->pid;

    enviar_interrupcion(fd_kernel_cpu_interrupt, interrupcion);
    free(interrupcion);
}

t_pcb *recibir_pcb_CPU(int fd_cpu)
{
    op_cod cop;

    recv(fd_cpu, &cop, sizeof(op_cod), 0);

    t_pcb *pcbDeCPU = recibir_pcb(fd_cpu);

    if (pcbDeCPU == NULL)
    {
        log_error(LOGGER_KERNEL, "Error al recibir PCB de CPU");
        return NULL;
    }

    log_debug(LOGGER_KERNEL, "Se recibio el PCB con PID %d de CPU", pcbDeCPU->pid);
    return pcbDeCPU;
}

void interrupcion_quantum(){} // NO SE USA
/*{
    t_interrupcion *interrupcion = malloc(sizeof(t_interrupcion));
    interrupcion->motivo_interrupcion = INTERRUPCION_FIN_QUANTUM;
    interrupcion->pid = -1; // ????
    while (1)
    {
        usleep(QUANTUM * 1000);
        if (strcmp(ALGORITMO_PLANIFICACION, "RR") == 0)
        {
            enviar_interrupcion(fd_kernel_cpu_interrupt, interrupcion);
        }
    }
    free(interrupcion);
}*/

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
    /* if (squeue == squeue_ready)
    {
        log_info(LOGGER_KERNEL, "Ingreso a Cola Ready:");
        mostrar_procesos_en_squeue(squeue);
    } */
    // COLA READY PLUS DEL VRR
    /* if (squeue == squeue_readyPlus) {
        log_info(LOGGER_KERNEL, "Ingreso a Cola Ready+:);
        mostrar_procesos_en_squeue(squeue);
    } */
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
    pthread_mutex_init(&procesoMutex, NULL);
    pthread_mutex_init(&procesosEnSistemaMutex, NULL);
    pthread_mutex_init(&mutex_pid, NULL);

    sem_init(&semMultiprogramacion, 0, GRADO_MULTIPROGRAMACION);
    sem_init(&semNew, 0, 0);
    sem_init(&semReady, 0, 0);
    sem_init(&semExec, 0, 0);
    sem_init(&semDetener, 0, 0);
    sem_init(&semBlocked, 0, 0);
    sem_init(&semFinalizado, 0, 0);
    sem_init(&semExit, 0, 0);

    procesosEnSistema = list_create();

    squeue_new = squeue_create();
    squeue_ready = squeue_create();
    squeue_exec = squeue_create();
    squeue_blocked = squeue_create();
    squeue_exit = squeue_create();
}

uint32_t asignar_pid()
{
    uint32_t valor_pid;

    pthread_mutex_lock(&mutex_pid);
    valor_pid = PID_GLOBAL;
    PID_GLOBAL++;
    pthread_mutex_unlock(&mutex_pid);

    return valor_pid;
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

void mostrar_procesos_en_squeue(t_squeue *squeue)
{
    t_list *temp_list = list_create();

    if (queue_is_empty(squeue->cola))
    {
        log_info(LOGGER_KERNEL, "No hay procesos en la cola");
    }
    else
    {
        while (!queue_is_empty(squeue->cola))
        {
            t_pcb *proceso = squeue_pop(squeue);
            list_add(temp_list, proceso);
        }

        for (int i = 0; i < list_size(temp_list); i++)
        {
            t_pcb *proceso = list_get(temp_list, i);
            log_info(LOGGER_KERNEL, "PID: %d", proceso->pid);
        }
    }
}