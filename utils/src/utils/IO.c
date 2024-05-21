#include "../include/IO.h"

void enviar_interfaz_IO(t_pcb *pcb_actual, char *interfaz, int unidades_de_trabajo, int socket_cliente)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(ENVIAR_INTERFAZ);
    serializar_IO_instruccion(paquete, pcb_actual, unidades_de_trabajo, interfaz);
    enviar_paquete(paquete, socket_cliente);
    eliminar_paquete(paquete);
}

void serializar_IO_instruccion(t_paquete *paquete, t_pcb *pcb, uint32_t unidades_de_trabajo, char *interfaz)
{
    uint32_t interfaz_length = strlen(interfaz) + 1;

    size_t tam_registros = sizeof(uint32_t) +
                           sizeof(uint8_t) * 4 +
                           sizeof(uint32_t) * 6;

    int buffer_size = sizeof(uint32_t) +
                      sizeof(uint32_t) +
                      interfaz_length +
                      sizeof(uint32_t) +
                      sizeof(t_estado_proceso) +
                      sizeof(uint32_t) +
                      tam_registros +
                      sizeof(t_motivo_desalojo);

    void *stream = malloc(buffer_size);
    if (stream == NULL)
    {
        return;
    }

    int offset = 0;
    memcpy(stream + offset, &unidades_de_trabajo, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(stream + offset, &interfaz_length, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(stream + offset, interfaz, interfaz_length);
    offset += interfaz_length;

    memcpy(stream + offset, &(pcb->pid), sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(stream + offset, &(pcb->estado), sizeof(t_estado_proceso));
    offset += sizeof(t_estado_proceso);

    memcpy(stream + offset, &(pcb->quantum), sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(stream + offset, pcb->contexto_ejecucion->registros, tam_registros);
    offset += tam_registros;

    memcpy(stream + offset, &(pcb->contexto_ejecucion->motivo_desalojo), sizeof(t_motivo_desalojo));

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
