#include "../include/planificador.h"

pthread_mutex_t procesoMutex;
pthread_mutex_t procesosEnSistemaMutex;
pthread_mutex_t mutex_pid;
pthread_mutex_t mutex_lista_interfaces;

sem_t semMultiprogramacion;
sem_t semNew;
sem_t semReady;
sem_t semExec;
sem_t semDetener;
sem_t semBlocked;
sem_t semFinalizado;
sem_t semExit;

t_list *procesosEnSistema;
t_list *interfaces_conectadas;

t_squeue *squeue_new;
t_squeue *squeue_ready;
t_squeue *squeue_readyPlus;
t_squeue *squeue_exec;
t_squeue *squeue_blocked;
t_squeue *squeue_exit;

uint32_t PID_GLOBAL = 1;

pthread_t hilo_quantum;

int sem_value; // VARIABLE PARA VERIFICAR EL VALOR DEL SEMAFORO MULTIPROGRAMACION

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

    sem_post(&semNew);
}

void chequear_grado_de_multiprogramacion()
{
    while (1)
    {
        sem_wait(&semNew);

        if (list_size(squeue_new->cola) == 0)
        {
            log_warning(LOGGER_KERNEL, "No hay procesos en NEW");
            continue;
        }

        sem_wait(&semMultiprogramacion);

        int sem_value;
        if (sem_getvalue(&semMultiprogramacion, &sem_value) == 0)
        {
            log_trace(LOGGER_KERNEL, "Valor semaforo multiprogramacion: %d", sem_value);
        }

        t_pcb *pcb_a_mover = squeue_pop(squeue_new);
        cambiar_estado_pcb(pcb_a_mover, LISTO); // previamente tendria que hablar con memoria antes de pasar a ready, tengo que esperar respuesta de que memoria todo en orden
        squeue_push(squeue_ready, pcb_a_mover);
        log_info(LOGGER_KERNEL, "Cola Ready:");
        mostrar_procesos_en_squeue(squeue_ready, LOGGER_KERNEL);
        sem_post(&semReady);
    }
}

void enviar_proceso_a_memoria(int pid_nuevo, char *path_proceso)
{
    t_paquete *paquete_nuevo_proceso = crear_paquete_con_codigo_de_operacion(INICIALIZAR_PROCESO);
    serializar_inicializar_proceso(paquete_nuevo_proceso, pid_nuevo, path_proceso);
    enviar_paquete(paquete_nuevo_proceso, fd_kernel_memoria);
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
        if (list_size(squeue_ready->cola) == 0)
        {
            log_debug(LOGGER_KERNEL, "No hay procesos en Ready");
            continue;
        }
        t_pcb *pcb = squeue_pop(squeue_ready);
        ejecutar_PCB(pcb);
    }
}

void ejecutar_PCB(t_pcb *pcb)
{
    cambiar_estado_pcb(pcb, EJECUTANDO);
    squeue_push(squeue_exec, pcb);

    enviar_pcb(pcb, fd_kernel_cpu_dispatch);

    pcb_ejecutandose = pcb;

    if (strcmp(ALGORITMO_PLANIFICACION, "RR") == 0)
    {
        pthread_create(&hilo_quantum, NULL, (void *)atender_quantum, NULL);
    }

    pcb = recibir_pcb_CPU(fd_kernel_cpu_dispatch); // Tener en cuenta la funcion para I/O

    if (pcb == NULL)
    {
        log_error(LOGGER_KERNEL, "Error al recibir PCB de CPU");
        desalojo_cpu(pcb, hilo_quantum); // Manejar el desalojo en caso de error
        return;
    }

    desalojo_cpu(pcb, hilo_quantum); // Manejar el desalojo y finalización del hilo de quantum
}

t_pcb *recibir_pcb_CPU(int fd_cpu)
{
    op_cod cop;
    t_pcb *pcb_recibido;

    recv(fd_cpu, &cop, sizeof(op_cod), 0);

    switch (cop)
    {
    case PCB:
        pcb_recibido = recibir_pcb(fd_cpu);
        break;

    case ENVIAR_INTERFAZ:
        pcb_recibido = recibir_pcb_para_interfaz(fd_cpu, &nombre_interfaz, &unidades_de_trabajo, &instruccion_de_IO_a_ejecutar);
        break;

    default:
        log_error(LOGGER_KERNEL, "No se pudo recibir el pcb");
        break;
    }

    if (pcb_recibido == NULL)
    {
        log_error(LOGGER_KERNEL, "Error al recibir PCB de CPU");
        return NULL;
    }

    return pcb_recibido;
}

