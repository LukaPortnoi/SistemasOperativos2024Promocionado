#ifndef MANEJO_MEMORIA_H_
#define MANEJO_MEMORIA_H_

#include "./gestor.h"

void iniciar_estructura_proceso_memoria(t_proceso_memoria *proceso_memoria);
void liberar_estructura_proceso_memoria(t_proceso_memoria *proceso_memoria);

uint32_t resize_proceso_memoria(t_proceso_memoria *proceso_memoria, uint32_t tamanio);
uint32_t aumentar_tamanio_proceso_memoria(t_proceso_memoria *proceso_memoria, uint32_t tamanio);
uint32_t disminuir_tamanio_proceso_memoria(t_proceso_memoria *proceso_memoria, uint32_t tamanio);

uint32_t cantidad_paginas_disponibles_memoria();
uint32_t obtener_nro_marco_disponible();
void liberar_marco(uint32_t nro_marco);

#endif // MANEJO_MEMORIA_H_