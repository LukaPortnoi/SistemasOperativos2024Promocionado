#include "../include/pcb.h"

// ------------------------------------------------------ Inicializar Registros

void inicializar_registros(t_pcb* pcb)
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

t_pcb* crear_pcb(u_int32_t pid, t_estado_proceso estado, int quantum)
{
    t_pcb* pcb = malloc(sizeof(t_pcb));
    pcb->pid = pid;
    pcb->estado = estado;
    pcb->quantum = quantum;
    inicializar_registros(pcb);

    return pcb;
}

void destruir_pcb(t_pcb* pcb)
{
    free(pcb);
}

// ------------------------------------------------------ Funciones Serializacion

void serializar_pcb(t_pcb* pcb, t_paquete* paquete)
{
    agregar_a_paquete(paquete, &(pcb->pid), sizeof(u_int32_t));
    agregar_a_paquete(paquete, &(pcb->estado), sizeof(t_estado_proceso));
    agregar_a_paquete(paquete, &(pcb->registros.program_counter), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(pcb->registros.ax), sizeof(uint8_t));
    agregar_a_paquete(paquete, &(pcb->registros.bx), sizeof(uint8_t));
    agregar_a_paquete(paquete, &(pcb->registros.cx), sizeof(uint8_t));
    agregar_a_paquete(paquete, &(pcb->registros.dx), sizeof(uint8_t));
    agregar_a_paquete(paquete, &(pcb->registros.eax), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(pcb->registros.ebx), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(pcb->registros.ecx), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(pcb->registros.edx), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(pcb->registros.si), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(pcb->registros.di), sizeof(uint32_t));
    agregar_a_paquete(paquete, &(pcb->quantum), sizeof(int));
}

t_pcb* deserializar_pcb(t_paquete* paquete)
{
    t_pcb* pcb = malloc(sizeof(t_pcb));
    int offset = 0;

    memcpy(&(pcb->pid), paquete->buffer->stream + offset, sizeof(u_int32_t));
    offset += sizeof(u_int32_t);
    memcpy(&(pcb->estado), paquete->buffer->stream + offset, sizeof(t_estado_proceso));
    offset += sizeof(t_estado_proceso);
    memcpy(&(pcb->registros.program_counter), paquete->buffer->stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(&(pcb->registros.ax), paquete->buffer->stream + offset, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(&(pcb->registros.bx), paquete->buffer->stream + offset, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(&(pcb->registros.cx), paquete->buffer->stream + offset, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(&(pcb->registros.dx), paquete->buffer->stream + offset, sizeof(uint8_t));
    offset += sizeof(uint8_t);
    memcpy(&(pcb->registros.eax), paquete->buffer->stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(&(pcb->registros.ebx), paquete->buffer->stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(&(pcb->registros.ecx), paquete->buffer->stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(&(pcb->registros.edx), paquete->buffer->stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(&(pcb->registros.si), paquete->buffer->stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(&(pcb->registros.di), paquete->buffer->stream + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(&(pcb->quantum), paquete->buffer->stream + offset, sizeof(int));

    return pcb;
}

// ------------------------------------------------------ Funciones de Envio y Recepcion

void enviar_pcb(t_pcb* pcb, int socket_cliente)
{
    t_paquete* paquete = crear_paquete();
    paquete->codigo_operacion = PCB;
    serializar_pcb(pcb, paquete);
    enviar_paquete(paquete, socket_cliente);
    eliminar_paquete(paquete);
}

t_pcb* recibir_pcb(int socket_cliente)
{
    t_paquete* paquete = recibir_paquete(socket_cliente);
    t_pcb* pcb = deserializar_pcb(paquete);
    eliminar_paquete(paquete);

    return pcb;
}

