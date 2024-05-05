#include "../include/pcb.h"

// Inicializar Registros

void inicializar_contexto_y_registros(t_pcb *pcb) {
    pcb->contexto_ejecucion = malloc(sizeof(t_contexto_ejecucion));
    pcb->contexto_ejecucion->registros = malloc(sizeof(t_registros));
    memset(pcb->contexto_ejecucion->registros, 0, sizeof(t_registros));
    pcb->contexto_ejecucion->motivo_desalojo = SIN_MOTIVO;
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
    free(pcb->contexto_ejecucion->registros);
    free(pcb->contexto_ejecucion);
    free(pcb);
}

// Funciones Serializacion

t_buffer *crear_buffer_pcb(t_pcb *pcb) {

    t_buffer *buffer = malloc(sizeof(t_buffer));

    buffer->size = sizeof(uint32_t) +
                   sizeof(t_estado_proceso) +
                   sizeof(int) +
                   sizeof(t_registros) +
                   sizeof(t_motivo_desalojo);

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

    memcpy(buffer->stream + desplazamiento, &(pcb->contexto_ejecucion->motivo_desalojo), sizeof(t_motivo_desalojo));
    desplazamiento += sizeof(t_motivo_desalojo);

    return buffer;
}

t_paquete *crear_paquete_PCB(t_pcb *pcb){
    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->codigo_operacion = PCB;
    paquete->buffer = crear_buffer_pcb(pcb);
    return paquete;
}

void agregar_a_paquetePCB(t_paquete *paquete, t_pcb *pcb){
    
    int desplazamiento = 0;

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->pid), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->estado), sizeof(t_estado_proceso));
    desplazamiento += sizeof(t_estado_proceso);

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->quantum), sizeof(int));
    desplazamiento += sizeof(int);

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->contexto_ejecucion->registros->program_counter), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->contexto_ejecucion->registros->ax), sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->contexto_ejecucion->registros->bx), sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->contexto_ejecucion->registros->cx), sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->contexto_ejecucion->registros->dx), sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->contexto_ejecucion->registros->eax), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->contexto_ejecucion->registros->ebx), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->contexto_ejecucion->registros->ecx), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->contexto_ejecucion->registros->edx), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->contexto_ejecucion->registros->si), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->contexto_ejecucion->registros->di), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->contexto_ejecucion->motivo_desalojo), sizeof(t_motivo_desalojo));
    desplazamiento += sizeof(t_motivo_desalojo);
}

t_pcb *deserializar_pcb(t_buffer *buffer) {
    t_pcb *pcb = malloc(sizeof(t_pcb));
    void *stream = buffer->stream;
    int desplazamiento = 0;

    memcpy(&(pcb->pid), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(pcb->estado), stream + desplazamiento, sizeof(t_estado_proceso));
    desplazamiento += sizeof(t_estado_proceso);

    memcpy(&(pcb->quantum), stream + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    pcb->contexto_ejecucion = malloc(sizeof(t_contexto_ejecucion));
    pcb->contexto_ejecucion->registros = malloc(sizeof(t_registros));

    memcpy(pcb->contexto_ejecucion->registros, stream + desplazamiento, sizeof(t_registros));
    desplazamiento += sizeof(t_registros);

    memcpy(&(pcb->contexto_ejecucion->motivo_desalojo), stream + desplazamiento, sizeof(t_motivo_desalojo));
    desplazamiento += sizeof(t_motivo_desalojo);

    return pcb;
}

// Funciones de Envio y Recepcion

void enviar_pcb(t_pcb *pcb, int socket_cliente)
{
    t_paquete *paquete = crear_paquete_PCB(pcb);
    agregar_a_paquetePCB(paquete, pcb);
    enviar_paquete(paquete, socket_cliente);
    eliminar_paquete(paquete);
}

t_pcb *recibir_pcb(int socket_cliente)
{
    t_paquete *paquete = recibir_paqueteTOP(socket_cliente);
    t_pcb *pcb = deserializar_pcb(paquete->buffer);
    eliminar_paquete(paquete);
    return pcb;
}
