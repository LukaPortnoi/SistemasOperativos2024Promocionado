#include "../include/tlb.h"

int *cola_fifo;
t_list *lista_lru;
int cant_entradas;
algoritmos_tlb algortimo;

void iniciar_tlb(t_log *logger, char *cantidad_entradas_tlb, char *algoritmo_tlb){}
/*{
    log_info(logger, "Iniciando TLB");
    cant_entradas = cantidad_entradas_tlb;
    //algortimo = algoritmo_tlb;

    if (algoritmo = FIFO)
        log_info(logger, "El algoritmo utilizado en la TLB es FIFO");
    else
        log_info(logger, "El algoritmo utilizado en la TLB es LRU");

    cola_fifo = malloc(sizeof(int) * cant_entradas); // -> FIFO
    lista_lru = list_create();

    //tlb.paginas = (int *)malloc(sizeof(int) * cant_entradas);
    //tlb.marcos = (int *)malloc(sizeof(int) * cant_entradas);

    free_tlb();
}*/

bool buscar_en_tlb(int direccion_logica, int *direccion_fisica, int tamanio_pagina)
{
    int pagina = direccion_logica / tamanio_pagina;
    int desplazamiento = direccion_logica - (pagina * tamanio_pagina);

    int entrada = encontrar_pagina_tlb(pagina);
    if (entrada != -1)
    {
        int marco = 1; //tlb.marcos[entrada];
        *direccion_fisica = marco * 1000 + desplazamiento;
        agregar_a_lru(pagina);
        return true;
    }
    else
        return false;
}

void agregar_a_tlb(int entrada, int pagina, int marco){}
/*{
    if (algoritmo = FIFO)
    { // -> Actualizo la cola del FIFO
        if (!tlb_esta_llena())
        { // -> Encolo
            for (int i = 0; i < cant_entradas; i++)
            {
                if (cola_fifo[i] == -1)
                {
                    cola_fifo[i] = pagina;
                    break;
                }
            }
        }
        else
        {
            for (int i = 1; i < cant_entradas; i++)
            {
                cola_fifo[i - 1] = cola_fifo[i];
            }
            cola_fifo[cant_entradas - 1] = pagina;
        }
    }
    else if (algoritmo == LRU)
    {
        agregar_a_lru(pagina);
    }
    tlb.paginas[entrada] = pagina;
    tlb.marcos[entrada] = marco;
}*/

int encontrar_pagina_tlb(int pagina)
{
    for (int i = 0; i < cant_entradas; i++)
    {
        if (true)//tlb.paginas[i] == pagina)
        {
            return i;
        }
    }
    return -1;
}

void agregar_a_lru(int pagina){}
/*{
    int *pagina_auxiliar = malloc(sizeof(int));
    *pagina_auxiliar = pagina;
    list_add(lista_lru, (void *)pagina_auxiliar);
}*/

void free_tlb(){}
/*{
    for (int i = 0; i < cant_entradas; i++)
    {
        tlb.paginas[i] = -1;
        tlb.marcos[i] = -1;
    }
    for (int j = 0; j < cant_entradas; j++)
    {
        cola_fifo[j] = -1;
    }
    if (list_size(lista_lru) > 0)
    {
        list_clean_and_destroyt_elements(lista_lru, (void *)limpiar_lru);
    }
}*/

void limpiar_lru(int *puntero)
{
    free(puntero);
}