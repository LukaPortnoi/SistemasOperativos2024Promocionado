#include "../include/pcb.h"

// Inicializar Registros

void inicializar_contexto_y_registros(t_pcb *pcb)
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

// Funciones PCB

t_pcb *crear_pcb(int pid, t_estado_proceso estado, int quantum)
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
    free(pcb);
}

// Funciones Serializacion

t_buffer *crear_buffer_pcb(t_pcb *pcb)
{
    buffer->size = sizeof(uint32_t) + sizeof(t_estado_proceso) + sizeof(int) + sizeof(t_contexto_ejecucion)
                   + sizeof(t_registros) + sizeof(t_instruccion)

    buffer->stream = malloc(buffer->size);

    int desplazamiento = 0;

    memcpy(buffer->stream + desplazamiento, &(pcb->pid), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, &(pcb->estado), sizeof(t_estado_proceso));
    desplazamiento += sizeof(t_estado_proceso);

    memcpy(buffer->stream + desplazamiento, &(pcb->quantum), sizeof(int));
    desplazamiento += sizeof(int);

    memcpy(buffer->stream + desplazamiento, pcb->contexto_ejecucion->registros, sizeof(t_registros));
    desplazamiento += sizeof(t_registros);
    
    return buffer;
}

t_pcb *deserializar_pcb(t_paquete *paquete) //Hacerlo d nuevo con la estructura de t_pcb
{
    

    void *stream = paquete->buffer->stream;
    int presize = paquete->buffer->size;

    memcpy(&(pcb->pid), stream, sizeof(int));
    stream += sizeof(int);
    presize -= sizeof(int);
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

// Funciones de Envio y Recepcion

void enviar_pcb(t_pcb *pcb, int socket_cliente)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(PCB);
    agregar_a_paquete(paquete, pcb, sizeof(t_pcb));
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