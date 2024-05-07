#include "../include/utils_cpu.h"

void ejecutar_ciclo_instruccion()
{
    t_instruccion *instruccion = fetch(pcb_actual->pid, pcb_actual->contexto_ejecucion->registros->program_counter);
    // TODO decode: manejo de TLB y MMU
    execute(instruccion);
    // if (!page_fault)
    pcb_actual->contexto_ejecucion->registros->program_counter++;
}

t_instruccion *fetch(int pid, int pc)
{
    // TODO -- chequear que en los casos de instruccion con memoria logica puede dar PAGE FAULT y no hay que aumentar el pc (restarlo dentro del decode en esos casos)

    pedir_instruccion_memoria(pid, pc, fd_cpu_memoria);

    op_cod codigo_op = recibir_operacion(fd_cpu_memoria);

    t_instruccion *instruccion;

    if (codigo_op == INSTRUCCION)
    {
        instruccion = deserializar_instruccion(fd_cpu_memoria);
        // pcb_actual->contexto_ejecucion-> = instruccion;
    }
    else
    {
        log_warning(LOGGER_CPU, "Operación desconocida. No se pudo recibir la instruccion de memoria.");
        abort();
    }

    log_info(LOGGER_CPU, "PID: %d - FETCH - Program Counter: %d", pid, pc);

    return instruccion;
}

void execute(t_instruccion *instruccion)
{
    char *param1 = string_new();
    char *param2 = string_new();
    if (instruccion->parametro1 != NULL)
    {
        strcpy(param1, instruccion->parametro1);
    }
    if (instruccion->parametro2 != NULL)
    {
        strcpy(param2, instruccion->parametro2);
    }
    // log_info(cpu_logger_info, "PID: %d - Ejecutando: %s - Parametros: %s - %s", contexto_actual->pid, cod_inst_to_str(instruccion->codigo), param1, param2);

    switch (instruccion->nombre)
    {
    case SET:
        _set(instruccion->parametro1, instruccion->parametro2);
        break;
    case SUM:
        _sum(instruccion->parametro1, instruccion->parametro2);
        break;
    case SUB:
        _sub(instruccion->parametro1, instruccion->parametro2);
        break;
    case JNZ:
        _jnz(instruccion->parametro1, instruccion->parametro2);
        break;
        // case IO_GEN_SLEEP:
        //    _io_gen_sleep(instruccion->parametro1, instruccion->parametro2);
        //   break;
    default:
        break;
    }
}

void pedir_instruccion_memoria(int pid, int pc, int socket)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(PEDIDO_INSTRUCCION);
    paquete->buffer->size += sizeof(int) * 2;
    paquete->buffer->stream = malloc(paquete->buffer->size);
    memcpy(paquete->buffer->stream, &(pid), sizeof(int));
    memcpy(paquete->buffer->stream + sizeof(int), &(pc), sizeof(int));
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

    uint32_t tamanio_parametro1;                                            // Cambio aquí
    memcpy(&tamanio_parametro1, stream + desplazamiento, sizeof(uint32_t)); // Cambio aquí
    desplazamiento += sizeof(uint32_t);

    uint32_t tamanio_parametro2;                                            // Cambio aquí
    memcpy(&tamanio_parametro2, stream + desplazamiento, sizeof(uint32_t)); // Cambio aquí
    desplazamiento += sizeof(uint32_t);

    instruccion->parametro1 = malloc(tamanio_parametro1);
    memcpy(instruccion->parametro1, stream + desplazamiento, tamanio_parametro1);
    desplazamiento += tamanio_parametro1;

    instruccion->parametro2 = malloc(tamanio_parametro2);
    memcpy(instruccion->parametro2, stream + desplazamiento, tamanio_parametro2);
    desplazamiento += tamanio_parametro2;

    eliminar_paquete(paquete);

    return instruccion;
}