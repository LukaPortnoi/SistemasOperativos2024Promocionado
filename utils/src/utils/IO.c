#include "../include/IO.h"

void enviar_interfaz_IO(t_pcb *pcb_actual, char *interfaz, int unidades_de_trabajo, int socket_cliente)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(ENVIAR_INTERFAZ);
    serializar_IO_instruccion(paquete, pcb_actual, unidades_de_trabajo, interfaz);
    enviar_paquete(paquete, socket_cliente);
    eliminar_paquete(paquete);
}

void serializar_IO_instruccion(t_paquete *paquete, t_pcb *pcb, int unidades_de_trabajo, char *interfaz)
{
    int interfaz_length = strlen(interfaz) + 1;

    size_t tam_registros = sizeof(uint32_t) +
                           sizeof(uint8_t) * 4 +
                           sizeof(uint32_t) * 6;

    int buffer_size = sizeof(int) +
                      sizeof(int) +
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

    memcpy(stream + offset, &(pcb->pid), sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(stream + offset, &(pcb->estado), sizeof(t_estado_proceso));
    offset += sizeof(t_estado_proceso);

    memcpy(stream + offset, &(pcb->quantum), sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(stream + offset, pcb->contexto_ejecucion->registros, tam_registros);
    offset += tam_registros;

    memcpy(stream + offset, &(pcb->contexto_ejecucion->motivo_desalojo), sizeof(t_motivo_desalojo));
    offset += sizeof(t_motivo_desalojo);

    memcpy(stream + offset, &unidades_de_trabajo, sizeof(uint32_t));
    offset += sizeof(int);

    memcpy(stream + offset, &interfaz_length, sizeof(uint32_t));
    offset += sizeof(int);

    memcpy(stream + offset, interfaz, interfaz_length);

    
    t_buffer *buffer = malloc(sizeof(t_buffer));
    if (buffer == NULL)
    {
        free(stream);
        printf("null");
    }

    buffer->size = buffer_size;
    buffer->stream = stream;
    printf("EL TAMNIO DEL BUFFER ES: %d\n", buffer_size);


    paquete->buffer = buffer;
}


t_pcb *recibir_interfaz_cpu(int socket_cliente, char *nombre_interfaz, int unidades_de_trabajo)
{
    t_paquete *paquete = recibir_paquete(socket_cliente);
    t_pcb *pcb =  deserializar_interfaz(paquete->buffer, nombre_interfaz, unidades_de_trabajo);
    eliminar_paquete(paquete);
    return pcb;
}

t_pcb *deserializar_interfaz(t_buffer *buffer, char *nombre_interfaz, int unidades_de_trabajo)
{
    

    t_pcb *pcb_a_interfazRecibido = malloc(sizeof(t_pcb));
     if (pcb_a_interfazRecibido == NULL)
    {
        printf("null el pcb");
                return NULL;

    } 
    

    uint32_t long_interfaz;
    void *stream = buffer->stream;
    int desplazamiento = 0;

    memcpy(&(pcb_a_interfazRecibido->pid), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(pcb_a_interfazRecibido->estado), stream + desplazamiento, sizeof(t_estado_proceso));
    desplazamiento += sizeof(t_estado_proceso);

    memcpy(&(pcb_a_interfazRecibido->quantum), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    pcb_a_interfazRecibido->contexto_ejecucion = malloc(sizeof(t_contexto_ejecucion));
    if (pcb_a_interfazRecibido->contexto_ejecucion == NULL)
    {
        free(pcb_a_interfazRecibido);
                return NULL;

    }

    pcb_a_interfazRecibido->contexto_ejecucion->registros = malloc(sizeof(t_registros));
    if (pcb_a_interfazRecibido->contexto_ejecucion->registros == NULL)
    {
        free(pcb_a_interfazRecibido->contexto_ejecucion);
        free(pcb_a_interfazRecibido);
                return NULL;

    }

    memcpy(pcb_a_interfazRecibido->contexto_ejecucion->registros, stream + desplazamiento, sizeof(t_registros));
    desplazamiento += sizeof(t_registros);

    memcpy(&(pcb_a_interfazRecibido->contexto_ejecucion->motivo_desalojo), stream + desplazamiento, sizeof(t_motivo_desalojo));
    desplazamiento += sizeof(t_motivo_desalojo);


    memcpy(&(unidades_de_trabajo), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(int);

    memcpy(&(long_interfaz), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(int);

    nombre_interfaz = malloc(long_interfaz);

    memcpy(&(nombre_interfaz), stream + desplazamiento, long_interfaz);

      printf("EL TAMNIO DEL DESPLAZAMIENTO ES: %d\n", desplazamiento);

    return pcb_a_interfazRecibido;

}