#ifndef TLB_H_
#define TLB_H_

#include "./gestor.h"

typedef enum
{
    LRU,
    FIFO
} algoritmos_tlb;

typedef struct
{
    uint32_t pid;
    uint32_t pagina;
    uint32_t marco;
    uint32_t tiempo_lru; // El algoritmo LRU sustituye la pagina menos recientemente usada, por eso un campo extra para saber esos tiempos
} entrada_tlb;

typedef struct
{
    entrada_tlb *entradas;
    uint32_t size_tlb;        // Cantidad de entradas totales de la TLB (32 segun el config)
    uint32_t size_actual_tlb; // Nos permite saber la cantidad de entradas que se estan usando el en el momento
    algoritmos_tlb algoritmo;
} t_tlb;

typedef struct
{
    uint32_t direccion_fisica;
    uint32_t tamanio;
} t_direcciones_fisicas;

t_tlb *inicializar_tlb();
void destruir_tlb();
uint32_t buscar_en_tlb(uint32_t pid, uint32_t pagina);
void reemplazo_algoritmo_FIFO(uint32_t pid, uint32_t pagina, uint32_t marco);
void reemplazo_algoritmo_LRU(uint32_t pid, uint32_t pagina, uint32_t marco);
void actualizar_TLB(uint32_t pid, uint32_t pagina, uint32_t marco);
uint32_t traducir_direccion(uint32_t pid, uint32_t logicalAddress, uint32_t pageSize, uint32_t tamanio_registro);
void enviar_Pid_Pagina_Memoria(uint32_t pid_proceso, uint32_t pagina_nueva);
void serializar_nueva_pagina(t_paquete *paquete, uint32_t pid_proceso, uint32_t pagina_nueva);
uint32_t obtener_valor_direccion_fisica(uint32_t direccion_fisica);
void enviar_direccion_fisica_memoria(uint32_t direccion_fisica);
uint32_t recibir_direccion_fisica(int socket_cliente);
uint32_t recibir_marco_memoria(int fd_cpu_memoria);
uint32_t recibir_marco(int socket_cliente);
uint32_t deserializar_marco(t_buffer *buffer);

#endif // TLB_H_