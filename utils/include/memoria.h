#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "./gestor.h"
#include "./sockets_client.h"

typedef struct
{
    uint32_t direccion_fisica;
    uint32_t tamanio;
} t_direcciones_fisicas;

void serializar_direccion_fisica(t_paquete *paquete, uint32_t direccion_fisica);
void deserializar_direccion_fisica(t_buffer *buffer, uint32_t *direccion_fisica);

#endif