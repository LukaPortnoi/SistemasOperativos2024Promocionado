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
    uint32_t interfaz_length = strlen(interfaz) + 1;

    size_t tam_registros = sizeof(uint32_t) +
                           sizeof(uint8_t) * 4 +
                           sizeof(uint32_t) * 6;

    paquete->buffer->size = sizeof(uint32_t) +
                            sizeof(t_estado_proceso) +
                            sizeof(uint32_t) +
                            tam_registros +
                            sizeof(t_motivo_desalojo) +
                            sizeof(int) +
                            sizeof(uint32_t) +
                            interfaz_length;

    paquete->buffer->stream = malloc(paquete->buffer->size);
    int desplazamiento = 0;

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->pid), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->estado), sizeof(t_estado_proceso));
    desplazamiento += sizeof(t_estado_proceso);

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->quantum), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, pcb->contexto_ejecucion->registros, tam_registros);
    desplazamiento += tam_registros;

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->contexto_ejecucion->motivo_desalojo), sizeof(t_motivo_desalojo));
    desplazamiento += sizeof(t_motivo_desalojo);

    memcpy(paquete->buffer->stream + desplazamiento, &(unidades_de_trabajo), sizeof(int));
    desplazamiento += sizeof(int);

    memcpy(paquete->buffer->stream + desplazamiento, &(interfaz_length), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, interfaz, interfaz_length);

}


t_pcb *recibir_interfaz_cpu(int socket_cliente, char **nombre_interfaz, int *unidades_de_trabajo)
{
    t_paquete *paquete = recibir_paquete(socket_cliente);
    t_pcb *pcb =  deserializar_interfaz(paquete->buffer, nombre_interfaz, unidades_de_trabajo);
    eliminar_paquete(paquete);
    return pcb;
}

t_pcb *deserializar_interfaz(t_buffer *buffer, char **nombre_interfaz, int *unidades_de_trabajo)
{
    t_pcb *pcb = malloc(sizeof(t_pcb));
     if (pcb == NULL)
    {
        return NULL;

    } 
    
    uint32_t long_interfaz;
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
    desplazamiento += sizeof(t_motivo_desalojo);

    memcpy(unidades_de_trabajo, stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(int);

    memcpy(&(long_interfaz), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(int);

    *nombre_interfaz = malloc(long_interfaz);

    memcpy(*nombre_interfaz, stream + desplazamiento, long_interfaz);


    return pcb;
}

interfaz *recibir_datos_interfaz(int socket_cliente)
{
    t_paquete *paquete = recibir_paquete(socket_cliente);
	printf("LLEGO BIEN \n");
    interfaz *interfaz_received = deserializar_interfaz_recibida(paquete->buffer);
    eliminar_paquete(paquete);
    return interfaz_received;
}

interfaz *deserializar_interfaz_recibida(t_buffer *buffer)
{
    interfaz *interfaz_received = malloc(sizeof(interfaz));

	if(interfaz_received == NULL)
	{
		return NULL;
	}

    void *stream = buffer->stream;
    int desplazamiento = 0;

    uint32_t longitud_nombre_interfaz;                                              // Cambio aquí
    memcpy(&(longitud_nombre_interfaz), stream + desplazamiento, sizeof(uint32_t)); // Cambio aquí
    desplazamiento += sizeof(uint32_t);

    uint32_t longitud_tipo_interfaz;                                              // Cambio aquí
    memcpy(&(longitud_tipo_interfaz), stream + desplazamiento, sizeof(uint32_t)); // Cambio aquí
    desplazamiento += sizeof(uint32_t);

    interfaz_received->nombre_interfaz = malloc(longitud_nombre_interfaz + 1);
    memcpy(interfaz_received->nombre_interfaz, stream + desplazamiento, longitud_nombre_interfaz);
    desplazamiento += longitud_nombre_interfaz;

	printf("Durante la deserializacion el nombre es %s \n", interfaz_received->nombre_interfaz);

    interfaz_received->tipo_interfaz = malloc(longitud_tipo_interfaz + 1);
    memcpy(interfaz_received->tipo_interfaz, stream + desplazamiento, longitud_tipo_interfaz);

	printf("Durante la deserializacion el tipo es %s \n", interfaz_received->tipo_interfaz);

    return interfaz_received;
}
