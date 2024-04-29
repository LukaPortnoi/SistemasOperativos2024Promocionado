#ifndef TLB_H_
#define TLB_H_

#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <commons/log.h>
#include <commons/collections/list.h>

typedef struct{
    int *paginas;
    int *marcos;
    int pid;
} tlb;

typedef enum {
    LRU,
    FIFO
} algoritmos_tlb;

void iniciar_tlb(t_log *logger, char *cantidad_entradas_tlb, char *algoritmo_tlb);
bool buscar_en_tlb(int direccion_logica, int *direccion_fisica, int tamanio_pagina);
void agregar_a_tlb(int entrada, int pagina, int marco);
int encontrar_pagina_tlb(int pagina);
void agregar_a_lru(int pagina);
void free_tlb(void);
void limpiar_lru(int *puntero);

#endif // TLB_H_