#include "../include/pcb.h"

// ------------------------------------------------------ Inicializar Registros

void inicializar_registros(t_pcb *pcb)
{
    pcb->registros.program_counter = 0;
    pcb->registros.ax = 0;
    pcb->registros.bx = 0;
    pcb->registros.cx = 0;
    pcb->registros.dx = 0;
    pcb->registros.eax = 0;
    pcb->registros.ebx = 0;
    pcb->registros.ecx = 0;
    pcb->registros.edx = 0;
    pcb->registros.si = 0;
    pcb->registros.di = 0;
}

// ------------------------------------------------------ Funciones PCB

t_pcb *crear_pcb(uint32_t pid, t_estado_proceso estado, int quantum)
{
    t_pcb *pcb = malloc(sizeof(t_pcb));
    pcb->pid = pid;
    pcb->estado = estado;
    pcb->quantum = quantum;
    inicializar_registros(pcb);
    return pcb;
}

void destruir_pcb(t_pcb *pcb)
{
    free(pcb);
}

// ------------------------------------------------------ Funciones Serializacion

t_buffer *crear_buffer_pcb(t_pcb *pcb)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));
    buffer->size = sizeof(u_int32_t) + sizeof(t_estado_proceso) + sizeof(uint32_t) + 4 * sizeof(uint8_t) + 4 * sizeof(uint32_t) + 2 * sizeof(int);

    void *stream = malloc(buffer->size);
    
    int offset = 0;
    memcpy(stream + offset, &(pcb->pid), sizeof(u_int32_t));
    offset += sizeof(u_int32_t);
    memcpy(stream + offset, &(pcb->estado), sizeof(t_estado_proceso));
    offset += sizeof(t_estado_proceso);
    memcpy(stream + offset, &(pcb->registros.program_counter), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &(pcb->registros.ax), sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(stream + offset, &(pcb->registros.bx), sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(stream + offset, &(pcb->registros.cx), sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(stream + offset, &(pcb->registros.dx), sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(stream + offset, &(pcb->registros.eax), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &(pcb->registros.ebx), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &(pcb->registros.ecx), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &(pcb->registros.edx), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &(pcb->registros.si), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &(pcb->registros.di), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(stream + offset, &(pcb->quantum), sizeof(int));

    buffer->stream = stream;

    return buffer;
}

t_pcb *deserializar_pcb(t_paquete *paquete)
{
    t_pcb *pcb = malloc(sizeof(t_pcb));

    void *stream = paquete->buffer->stream;
    int presize  = paquete->buffer->size;

    memcpy(&(pcb->pid), stream, sizeof(u_int32_t));
    stream += sizeof(u_int32_t);
    presize -= sizeof(u_int32_t);
    memcpy(&(pcb->estado), stream, sizeof(t_estado_proceso));
    stream += sizeof(t_estado_proceso);
    presize -= sizeof(t_estado_proceso);
    memcpy(&(pcb->registros.program_counter), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    presize -= sizeof(uint32_t);
    memcpy(&(pcb->registros.ax), stream, sizeof(uint8_t));
    stream += sizeof(uint8_t);
    presize -= sizeof(uint8_t);
    memcpy(&(pcb->registros.bx), stream, sizeof(uint8_t));
    stream += sizeof(uint8_t);
    presize -= sizeof(uint8_t);
    memcpy(&(pcb->registros.cx), stream, sizeof(uint8_t));
    stream += sizeof(uint8_t);
    presize -= sizeof(uint8_t);
    memcpy(&(pcb->registros.dx), stream, sizeof(uint8_t));
    stream += sizeof(uint8_t);
    presize -= sizeof(uint8_t);
    memcpy(&(pcb->registros.eax), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    presize -= sizeof(uint32_t);
    memcpy(&(pcb->registros.ebx), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    presize -= sizeof(uint32_t);
    memcpy(&(pcb->registros.ecx), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    presize -= sizeof(uint32_t);
    memcpy(&(pcb->registros.edx), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    presize -= sizeof(uint32_t);
    memcpy(&(pcb->registros.si), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    presize -= sizeof(uint32_t);
    memcpy(&(pcb->registros.di), stream, sizeof(uint32_t));
    stream += sizeof(uint32_t);
    presize -= sizeof(uint32_t);
    memcpy(&(pcb->quantum), stream, sizeof(int));
    stream += sizeof(int);
    presize -= sizeof(int);

    memcpy(paquete->buffer->stream, stream, presize);
    paquete->buffer->size = presize;
    
    return pcb;
}

// ------------------------------------------------------ Funciones de Envio y Recepcion

void enviar_pcb(t_pcb *pcb, int socket_cliente)
{
    t_buffer *buffer = crear_buffer_pcb(pcb);
    t_paquete *paquete = crear_paquete(buffer, PCB);
    enviar_paquete(paquete, socket_cliente);
    eliminar_paquete(paquete);
}

t_pcb *recibir_pcb(int socket_cliente)
{
    t_paquete *paquete = recibir_paqueteTOP(socket_cliente);
    t_pcb *pcb = deserializar_pcb(paquete);
    eliminar_paquete(paquete);

    return pcb;
}
