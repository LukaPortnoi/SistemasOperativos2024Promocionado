#ifndef MANEJO_MEMORIA_H_
#define MANEJO_MEMORIA_H_

#include "./gestor.h"

void iniciar_estructura_proceso_memoria(t_proceso_memoria *proceso_memoria);
void liberar_estructura_proceso_memoria(t_proceso_memoria *proceso_memoria);

op_cod resize_proceso_memoria(t_proceso_memoria *proceso_memoria, uint32_t tamanio);
op_cod aumentar_tamanio_proceso_memoria(t_proceso_memoria *proceso_memoria, uint32_t tamanio);
op_cod disminuir_tamanio_proceso_memoria(t_proceso_memoria *proceso_memoria, uint32_t tamanio);

uint32_t cantidad_paginas_disponibles_memoria();
uint32_t obtener_nro_marco_disponible();
void liberar_marco(uint32_t nro_marco);
uint32_t obtener_marco_de_pagina(t_proceso_memoria *proceso_memoria, uint32_t nro_pagina);

#endif // MANEJO_MEMORIA_H_