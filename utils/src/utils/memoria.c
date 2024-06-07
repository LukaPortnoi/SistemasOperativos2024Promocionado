#include "../include/memoria.h"

// REVISAR
void serializar_direccion_fisica(t_paquete *paquete, uint32_t direccion_fisica)
{
    int buffer_size = sizeof(uint32_t);
    void *stream = malloc(buffer_size);
    if (stream == NULL)
    {
        return;
    }

    memcpy(stream, &direccion_fisica, sizeof(uint32_t));

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

void deserializar_direccion_fisica(t_buffer *buffer, uint32_t *direccion_fisica)
{
    memcpy(direccion_fisica, buffer->stream, sizeof(uint32_t));
}