void desalojo_cpu(t_pcb *pcb, pthread_t hilo_quantum_id)
{
    if (strcmp(ALGORITMO_PLANIFICACION, "RR") == 0)
    {
        pthread_cancel(hilo_quantum_id);
        pthread_join(hilo_quantum_id, NULL);
    }

    squeue_pop(squeue_exec);

    t_motivo_desalojo *motivo_desalojo = malloc(sizeof(t_motivo_desalojo));
    *motivo_desalojo = pcb->contexto_ejecucion->motivo_desalojo;

    switch (*motivo_desalojo)
    {
    case INTERRUPCION_FIN_QUANTUM:
        log_info(LOGGER_KERNEL, "PID: %d - Desalojado por fin de Quantum", pcb->pid);
        cambiar_estado_pcb(pcb, LISTO);
        squeue_push(squeue_ready, pcb);
        log_info(LOGGER_KERNEL, "Cola Ready:");
        mostrar_procesos_en_squeue(squeue_ready, LOGGER_KERNEL);
        sem_post(&semReady);
        break;
    case INTERRUPCION_BLOQUEO:
        log_debug(LOGGER_KERNEL, "PID %d - Desalojado por bloqueo", pcb->pid);
        ejecutar_intruccion_io(pcb);
        sem_post(&semBlocked);
        break;
    case FINALIZACION:
        finalizar_proceso(pcb);
        break;
    case INTERRUPCION_FINALIZACION:
        pcb->contexto_ejecucion->motivo_finalizacion = INTERRUPTED_BY_USER;
        finalizar_proceso(pcb);

        if (sem_getvalue(&semMultiprogramacion, &sem_value) == 0)
        {
            log_trace(LOGGER_KERNEL, "Se libera un espacio de multiprogramacion, semaforo: %d", sem_value);
        }
        break;
    default:
        log_error(LOGGER_KERNEL, "PID: %d - Desalojado por motivo desconocido", pcb->pid);
        break;
    }

    free(motivo_desalojo);
}

void atender_quantum(void *arg)
{
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    sleep(QUANTUM / 1000);

    t_interrupcion *interrupcion = malloc(sizeof(t_interrupcion));
    interrupcion->motivo_interrupcion = INTERRUPCION_FIN_QUANTUM;
    interrupcion->pid = pcb_ejecutandose->pid;

    enviar_interrupcion(fd_kernel_cpu_interrupt, interrupcion);
    free(interrupcion);
}

