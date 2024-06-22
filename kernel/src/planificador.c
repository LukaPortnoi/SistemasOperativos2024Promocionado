#include "../include/planificador.h"

pthread_mutex_t procesoMutex;
pthread_mutex_t procesosEnSistemaMutex;
pthread_mutex_t mutex_pid;
pthread_mutex_t mutex_lista_interfaces;
pthread_mutex_t MUTEX_RECURSO;

sem_t sem_planificador_largo_plazo;
sem_t sem_planificador_corto_plazo;
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
bool PLANIFICACION_DETENIDA = false;
pthread_t hilo_quantum;
t_temporal *temporizador;
int sem_value;

// PLANIFICADOR LARGO PLAZO
void iniciar_planificador_largo_plazo()
{
    pthread_t hilo_largo_plazo;
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
        if (PLANIFICACION_DETENIDA)
        {
            sem_wait(&sem_planificador_largo_plazo);
        }

        int largo_plazo;
        sem_getvalue(&sem_planificador_largo_plazo, &largo_plazo);
        log_trace(LOGGER_KERNEL, "Valor semaforo LARGO plazo en su hilo: %d", largo_plazo);

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
        proceso_listo(pcb_a_mover, false);
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

    paquete->buffer->size = buffer_size;
    paquete->buffer->stream = stream;
}

// PLANIFICADOR CORTO PLAZO
void iniciar_planificador_corto_plazo()
{
    pthread_t hilo_corto_plazo;
    pthread_create(&hilo_corto_plazo, NULL, (void *)planificar_PCB_cortoPlazo, NULL);
    pthread_detach(hilo_corto_plazo);
}

