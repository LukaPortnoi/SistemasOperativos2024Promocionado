#include "../include/utils_IO.h"

void enviar_datos_interfaz(interfaz *interfaz, int socket_server){
    t_paquete *paquete = crear_paquete_interfaz(interfaz);
    enviar_paquete(paquete, socket_server);
    eliminar_paquete(paquete);
}

t_paquete *crear_paquete_interfaz(interfaz *interfaz)
{
    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->codigo_operacion = DATOS_INTERFAZ;
    paquete->buffer = crear_buffer_interfaz(interfaz);
    return paquete;
}

t_buffer *crear_buffer_interfaz(interfaz *interfaz)
{
    t_buffer *buffer = malloc(sizeof(t_buffer));

    buffer->size = sizeof(uint32_t) * 2 +
                   interfaz->longitud_nombre_interfaz +
                   interfaz->longitud_tipo_interfaz;

    buffer->stream = malloc(buffer->size);

    int desplazamiento = 0;

    memcpy(buffer->stream + desplazamiento, &(interfaz->longitud_nombre_interfaz), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, &(interfaz->longitud_tipo_interfaz), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, interfaz->nombre_interfaz, interfaz->longitud_nombre_interfaz);
    desplazamiento += interfaz->longitud_nombre_interfaz;

    memcpy(buffer->stream + desplazamiento, interfaz->tipo_interfaz, interfaz->longitud_tipo_interfaz);
    desplazamiento += interfaz->longitud_tipo_interfaz;

    return buffer;
}

