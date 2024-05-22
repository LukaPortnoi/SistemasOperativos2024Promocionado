#include "../include/pcb.h"

// Inicializar Registros

void inicializar_contexto_y_registros(t_pcb *pcb)
{
    pcb->contexto_ejecucion = malloc(sizeof(t_contexto_ejecucion));
    pcb->contexto_ejecucion->registros = malloc(sizeof(t_registros));
    memset(pcb->contexto_ejecucion->registros, 0, sizeof(t_registros));
    pcb->contexto_ejecucion->motivo_desalojo = SIN_MOTIVO;
}

// Funciones PCB

t_pcb *crear_pcb(uint32_t pid, t_estado_proceso estado, uint32_t quantum)
{
    t_pcb *pcb = malloc(sizeof(t_pcb));
    pcb->pid = pid;
    pcb->estado = estado;
    pcb->quantum = quantum;
    inicializar_contexto_y_registros(pcb);
    return pcb;
}

void destruir_pcb(t_pcb *pcb)
{
    free(pcb->contexto_ejecucion->registros);
    free(pcb->contexto_ejecucion);
    free(pcb);
}

// Funciones Serializacion

t_buffer *crear_buffer_pcb(t_pcb *pcb)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));

    size_t tam_registros = sizeof(uint32_t) +
                           sizeof(uint8_t) * 4 +
                           sizeof(uint32_t) * 6;

    buffer->size = sizeof(uint32_t) +
                   sizeof(t_estado_proceso) +
                   sizeof(uint32_t) +
                   tam_registros +
                   sizeof(t_motivo_desalojo);

    buffer->stream = malloc(buffer->size);
    int desplazamiento = 0;

    memcpy(buffer->stream + desplazamiento, &(pcb->pid), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, &(pcb->estado), sizeof(t_estado_proceso));
    desplazamiento += sizeof(t_estado_proceso);

    memcpy(buffer->stream + desplazamiento, &(pcb->quantum), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, pcb->contexto_ejecucion->registros, tam_registros);
    desplazamiento += tam_registros;

    memcpy(buffer->stream + desplazamiento, &(pcb->contexto_ejecucion->motivo_desalojo), sizeof(t_motivo_desalojo));

    return buffer;
}

t_paquete *crear_paquete_PCB(t_pcb *pcb)
{
    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->codigo_operacion = PCB;
    paquete->buffer = crear_buffer_pcb(pcb);
    return paquete;
}

t_pcb *deserializar_pcb(t_buffer *buffer)
{
    t_pcb *pcb = malloc(sizeof(t_pcb));
    if (pcb == NULL)
    {
        return NULL;
    }

    void *stream = buffer->stream;
    int desplazamiento = 0;

    memcpy(&(pcb->pid), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(pcb->estado), stream + desplazamiento, sizeof(t_estado_proceso));
    desplazamiento += sizeof(t_estado_proceso);

    memcpy(&(pcb->quantum), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    pcb->contexto_ejecucion = malloc(sizeof(t_contexto_ejecucion));
    if (pcb->contexto_ejecucion == NULL)
    {
        free(pcb);
        return NULL;
    }

    pcb->contexto_ejecucion->registros = malloc(sizeof(t_registros));
    if (pcb->contexto_ejecucion->registros == NULL)
    {
        free(pcb->contexto_ejecucion);
        free(pcb);
        return NULL;
    }

    memcpy(pcb->contexto_ejecucion->registros, stream + desplazamiento, sizeof(t_registros));
    desplazamiento += sizeof(t_registros);

    memcpy(&(pcb->contexto_ejecucion->motivo_desalojo), stream + desplazamiento, sizeof(t_motivo_desalojo));

    return pcb;
}

// Funciones de Envio y Recepcion

void enviar_pcb(t_pcb *pcb, int socket_cliente)
{
    t_paquete *paquete = crear_paquete_PCB(pcb);
    enviar_paquete(paquete, socket_cliente);
    eliminar_paquete(paquete);
}

t_pcb *recibir_pcb(int socket_cliente)
{
    t_paquete *paquete = recibir_paquete(socket_cliente);
    t_pcb *pcb = deserializar_pcb(paquete->buffer);
    eliminar_paquete(paquete);
    return pcb;
}

uint32_t str_to_uint32(char *str)
{
    char *endptr;
    uint32_t result = (uint32_t)strtoul(str, &endptr, 10);

    // Comprobar si hubo errores durante la conversión
    if (*endptr != '\0')
    {
        fprintf(stderr, "Error en la conversión de '%s' a uint32_t.\n", str);
        exit(EXIT_FAILURE);
    }

    return result;
}

uint8_t str_to_uint8(char *str)
{
    char *endptr;
    uint8_t result = (uint8_t)strtoul(str, &endptr, 10);

    // Comprobar si hubo errores durante la conversión
    if (*endptr != '\0')
    {
        fprintf(stderr, "Error en la conversión de '%s' a uint8_t.\n", str);
        exit(EXIT_FAILURE);
    }

    return result;
}

t_pcb *recibir_pcbTOP(int socket_cliente)
{
    int size;
    void *buffer;

    buffer = recibir_bufferTOP(socket_cliente, &size);

    // vamos a printear el buffer recibido

    t_pcb *pcb = malloc(sizeof(t_pcb));
    int desplazamiento = 0;

    memcpy(&(pcb->pid), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(pcb->estado), buffer + desplazamiento, sizeof(t_estado_proceso));
    desplazamiento += sizeof(t_estado_proceso);

    memcpy(&(pcb->quantum), buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    pcb->contexto_ejecucion = malloc(sizeof(t_contexto_ejecucion));
    pcb->contexto_ejecucion->registros = malloc(sizeof(t_registros));

    memcpy(pcb->contexto_ejecucion->registros, buffer + desplazamiento, sizeof(t_registros));
    desplazamiento += sizeof(t_registros);

    memcpy(&(pcb->contexto_ejecucion->motivo_desalojo), buffer + desplazamiento, sizeof(t_motivo_desalojo));

    free(buffer);

    return pcb;
}

void *recibir_bufferTOP(int socket_cliente, int *size)
{
    void *buffer;

    recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
    buffer = malloc(*size);
    recv(socket_cliente, buffer, *size, MSG_WAITALL);

    return buffer;
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
    default:
        return "ERROR";
    }
}