void planificar_PCB_cortoPlazo()
{
    while (1)
    {
        sem_wait(&semReady);
        if (PLANIFICACION_DETENIDA)
        {
            sem_wait(&sem_planificador_corto_plazo);
        }

        t_pcb *pcb;

        if (strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0 && list_size(squeue_readyPlus->cola) > 0)
        {
            pcb = squeue_pop(squeue_readyPlus);
        }
        else
        {
            if (list_size(squeue_ready->cola) == 0)
            {
                log_debug(LOGGER_KERNEL, "No hay procesos en Ready");
                sem_post(&sem_planificador_corto_plazo);
                continue;
            }
            pcb = squeue_pop(squeue_ready);
        }

        ejecutar_PCB(pcb);
        pcb_ejecutandose = NULL;
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

    if (strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0)
    {
        temporizador = temporal_create();
        pthread_create(&hilo_quantum, NULL, (void *)atender_quantum, NULL);
    }

    recibir_pcb_CPU(pcb, fd_kernel_cpu_dispatch);

    if (pcb == NULL)
    {
        log_error(LOGGER_KERNEL, "Error al recibir PCB de CPU");
        desalojo_cpu(pcb, hilo_quantum);
        return;
    }

    desalojo_cpu(pcb, hilo_quantum);
}

void recibir_pcb_CPU(t_pcb *pcb_recibido, int fd_cpu)
{
    op_cod cop;

    recv(fd_cpu, &cop, sizeof(op_cod), 0);

    switch (cop)
    {
    case PCB:
        recibir_pcb(pcb_recibido, fd_cpu);
        break;

    case PEDIDO_WAIT:
        recibir_pcb_para_manejo_recurso(pcb_recibido, fd_cpu, &RECURSO_A_USAR);
        INSTRUCCION_RECURSO_A_USAR = WAIT;
        break;

    case PEDIDO_SIGNAL:
        recibir_pcb_para_manejo_recurso(pcb_recibido, fd_cpu, &RECURSO_A_USAR);
        INSTRUCCION_RECURSO_A_USAR = SIGNAL;
        break;

    case ENVIAR_INTERFAZ:
        recibir_pcb_para_interfaz(pcb_recibido, fd_cpu, &nombre_interfaz, &unidades_de_trabajo, &instruccion_de_IO_a_ejecutar);
        break;

    case ENVIAR_INTERFAZ_STDIN:
        recibir_pcb_para_interfaz_in_out(pcb_recibido, fd_cpu, &nombre_interfaz, direcciones_fisicas, &instruccion_de_IO_a_ejecutar);
        break;

    case ENVIAR_INTERFAZ_STDOUT:
        recibir_pcb_para_interfaz_in_out(pcb_recibido, fd_cpu, &nombre_interfaz, direcciones_fisicas, &instruccion_de_IO_a_ejecutar);
        break;

    case FS_CREATE_DELETE:
        recibir_pcb_fs_create_delete(pcb_recibido, fd_cpu, &nombre_interfaz, &nombre_archivo, &instruccion_de_IO_a_ejecutar); 
        break;

    default:
        log_error(LOGGER_KERNEL, "No se pudo recibir el pcb");
        break;
    }

    if (pcb_recibido == NULL)
    {
        log_error(LOGGER_KERNEL, "Error al recibir PCB de CPU");
    }
}

void desalojo_cpu(t_pcb *pcb, pthread_t hilo_quantum_id)
{
    if (strcmp(ALGORITMO_PLANIFICACION, "RR") == 0)
    {
        pthread_cancel(hilo_quantum_id);
        pthread_join(hilo_quantum_id, NULL);
    }

    if (strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0)
    {
        pthread_cancel(hilo_quantum_id);
        pthread_join(hilo_quantum_id, NULL);
        temporal_stop(temporizador);
        pcb->tiempo_q = temporal_gettime(temporizador);
        temporal_destroy(temporizador);
    }

    squeue_pop(squeue_exec);

    t_motivo_desalojo *motivo_desalojo = malloc(sizeof(t_motivo_desalojo));
    *motivo_desalojo = pcb->contexto_ejecucion->motivo_desalojo;

    switch (*motivo_desalojo)
    {
    case INTERRUPCION_FIN_QUANTUM:
        log_info(LOGGER_KERNEL, "PID: %d - Desalojado por fin de Quantum", pcb->pid);
        pcb->quantum = QUANTUM;
        proceso_listo(pcb, false);
        break;
    case INTERRUPCION_SYSCALL: // MANEJO RECURSO
        log_debug(LOGGER_KERNEL, "PID %d - Desalojado por manejo de recurso", pcb->pid);
        manejar_recurso(pcb, RECURSO_A_USAR); // ojo ver si hay que hacer un free
        break;
    case INTERRUPCION_BLOQUEO:
        log_debug(LOGGER_KERNEL, "PID %d - Desalojado por instruccion IO", pcb->pid);
        ejecutar_intruccion_io(pcb);
        break;
    case FINALIZACION:
        finalizar_proceso(pcb);
        break;
    case INTERRUPCION_FINALIZACION:
        pcb->contexto_ejecucion->motivo_finalizacion = INTERRUPTED_BY_USER;
        finalizar_proceso(pcb);
        break;
    case INTERRUPCION_OUT_OF_MEMORY:
        log_debug(LOGGER_KERNEL, "PID: %d - Desalojado por OUT OF MEMORY", pcb->pid);
        finalizar_proceso(pcb);
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

    usleep(pcb_ejecutandose->quantum * 1000);

    crear_y_enviar_interrupcion(INTERRUPCION_FIN_QUANTUM, pcb_ejecutandose->pid);
}

void crear_y_enviar_interrupcion(t_motivo_desalojo mot_interrupcion, uint32_t pid)
{
    t_interrupcion *interrupcion = malloc(sizeof(t_interrupcion));
    interrupcion->motivo_interrupcion = mot_interrupcion;
    interrupcion->pid = pid;

    enviar_interrupcion(fd_kernel_cpu_interrupt, interrupcion);
    free(interrupcion);
}

// OTRAS FUNCIONES
void detener_planificadores()
{
    int largo_plazo;
    int corto_plazo;

    sem_getvalue(&sem_planificador_largo_plazo, &largo_plazo);
    sem_getvalue(&sem_planificador_corto_plazo, &corto_plazo);

    while (largo_plazo > 0)
    {
        sem_wait(&sem_planificador_largo_plazo);
        sem_getvalue(&sem_planificador_largo_plazo, &largo_plazo);
    }

    while (corto_plazo > 0)
    {
        sem_wait(&sem_planificador_corto_plazo);
        sem_getvalue(&sem_planificador_corto_plazo, &corto_plazo);
    }

    PLANIFICACION_DETENIDA = true;

    sem_getvalue(&sem_planificador_largo_plazo, &largo_plazo);
    sem_getvalue(&sem_planificador_corto_plazo, &corto_plazo);
    log_trace(LOGGER_KERNEL, "Valor semaforo LARGO plazo: %d", largo_plazo);
    log_trace(LOGGER_KERNEL, "Valor semaforo CORTO plazo: %d", corto_plazo);

    log_debug(LOGGER_KERNEL, "Planificacion detenida");
}

void iniciar_planificadores()
{
    int largo_plazo;
    int corto_plazo;
    sem_getvalue(&sem_planificador_largo_plazo, &largo_plazo);
    sem_getvalue(&sem_planificador_corto_plazo, &corto_plazo);

    if (largo_plazo == 0 && corto_plazo == 0)
    {
        sem_post(&sem_planificador_largo_plazo);
        sem_post(&sem_planificador_corto_plazo);
    }

    PLANIFICACION_DETENIDA = false;

    sem_getvalue(&sem_planificador_largo_plazo, &largo_plazo);
    sem_getvalue(&sem_planificador_corto_plazo, &corto_plazo);
    log_trace(LOGGER_KERNEL, "Valor semaforo LARGO plazo: %d", largo_plazo);
    log_trace(LOGGER_KERNEL, "Valor semaforo CORTO plazo: %d", corto_plazo);

    log_debug(LOGGER_KERNEL, "Planificacion iniciada");
}

void iniciar_colas_y_semaforos()
{
    pthread_mutex_init(&procesoMutex, NULL);
    pthread_mutex_init(&procesosEnSistemaMutex, NULL);
    pthread_mutex_init(&mutex_pid, NULL);
    pthread_mutex_init(&mutex_lista_interfaces, NULL);
    pthread_mutex_init(&MUTEX_RECURSO, NULL);

    sem_init(&sem_planificador_largo_plazo, 0, 1);
    sem_init(&sem_planificador_corto_plazo, 0, 1);
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
    RECURSOS_DISPONIBLES = list_create();
    direcciones_fisicas = list_create();

    inicializar_recursos();

    squeue_new = squeue_create();
    squeue_ready = squeue_create();
    squeue_readyPlus = squeue_create();
    squeue_exec = squeue_create();
    squeue_blocked = squeue_create();
    squeue_exit = squeue_create();
}

void inicializar_recursos()
{
    int i = 0;
    while (RECURSOS[i] != NULL)
    {
        t_recurso *recurso = malloc(sizeof(t_recurso));
        recurso->nombre_recurso = RECURSOS[i];
        recurso->instancias = atoi(INSTANCIAS_RECURSOS[i]);
        recurso->cola_procesos_bloqueados = squeue_create();
        list_add(RECURSOS_DISPONIBLES, recurso);
        i++;
    }
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

void proceso_listo(t_pcb *pcb, bool es_ready_plus)
{
    // proceso_ready_memoria(pcb); // TODO: previamente tendria que hablar con memoria antes de pasar a ready, tengo que esperar respuesta de que memoria todo en orden
    cambiar_estado_pcb(pcb, LISTO);
    if (es_ready_plus)
    {
        squeue_push(squeue_readyPlus, pcb);
        loguear_cola(squeue_readyPlus, "Ready Prioridad");
    }
    else
    {
        squeue_push(squeue_ready, pcb);
        loguear_cola(squeue_ready, "Ready");
    }
    sem_post(&semReady);
}

void loguear_cola(t_squeue *squeue, const char *mensaje)
{
    pthread_mutex_lock(squeue->mutex);

    char log_message[1024];
    snprintf(log_message, sizeof(log_message), "Cola %s: [", mensaje);

    int list_size_ready = list_size(squeue->cola);
    for (int i = 0; i < list_size_ready; i++)
    {
        t_pcb *proceso = list_get(squeue->cola, i);
        char pid_str[12];
        snprintf(pid_str, sizeof(pid_str), "%d", proceso->pid);

        strcat(log_message, pid_str);
        if (i < list_size_ready - 1)
        {
            strcat(log_message, ", ");
        }
    }

    strcat(log_message, "]");
    pthread_mutex_unlock(squeue->mutex);

    log_info(LOGGER_KERNEL, "%s", log_message);
}

void finalizar_proceso(t_pcb *pcb)
{
    log_info(LOGGER_KERNEL, "Finaliza el proceso %d - Motivo: %s", pcb->pid, motivo_finalizacion_to_string(pcb->contexto_ejecucion->motivo_finalizacion));
    cambiar_estado_pcb(pcb, FINALIZADO);
    squeue_push(squeue_exit, pcb);
    liberar_recursos(pcb);
    liberar_estructuras_memoria(pcb->pid);
    sem_post(&semMultiprogramacion);
    if (sem_getvalue(&semMultiprogramacion, &sem_value) == 0)
    {
        log_trace(LOGGER_KERNEL, "Se libera un espacio de multiprogramacion, semaforo: %d", sem_value);
    }
}

void liberar_estructuras_memoria(uint32_t pid)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(FINALIZAR_PROCESO);

    paquete->buffer->size = sizeof(uint32_t);
    paquete->buffer->stream = malloc(sizeof(uint32_t));

    int desplazamiento = 0;

    memcpy(paquete->buffer->stream + desplazamiento, &pid, sizeof(uint32_t));

    enviar_paquete(paquete, fd_kernel_memoria);
    eliminar_paquete(paquete);
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
        if (strcmp(ALGORITMO_PLANIFICACION, "VRR") == 0)
        {
            if (pcb->tiempo_q < pcb->quantum)
            {
                pcb->quantum -= pcb->tiempo_q;
                proceso_listo(pcb, true);
            }
            else
            {
                proceso_listo(pcb, false);
            }
        }
        else
        {
            proceso_listo(pcb, false);
        }
    }
}

void recibir_pcb_para_manejo_recurso(t_pcb *pcb, int socket, char **recurso)
{
    t_paquete *paquete = recibir_paquete(socket);
    deserializar_pcb_recurso(pcb, paquete->buffer, recurso);
    eliminar_paquete(paquete);
}

void deserializar_pcb_recurso(t_pcb *pcb, t_buffer *buffer, char **recurso)
{
    uint32_t long_recurso;
    void *stream = buffer->stream;
    int desplazamiento = 0;

    memcpy(&(pcb->pid), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(pcb->estado), stream + desplazamiento, sizeof(t_estado_proceso));
    desplazamiento += sizeof(t_estado_proceso);

    memcpy(&(pcb->quantum), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(pcb->tiempo_q), stream + desplazamiento, sizeof(uint64_t));
    desplazamiento += sizeof(uint64_t);

    memcpy(pcb->contexto_ejecucion->registros, stream + desplazamiento, sizeof(t_registros));
    desplazamiento += sizeof(t_registros);

    memcpy(&(pcb->contexto_ejecucion->motivo_desalojo), stream + desplazamiento, sizeof(t_motivo_desalojo));
    desplazamiento += sizeof(t_motivo_desalojo);

    memcpy(&(pcb->contexto_ejecucion->motivo_finalizacion), stream + desplazamiento, sizeof(t_motivo_finalizacion));
    desplazamiento += sizeof(t_motivo_finalizacion);

    memcpy(&long_recurso, stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    *recurso = malloc(long_recurso);

    memcpy(*recurso, stream + desplazamiento, long_recurso);
}

void manejar_recurso(t_pcb *pcb, char *recurso)
{
    t_recurso *recurso_a_utilizar = encontrar_recurso(recurso);
    log_trace(LOGGER_KERNEL, "PID %d - Recurso %s - Instruccion %s", pcb->pid, recurso_a_utilizar->nombre_recurso, nombre_instruccion_to_string(INSTRUCCION_RECURSO_A_USAR));
    if (recurso_a_utilizar)
    {
        if (pcb->recursos_asignados == NULL)
        {
            log_error(LOGGER_KERNEL, "PID %d - Recursos asignados no inicializados", pcb->pid);
        }

        switch (INSTRUCCION_RECURSO_A_USAR)
        {
        case WAIT:
            if (recurso_a_utilizar->instancias > 0)
            {
                asignar_recurso(pcb, recurso_a_utilizar);
                proceso_listo(pcb, false);
            }
            else
            {
                bloquear_proceso(pcb, recurso);
                squeue_push(recurso_a_utilizar->cola_procesos_bloqueados, pcb);
            }
            break;
        case SIGNAL:
            liberar_recurso(pcb, recurso_a_utilizar);
            proceso_listo(pcb, false);
            break;
        default:
            log_error(LOGGER_KERNEL, "Instruccion %s no reconocida", nombre_instruccion_to_string(INSTRUCCION_RECURSO_A_USAR));
            break;
        }
    }
    else
    {
        log_error(LOGGER_KERNEL, "PID %d - Recurso %s no existe", pcb->pid, recurso);
        pcb->contexto_ejecucion->motivo_finalizacion = INVALID_RESOURCE;
        finalizar_proceso(pcb);
    }
}

void asignar_recurso(t_pcb *pcb, t_recurso *recurso)
{
    pthread_mutex_lock(&MUTEX_RECURSO);
    recurso->instancias--;
    list_add(pcb->recursos_asignados, recurso);
    pthread_mutex_unlock(&MUTEX_RECURSO);
}

void liberar_recurso(t_pcb *pcb, t_recurso *recurso)
{
    bool es_el_recurso(void *recurso_en_lista)
    {
        return (t_recurso *)recurso_en_lista == recurso;
    }

    pthread_mutex_lock(&MUTEX_RECURSO);
    recurso->instancias++;
    list_remove_by_condition(pcb->recursos_asignados, es_el_recurso);
    pthread_mutex_unlock(&MUTEX_RECURSO);

    if (recurso->instancias > 0 && !list_is_empty(recurso->cola_procesos_bloqueados->cola))
    {
        t_pcb *pcb_desbloquear = squeue_pop(recurso->cola_procesos_bloqueados);
        desbloquear_proceso(pcb_desbloquear->pid);
    }
}

void liberar_recursos(t_pcb *pcb)
{
    if (list_size(pcb->recursos_asignados) > 0)
    {
        for (int i = 0; i < list_size(pcb->recursos_asignados); i++)
        {
            liberar_recurso(pcb, list_get(pcb->recursos_asignados, i));
        }
    }
}

t_recurso *encontrar_recurso(char *recurso)
{
    bool _es_el_recurso(t_recurso * r)
    {
        return strcmp(r->nombre_recurso, recurso) == 0;
    }

    return list_find(RECURSOS_DISPONIBLES, (void *)_es_el_recurso);
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
                bloquear_procesosIO(pcb_recibido, interfaz_a_utilizar);
                enviar_InterfazGenerica(interfaz_a_utilizar->socket_interfaz_recibida, unidades_de_trabajo, pcb_recibido->pid, interfaz_a_utilizar->nombre_interfaz_recibida);
                free(nombre_interfaz);
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
                bloquear_procesosIO(pcb_recibido, interfaz_a_utilizar);
                enviar_InterfazStdin(interfaz_a_utilizar->socket_interfaz_recibida, direcciones_fisicas, pcb_recibido->pid, interfaz_a_utilizar->nombre_interfaz_recibida);
                list_clean_and_destroy_elements(direcciones_fisicas, free);
                free(nombre_interfaz);
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
                bloquear_procesosIO(pcb_recibido, interfaz_a_utilizar);
                enviar_InterfazStdout(interfaz_a_utilizar->socket_interfaz_recibida, direcciones_fisicas, pcb_recibido->pid, interfaz_a_utilizar->nombre_interfaz_recibida);
                list_clean_and_destroy_elements(direcciones_fisicas, free);
                free(nombre_interfaz);
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
                bloquear_procesosIO(pcb_recibido, interfaz_a_utilizar);
                enviar_InterfazDialFS(interfaz_a_utilizar->socket_interfaz_recibida, pcb_recibido->pid, interfaz_a_utilizar->nombre_interfaz_recibida, instruccion_de_IO_a_ejecutar);
                free(nombre_interfaz);
                free(nombre_archivo);
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

void bloquear_procesosIO(t_pcb *pcbAbloquear, t_interfaz_recibida *interfaz_a_utilizar)
{
    bloquear_proceso(pcbAbloquear, interfaz_a_utilizar->nombre_interfaz_recibida);
    squeue_push(interfaz_a_utilizar->cola_procesos_bloqueados, pcbAbloquear);
}

void recibir_pcb_fs_create_delete(t_pcb *pcb, int socket, char **nombre_interfaz, char **nombre_archivo, nombre_instruccion *instruccion)
{
    t_paquete *paquete = recibir_paquete(socket);
    deserializar_pcb_fs_create_delete(pcb, paquete->buffer, nombre_interfaz, nombre_archivo, instruccion);
    eliminar_paquete(paquete);
}

void deserializar_pcb_fs_create_delete(t_pcb *pcb, t_buffer *buffer, char **nombre_interfaz, char **nombre_archivo, nombre_instruccion *instruccion)
{
    uint32_t long_nombre_interfaz;
    uint32_t long_nombre_archivo;
    void *stream = buffer->stream;
    int desplazamiento = 0;

    memcpy(&(pcb->pid), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(pcb->estado), stream + desplazamiento, sizeof(t_estado_proceso));
    desplazamiento += sizeof(t_estado_proceso);

    memcpy(&(pcb->quantum), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(pcb->tiempo_q), stream + desplazamiento, sizeof(uint64_t));
    desplazamiento += sizeof(uint64_t);

    memcpy(pcb->contexto_ejecucion->registros, stream + desplazamiento, sizeof(t_registros));
    desplazamiento += sizeof(t_registros);

    memcpy(&(pcb->contexto_ejecucion->motivo_desalojo), stream + desplazamiento, sizeof(t_motivo_desalojo));
    desplazamiento += sizeof(t_motivo_desalojo);

    memcpy(&(pcb->contexto_ejecucion->motivo_finalizacion), stream + desplazamiento, sizeof(t_motivo_finalizacion));
    desplazamiento += sizeof(t_motivo_finalizacion);

    memcpy(&long_nombre_interfaz, stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    *nombre_interfaz = malloc(long_nombre_interfaz);

    memcpy(*nombre_interfaz, stream + desplazamiento, long_nombre_interfaz);
    desplazamiento += long_nombre_interfaz;

    memcpy(&long_nombre_archivo, stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    *nombre_archivo = malloc(long_nombre_archivo);

    memcpy(*nombre_archivo, stream + desplazamiento, long_nombre_archivo);
    desplazamiento += long_nombre_archivo;

    memcpy(instruccion, stream + desplazamiento, sizeof(nombre_instruccion));
}

void enviar_InterfazDialFS(int socket, uint32_t pid, char *nombre_interfaz, nombre_instruccion instruccion)
{
    switch (instruccion)
    {
    case IO_FS_CREATE:
        enviar_interfaz_dialFS_create_delete(socket, nombre_archivo, pid, nombre_interfaz, instruccion);
        break;
    
    case IO_FS_DELETE:
        enviar_interfaz_dialFS_create_delete(socket, nombre_archivo, pid, nombre_interfaz, instruccion);
        break;
    /*
    case IO_FS_TRUNCATE:
        enviar_interfaz_dialFS_truncate(socket, nombre_archivo, pid, nombre_interfaz);
        break;
    
    case IO_FS_WRITE:
        enviar_interfaz_dialFS_write(socket, nombre_archivo, pid, nombre_interfaz);
        break;

    case IO_FS_READ:
        enviar_interfaz_dialFS_read(socket, nombre_archivo, pid, nombre_interfaz);
        break;*/

    default:
        log_error(LOGGER_KERNEL, "Instruccion %s no reconocida", nombre_instruccion_to_string(instruccion));
        break;
    }
}