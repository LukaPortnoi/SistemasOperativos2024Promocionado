#include "../include/IO.h"

void enviar_interfaz_IO(t_pcb *pcb_actual, char *interfaz, uint32_t *unidades_de_trabajo, int socket_cliente)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(ENVIAR_INTERFAZ);
    serializar_IO_instruccion(paquete, pcb_actual, unidades_de_trabajo, interfaz);
    enviar_paquete(paquete, socket_cliente);
    eliminar_paquete(paquete);
}

void serializar_IO_instruccion(t_paquete *paquete, t_pcb *pcb, uint32_t *unidades_de_trabajo, char *interfaz)
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
        printf("null");
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
        printf("null");
    }

    buffer->size = buffer_size;
    buffer->stream = stream;

    paquete->buffer = buffer;
}


void recibir_interfaz(int socket_cliente, t_pcb *pcb_a_interfaz, char *nombre_interfaz, uint32_t *unidades_de_trabajo)
{
    t_paquete *paquete = recibir_paquete(socket_cliente);
    deserializar_interfaz(paquete->buffer, pcb_a_interfaz, nombre_interfaz, unidades_de_trabajo);
    eliminar_paquete(paquete);
}

void deserializar_interfaz(t_buffer *buffer, t_pcb *pcb_a_interfaz, char *nombre_interfaz, uint32_t *unidades_de_trabajo)
{
    pcb_a_interfaz = malloc(sizeof(t_pcb));
    unidades_de_trabajo = malloc(sizeof(uint32_t));
     if (pcb_a_interfaz == NULL)
    {
        printf("null el pcb");
    } 
    

    uint32_t long_interfaz;
    void *stream = buffer->stream;
    int desplazamiento = 0;

    memcpy(&(unidades_de_trabajo), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(long_interfaz), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    nombre_interfaz = malloc(long_interfaz);

    memcpy(nombre_interfaz, stream + desplazamiento, long_interfaz);
    desplazamiento += long_interfaz;

     memcpy(&(pcb_a_interfaz->pid), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(pcb_a_interfaz->estado), stream + desplazamiento, sizeof(t_estado_proceso));
    desplazamiento += sizeof(t_estado_proceso);

    memcpy(&(pcb_a_interfaz->quantum), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    pcb_a_interfaz->contexto_ejecucion = malloc(sizeof(t_contexto_ejecucion));
    if (pcb_a_interfaz->contexto_ejecucion == NULL)
    {
        free(pcb_a_interfaz);
        printf("null el pcb");
    }

    pcb_a_interfaz->contexto_ejecucion->registros = malloc(sizeof(t_registros));
    if (pcb_a_interfaz->contexto_ejecucion->registros == NULL)
    {
        free(pcb_a_interfaz->contexto_ejecucion);
        free(pcb_a_interfaz);
        printf("null el pcb");
    }

    memcpy(pcb_a_interfaz->contexto_ejecucion->registros, stream + desplazamiento, sizeof(t_registros));
    desplazamiento += sizeof(t_registros);

    memcpy(&(pcb_a_interfaz->contexto_ejecucion->motivo_desalojo), stream + desplazamiento, sizeof(t_motivo_desalojo));

}