// OTRAS FUNCIONES
void iniciar_colas_y_semaforos()
{
    pthread_mutex_init(&procesoMutex, NULL);
    pthread_mutex_init(&procesosEnSistemaMutex, NULL);
    pthread_mutex_init(&mutex_pid, NULL);
    pthread_mutex_init(&mutex_lista_interfaces, NULL);

    sem_init(&semMultiprogramacion, 0, GRADO_MULTIPROGRAMACION);
    sem_init(&semNew, 0, 0);
    sem_init(&semReady, 0, 0);
    sem_init(&semExec, 0, 0);
    sem_init(&semDetener, 0, 0);
    sem_init(&semBlocked, 0, 0);
    sem_init(&semFinalizado, 0, 0);
    sem_init(&semExit, 0, 0);

    procesosEnSistema = list_create();
    interfaces_conectadas = list_create();

    squeue_new = squeue_create();
    squeue_ready = squeue_create();
    squeue_readyPlus = squeue_create();
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

void finalizar_proceso(t_pcb *pcb)
{
    log_info(LOGGER_KERNEL, "Finaliza el proceso %d - Motivo: %s", pcb->pid, motivo_finalizacion_to_string(pcb->contexto_ejecucion->motivo_finalizacion));
    cambiar_estado_pcb(pcb, FINALIZADO);
    squeue_push(squeue_exit, pcb);
    // liberar_estructuras_memoria(pcb->pid); // TODO: Liberar estructuras de memoria
    sem_post(&semMultiprogramacion);
    if (sem_getvalue(&semMultiprogramacion, &sem_value) == 0)
    {
        log_trace(LOGGER_KERNEL, "Se libera un espacio de multiprogramacion, semaforo: %d", sem_value);
    }
}

void bloquear_proceso(t_pcb *pcb, char *motivo)
{
    cambiar_estado_pcb(pcb, BLOQUEADO);
    squeue_push(squeue_blocked, pcb);
    log_info(LOGGER_KERNEL, "PID %d - Bloqueado por: %s", pcb->pid, motivo);
}

void desbloquear_proceso(uint32_t pid)
{
    bool comparar_pid(void *elemento)
    {
        return ((t_pcb *)elemento)->pid == pid;
    }

    t_pcb *pcb = squeue_remove_by_condition(squeue_blocked, (void *)comparar_pid);

    if (pcb)
    {
        cambiar_estado_pcb(pcb, LISTO);
        squeue_push(squeue_ready, pcb);
        log_info(LOGGER_KERNEL, "Cola Ready:");
        mostrar_procesos_en_squeue(squeue_ready, LOGGER_KERNEL);
        sem_post(&semReady);
    }
}

void ejecutar_intruccion_io(t_pcb *pcb_recibido)
{
    t_interfaz_recibida *interfaz_a_utilizar = buscar_interfaz_por_nombre(nombre_interfaz);

    if (interfaz_a_utilizar)
    {
        switch (interfaz_a_utilizar->tipo_interfaz_recibida)
        {
        case GENERICA:
            if (instruccion_de_IO_a_ejecutar == IO_GEN_SLEEP)
            {
                bloquear_proceso(pcb_recibido, interfaz_a_utilizar->nombre_interfaz_recibida);
                squeue_push(interfaz_a_utilizar->cola_procesos_bloqueados, pcb_recibido);
                enviar_InterfazGenerica(interfaz_a_utilizar->socket_interfaz_recibida, unidades_de_trabajo, pcb_recibido->pid, interfaz_a_utilizar->nombre_interfaz_recibida);
            }
            else
            {
                log_error(LOGGER_KERNEL, "Instruccion %s no reconocida", nombre_instruccion_to_string(instruccion_de_IO_a_ejecutar));
                finalizar_proceso(pcb_recibido);
            }
            break;

        case STDIN:
            if (instruccion_de_IO_a_ejecutar == IO_STDIN_READ)
            {
                bloquear_proceso(pcb_recibido, interfaz_a_utilizar->nombre_interfaz_recibida);
                squeue_push(interfaz_a_utilizar->cola_procesos_bloqueados, pcb_recibido);
                // Enviar interfaz STDIN
            }
            else
            {
                log_error(LOGGER_KERNEL, "Instruccion %s no reconocida", nombre_instruccion_to_string(instruccion_de_IO_a_ejecutar));
                finalizar_proceso(pcb_recibido);
            }
            break;

        case STDOUT:
            if (instruccion_de_IO_a_ejecutar == IO_STDOUT_WRITE)
            {
                bloquear_proceso(pcb_recibido, interfaz_a_utilizar->nombre_interfaz_recibida);
                squeue_push(interfaz_a_utilizar->cola_procesos_bloqueados, pcb_recibido);
                // Enviar interfaz STDOUT
            }
            else
            {
                log_error(LOGGER_KERNEL, "Instruccion %s no reconocida", nombre_instruccion_to_string(instruccion_de_IO_a_ejecutar));
                finalizar_proceso(pcb_recibido);
            }
            break;

        case DIALFS:
            if (instruccion_de_IO_a_ejecutar == IO_FS_CREATE || instruccion_de_IO_a_ejecutar == IO_FS_DELETE ||
                instruccion_de_IO_a_ejecutar == IO_FS_TRUNCATE || instruccion_de_IO_a_ejecutar == IO_FS_WRITE ||
                instruccion_de_IO_a_ejecutar == IO_FS_READ)
            {
                bloquear_proceso(pcb_recibido, interfaz_a_utilizar->nombre_interfaz_recibida);
                squeue_push(interfaz_a_utilizar->cola_procesos_bloqueados, pcb_recibido);
                // Enviar interfaz DIALFS
            }
            else
            {
                log_error(LOGGER_KERNEL, "Instruccion %s no reconocida", nombre_instruccion_to_string(instruccion_de_IO_a_ejecutar));
                finalizar_proceso(pcb_recibido);
            }
            break;

        default:
            log_error(LOGGER_KERNEL, "Tipo de interfaz no reconocido");
            break;
        }
    }
    else
    {
        log_error(LOGGER_KERNEL, "No se encontro la interfaz %s", nombre_interfaz);
        pcb_recibido->contexto_ejecucion->motivo_finalizacion = INVALID_INTERFACE;
        finalizar_proceso(pcb_recibido);
    }
}

t_interfaz_recibida *buscar_interfaz_por_nombre(char *nombre_interfaz)
{
    bool comparar_nombre_interfaz(void *elemento)
    {
        return strcmp(((t_interfaz_recibida *)elemento)->nombre_interfaz_recibida, nombre_interfaz) == 0;
    }

    pthread_mutex_lock(&mutex_lista_interfaces);
    t_interfaz_recibida *interfaz = list_find(interfaces_conectadas, (void *)comparar_nombre_interfaz);
    pthread_mutex_unlock(&mutex_lista_interfaces);

    return interfaz;
}
