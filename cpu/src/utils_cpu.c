#include "../include/utils_cpu.h"

void ejecutar_ciclo_instruccion(int socket)
{
    t_instruccion *instruccion = fetch(pcb_actual->pid, pcb_actual->contexto_ejecucion->registros->program_counter);
    // TODO decode: manejo de TLB y MMU
    execute(instruccion, socket);
    // if (!page_fault)
}

t_instruccion *fetch(uint32_t pid, uint32_t pc)
{
    // TODO -- chequear que en los casos de instruccion con memoria logica puede dar PAGE FAULT y no hay que aumentar el pc (restarlo dentro del decode en esos casos)
    // log_trace(LOGGER_CPU, "PID Y PC PARA PEDIR INSTRUCCION A MEMORIA: %d - %d\n", pid, pc);
    pedir_instruccion_memoria(pid, pc, fd_cpu_memoria);

    op_cod codigo_op = recibir_operacion(fd_cpu_memoria);

    t_instruccion *instruccion;

    if (codigo_op == INSTRUCCION)
    {
        instruccion = deserializar_instruccion(fd_cpu_memoria);
    }
    else
    {
        log_warning(LOGGER_CPU, "Operación desconocida. No se pudo recibir la instruccion de memoria.");
        exit(EXIT_FAILURE);
    }

    log_info(LOGGER_CPU, "PID: %d - FETCH - Program Counter: %d", pid, pc);

    return instruccion;
}

void execute(t_instruccion *instruccion, int socket)
{
    switch (instruccion->nombre)
    {
    case SET:
        _set(instruccion->parametro1, instruccion->parametro2);
        loguear_y_sumar_pc(instruccion);
        break;
    case SUM:
        _sum(instruccion->parametro1, instruccion->parametro2);
        loguear_y_sumar_pc(instruccion);
        break;
    case SUB:
        _sub(instruccion->parametro1, instruccion->parametro2);
        loguear_y_sumar_pc(instruccion);
        break;
    case JNZ:
        _jnz(instruccion->parametro1, instruccion->parametro2);
        loguear_y_sumar_pc(instruccion);
        break;
    case WAIT:
        loguear_y_sumar_pc(instruccion);
        pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_SYSCALL;
        esSyscall = true;
        _wait(instruccion->parametro1, socket);
        break;
    case SIGNAL:
        loguear_y_sumar_pc(instruccion);
        pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_SYSCALL;
        esSyscall = true;
        _signal(instruccion->parametro1, socket);
        break;
    case RESIZE:
        _resize(instruccion->parametro1);
        // esperar_respuesta_resize();
        loguear_y_sumar_pc(instruccion);
        break;
    case MOV_IN:
        _mov_in(instruccion->parametro1, instruccion->parametro2);
        loguear_y_sumar_pc(instruccion);
        break;
    case MOV_OUT:
        _mov_out(instruccion->parametro1, instruccion->parametro2);
        loguear_y_sumar_pc(instruccion);
        break;
    case IO_GEN_SLEEP:
        loguear_y_sumar_pc(instruccion);
        pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_BLOQUEO;
        esSyscall = true;
        _io_gen_sleep(instruccion->parametro1, instruccion->parametro2, socket);
        break;
    case IO_FS_READ:
        loguear_y_sumar_pc(instruccion);
        pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_BLOQUEO;
        esSyscall = true;
        _io_stdin_read(instruccion->parametro1, instruccion->parametro2 , instruccion->parametro3, socket);
        break;
    case EXIT:
        log_info(LOGGER_CPU, "PID: %d - Ejecutando: %s", pcb_actual->pid, instruccion_to_string(instruccion->nombre));
        esSyscall = true;
        pcb_actual->contexto_ejecucion->motivo_desalojo = FINALIZACION;
        pcb_actual->contexto_ejecucion->motivo_finalizacion = SUCCESS;
        break;
    default:
        break;
    }
}

void loguear_y_sumar_pc(t_instruccion *instruccion)
{
    log_instruccion_ejecutada(instruccion->nombre, instruccion->parametro1, instruccion->parametro2);
    pcb_actual->contexto_ejecucion->registros->program_counter++;
}

void pedir_instruccion_memoria(uint32_t pid, uint32_t pc, int socket)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(PEDIDO_INSTRUCCION);
    paquete->buffer->size += sizeof(uint32_t) * 2;
    paquete->buffer->stream = malloc(paquete->buffer->size);
    memcpy(paquete->buffer->stream, &(pid), sizeof(uint32_t));
    memcpy(paquete->buffer->stream + sizeof(uint32_t), &(pc), sizeof(int));
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}

t_instruccion *deserializar_instruccion(int socket)
{
    t_paquete *paquete = recibir_paquete(socket);
    t_instruccion *instruccion = malloc(sizeof(t_instruccion));

    void *stream = paquete->buffer->stream;
    int desplazamiento = 0;

    memcpy(&(instruccion->nombre), stream + desplazamiento, sizeof(nombre_instruccion));
    desplazamiento += sizeof(nombre_instruccion);

    uint32_t tamanio_parametro1;                                              // Cambio aquí
    memcpy(&(tamanio_parametro1), stream + desplazamiento, sizeof(uint32_t)); // Cambio aquí
    desplazamiento += sizeof(uint32_t);

    uint32_t tamanio_parametro2;                                              // Cambio aquí
    memcpy(&(tamanio_parametro2), stream + desplazamiento, sizeof(uint32_t)); // Cambio aquí
    desplazamiento += sizeof(uint32_t);

    instruccion->parametro1 = malloc(tamanio_parametro1);
    memcpy(instruccion->parametro1, stream + desplazamiento, tamanio_parametro1);
    desplazamiento += tamanio_parametro1;

    instruccion->parametro2 = malloc(tamanio_parametro2);
    memcpy(instruccion->parametro2, stream + desplazamiento, tamanio_parametro2);

    eliminar_paquete(paquete);

    return instruccion;
}

void log_instruccion_ejecutada(nombre_instruccion nombre, char *param1, char *param2)
{
    char *nombre_instruccion = instruccion_to_string(nombre);
    log_info(LOGGER_CPU, "PID: %d - Ejecutando: %s - Parametros: %s - %s", pcb_actual->pid, nombre_instruccion, param1, param2);
}

void iniciar_semaforos_etc()
{
    pthread_mutex_init(&mutex_interrupt, NULL);
}