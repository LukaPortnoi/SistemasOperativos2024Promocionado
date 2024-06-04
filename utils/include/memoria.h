#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "./gestor.h"


void serializar_direccion_fisica(t_paquete *paquete, uint32_t direccion_fisica);
void deserializar_direccion_fisica (t_buffer *buffer, uint32_t direccion_fisica);

#endif 