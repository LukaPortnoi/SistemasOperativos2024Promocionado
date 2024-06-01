#include "../include/tlb.h"

t_tlb *tlb;

//Inicializar TLB
t_tlb* inicializar_tlb(){
    tlb = (t_tlb *)malloc(sizeof(t_tlb));
    tlb->entradas = (entrada_tlb *)malloc(sizeof(entrada_tlb) * CANTIDAD_ENTRADAS_TLB);
    tlb->algoritmo = (algoritmos_tlb)malloc(sizeof(algoritmos_tlb));
    tlb->size_tlb = CANTIDAD_ENTRADAS_TLB;
    tlb->size_actual_tlb = 0; //Inicialmente la TLB esta vacia
    if (strcmp(ALGORITMO_TLB, "FIFO") == 0){
        tlb->algoritmo = FIFO;
    }else{
        tlb->algoritmo = LRU;
    }
    log_debug(LOGGER_CPU, "Iniciando TLB");
    return tlb;
}

//Busqueda en la TLB
uint32_t buscar_en_tlb(uint32_t pid, uint32_t pagina){
    for(int i=0; i < tlb->size_actual_tlb; i++){
        if(tlb->entradas[i].pid == pid && tlb->entradas[i].pagina == pagina){
            return tlb->entradas[i].marcos; //TLB-HIT    
        }
    }
    return -1; // TLB-MISS
}

//Reemplazo por FIFO
void reemplazo_algoritmo_FIFO(uint32_t pid, uint32_t pagina, uint32_t marco){
    for(int i = 1; i < tlb->size_actual_tlb; i++){
        tlb->entradas[i-1] = tlb->entradas[i];
    }
    tlb->entradas[tlb->size_actual_tlb - 1].pid = pid;
    tlb->entradas[tlb->size_actual_tlb - 1].pagina = pagina;
    tlb->entradas[tlb->size_actual_tlb - 1].marcos = marco;
}

//Reemplazo por LRU
void reemplazo_algoritmo_LRU(uint32_t pid, uint32_t pagina, uint32_t marco, uint32_t tiempo_transcurrido){
    int lruIndex = 0;
    for(int i = 1; i < tlb->size_actual_tlb; i++){
        if(tlb->entradas[i].tiempo_lru < tlb->entradas[lruIndex].tiempo_lru){
            lruIndex = i;
        }
    }
    tlb->entradas[lruIndex].pid = pid;
    tlb->entradas[lruIndex].pagina = pagina;
    tlb->entradas[lruIndex].marcos = marco;
    tlb->entradas[lruIndex].tiempo_lru = tiempo_transcurrido;
}

//Actualizar TLB
void actualizar_TLB(uint32_t pid, uint32_t pagina, uint32_t marco, uint32_t tiempo_transcurrido){
    if(tlb->size_actual_tlb < tlb->size_tlb){
        tlb->entradas[tlb->size_actual_tlb].pid = pid;
        tlb->entradas[tlb->size_actual_tlb].pagina = pagina;
        tlb->entradas[tlb->size_actual_tlb].marcos = marco;
        tlb->entradas[tlb->size_actual_tlb].tiempo_lru = tiempo_transcurrido;
        tlb->size_actual_tlb++;
    }else{
        if(tlb->algoritmo == FIFO){
            reemplazo_algoritmo_FIFO(pid, pagina, marco);
        }else{
            reemplazo_algoritmo_LRU(pid, pagina, marco, tiempo_transcurrido);
        }
    }
}

