#ifndef TLB_H_
#define TLB_H_

#include "./gestor.h"

typedef struct
{
    uint32_t pid;
    uint32_t pagina;
    uint32_t marcos;
    uint32_t tiempo_lru; //El algoritmo LRU sustituye la pagina menos recientemente usada, por eso un campo extra para saber esos tiempos
} entrada_tlb;

typedef struct
{
    entrada_tlb *entradas;
    uint32_t size_tlb; //Cantidad de entradas totales de la TLB (32 segun el config)
    uint32_t size_actual_tlb; //Nos permite saber la cantidad de entradas que se estan usando el en el momento
    algoritmos_tlb algoritmo;  
} t_tlb;

typedef enum
{
    LRU,
    FIFO
} algoritmos_tlb;

t_tlb inicializar_tlb();
uint32_t buscar_en_tlb(uint32_t pid, uint32_t pagina);
void reemplazo_algoritmo_FIFO(uint32_t pid, uint32_t pagina, uint32_t marco);
void reemplazo_algoritmo_LRU(uint32_t pid, uint32_t pagina, uint32_t marco, uint32_t tiempo_transcurrido);
void actualizar_TLB(uint32_t pid, uint32_t pagina, uint32_t marco, uint32_t tiempo_transcurrido);


#endif // TLB_H_