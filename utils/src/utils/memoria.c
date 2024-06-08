#include "../include/memoria.h"

// REVISAR
void serializar_direccion_fisica(t_paquete *paquete, uint32_t direccion_fisica)
{
    paquete->buffer->size = sizeof(uint32_t);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    memcpy(paquete->buffer->stream, &direccion_fisica, sizeof(uint32_t));
}

void deserializar_direccion_fisica(t_buffer *buffer, uint32_t *direccion_fisica)
{
    uint32_t *df = malloc(sizeof(uint32_t));
    if (df == NULL)
    {
        return;
    }

    void *stream = buffer->stream;
    memcpy(df, stream, sizeof(uint32_t));
    *direccion_fisica = *df;
    free(df);
}