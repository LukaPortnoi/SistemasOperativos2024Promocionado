#include "../include/pcb.h"

// Inicializar Registros

void inicializar_contexto_y_registros(t_pcb *pcb)
{
    pcb->contexto_ejecucion->registros->program_counter = 0;
    pcb->contexto_ejecucion->registros->ax = 0;
    pcb->contexto_ejecucion->registros->bx = 0;
    pcb->contexto_ejecucion->registros->cx = 0;
    pcb->contexto_ejecucion->registros->dx = 0;
    pcb->contexto_ejecucion->registros->eax = 0;
    pcb->contexto_ejecucion->registros->ebx = 0;
    pcb->contexto_ejecucion->registros->ecx = 0;
    pcb->contexto_ejecucion->registros->edx = 0;
    pcb->contexto_ejecucion->registros->si = 0;
    pcb->contexto_ejecucion->registros->di = 0;
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
    free(pcb);
}

// Funciones Serializacion

t_buffer *crear_buffer_pcb(t_pcb *pcb1)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));

    buffer->size = sizeof(uint32_t)
                 + sizeof(t_estado_proceso)
                 + sizeof(int)
                 + sizeof(uint32_t)
                 + 4 * sizeof(uint8_t)
                 + 6 * sizeof(uint32_t)+
                 sizeof(t_motivo_desalojo);

    int desplazamiento = 0;
    buffer->stream = malloc(buffer->size);

    memcpy(buffer->stream + desplazamiento, &(pcb1->pid), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, &(pcb1->estado), sizeof(t_estado_proceso));
    desplazamiento += sizeof(t_estado_proceso);

    memcpy(buffer->stream + desplazamiento, &(pcb1->quantum), sizeof(int));
    desplazamiento += sizeof(int);

    memcpy(buffer->stream + desplazamiento, &(pcb1->contexto_ejecucion->registros->program_counter), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, &(pcb1->contexto_ejecucion->registros->ax), sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

    memcpy(buffer->stream + desplazamiento, &(pcb1->contexto_ejecucion->registros->bx), sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

    memcpy(buffer->stream + desplazamiento, &(pcb1->contexto_ejecucion->registros->cx), sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

    memcpy(buffer->stream + desplazamiento, &(pcb1->contexto_ejecucion->registros->dx), sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

    memcpy(buffer->stream + desplazamiento, &(pcb1->contexto_ejecucion->registros->eax), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, &(pcb1->contexto_ejecucion->registros->ebx), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, &(pcb1->contexto_ejecucion->registros->ecx), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, &(pcb1->contexto_ejecucion->registros->edx), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, &(pcb1->contexto_ejecucion->registros->si), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, &(pcb1->contexto_ejecucion->registros->di), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, &(pcb1->contexto_ejecucion->motivo_desalojo), sizeof(t_motivo_desalojo));
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
    
    // Armamos el stream a enviar
    void *a_enviar = malloc(paquete->buffer->size);
}

t_pcb *deserializar_pcb(t_paquete *paquete) // Hacerlo d nuevo con la estructura de t_pcb
{
    
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
    t_pcb *pcb = deserializar_pcb(paquete);
    eliminar_paquete(paquete);
    return pcb;
}
