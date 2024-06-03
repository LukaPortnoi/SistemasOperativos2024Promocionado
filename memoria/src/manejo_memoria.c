#include "../include/manejo_memoria.h"

void iniciar_estructura_proceso_memoria(t_proceso_memoria *proceso_memoria)
{
    proceso_memoria->tabla_paginas = list_create();
    proceso_memoria->tamanio = 0;
    log_info(LOGGER_MEMORIA, "PID: %d - Tamaño: %d", proceso_memoria->pid, proceso_memoria->tamanio);
    /*todos los procesos iniciarán sin espacio reservado en memoria,
    por lo que solamente tendrán una tabla de páginas vacía.*/
}

void liberar_estructura_proceso_memoria(t_proceso_memoria *proceso_memoria)
{
    list_destroy(proceso_memoria->tabla_paginas);
    // log obligario  “PID: <PID> - Tamaño: <CANTIDAD_PAGINAS>”
    log_info(LOGGER_MEMORIA, "PID: %d - Tamaño: %d", proceso_memoria->pid, proceso_memoria->tamanio);
}

/*RESIZE (Tamaño): Solicitará a la Memoria ajustar el tamaño
del proceso al tamaño pasado por parámetro. En caso de que
la respuesta de la memoria sea Out of Memory, se deberá devolver
el contexto de ejecución al Kernel informando de esta situación*/

uint32_t resize_proceso_memoria(t_proceso_memoria *proceso_memoria, uint32_t tamanio)
{
    uint32_t resultado = -1;
    if (tamanio > proceso_memoria->tamanio)
    {
        resultado = aumentar_tamanio_proceso_memoria(proceso_memoria, tamanio);
    }
    else if (tamanio < proceso_memoria->tamanio)
    {
        resultado = disminuir_tamanio_proceso_memoria(proceso_memoria, tamanio);
    }
    return resultado;
}

/*
Ampliación de un proceso
Se deberá ampliar el tamaño del proceso al final del mismo,
pudiendo solicitarse múltiples páginas. Es posible que en un
punto no se puedan solicitar más marcos ya que la memoria se
encuentra llena, por lo que en ese caso se deberá contestar
con un error de Out Of Memory.

Los marcos disponibles se encuentran en la lista de marcos
llamado marcosPaginas. La lista de marcos es una lista de
enteros que representan los números de marcos disponibles
si tienen un -1. Si tienen un numero mayor a 0, significa que
ese marco está ocupado por un proceso (el numero representa
el id del proceso).*/

uint32_t aumentar_tamanio_proceso_memoria(t_proceso_memoria *proceso_memoria, uint32_t tamanio)
{
    uint32_t resultado = 0;
    if (tamanio > proceso_memoria->tamanio)
    {
        uint32_t tamanio_aumento = tamanio - proceso_memoria->tamanio;
        uint32_t cantidad_paginas_aumento = tamanio_aumento / TAM_PAGINA;
        if (tamanio_aumento % TAM_PAGINA != 0)
        {
            cantidad_paginas_aumento++;
        }
        uint32_t cantidad_paginas_disponibles = cantidad_paginas_disponibles_memoria();
        if (cantidad_paginas_disponibles >= cantidad_paginas_aumento)
        {
            // “PID: <PID> - Tamaño Actual: <TAMAÑO_ACTUAL> - Tamaño a Ampliar: <TAMAÑO_A_AMPLIAR>”
            log_info(LOGGER_MEMORIA, "PID: %d - Tamaño Actual: %d - Tamaño a Ampliar: %d", proceso_memoria->pid, proceso_memoria->tamanio, tamanio_aumento);
            uint32_t i;
            for (i = 0; i < cantidad_paginas_aumento; i++)
            {
                uint32_t nro_marco = obtener_nro_marco_disponible();
                log_debug(LOGGER_MEMORIA, "PID: %d - Nro Marco a asignar: %d", proceso_memoria->pid, nro_marco);
            
                // Obtén el marco de la lista y actualiza su pid
                t_marco *marco = list_get(marcosPaginas, nro_marco);
                marco->pid = proceso_memoria->pid;
            
                list_add(proceso_memoria->tabla_paginas, (void *)nro_marco);
            }
            proceso_memoria->tamanio = tamanio;
            resultado = 1;
        }
        else
        {
            // out_of_memory(); en realidad esto se simplificaria devolviendo un -1 y que la cpu maneje ese -1 como out_of_memory
            resultado = -1;
        }
    }
    else
    {
        resultado = -1;
    }

    return resultado;
}

/*

Reducción de un proceso
Se reducirá el mismo desde el final, liberando, en caso de ser necesario,
las páginas que ya no sean utilizadas (desde la última hacia la primera).
*/

uint32_t disminuir_tamanio_proceso_memoria(t_proceso_memoria *proceso_memoria, uint32_t tamanio)
{
    uint32_t resultado = 0;
    if (tamanio < proceso_memoria->tamanio)
    {
        uint32_t tamanio_disminucion = proceso_memoria->tamanio - tamanio;
        uint32_t cantidad_paginas_disminucion = tamanio_disminucion / TAM_PAGINA;
        if (tamanio_disminucion % TAM_PAGINA != 0)
        {
            cantidad_paginas_disminucion++;
        }
        // “PID: <PID> - Tamaño Actual: <TAMAÑO_ACTUAL> - Tamaño a Reducir: <TAMAÑO_A_REDUCIR>”
        log_info(LOGGER_MEMORIA, "PID: %d - Tamaño Actual: %d - Tamaño a Reducir: %d", proceso_memoria->pid, proceso_memoria->tamanio, tamanio_disminucion);
        uint32_t i;
        for (i = 0; i < cantidad_paginas_disminucion; i++)
        {
            uint32_t nro_marco = (uint32_t)list_remove(proceso_memoria->tabla_paginas, proceso_memoria->tabla_paginas->elements_count - 1);
            liberar_marco(nro_marco);
        }
        proceso_memoria->tamanio = tamanio;
        resultado = 1;
    }
    else
    {
        // out_of_memory(); en realidad esto se simplificaria devolviendo un -1 y que la cpu maneje ese -1 como out_of_memory
        resultado = -1;
    }
    return resultado;
}

// cantidad de paginas disponibles en memoria va a entonces ser la cantidad de marcos disponibles
uint32_t cantidad_paginas_disponibles_memoria()
{
    // vamos a buscar entonces la cantidad de marcos disponibles en la lista de marcos
    uint32_t cantidad_paginas_disponibles = 0;
    uint32_t i;
    for (i = 0; i < marcosPaginas->elements_count; i++)
    {
        t_marco *marco = list_get(marcosPaginas, i);
        if (marco->pid == -1)
        {
            cantidad_paginas_disponibles++;
        }
    }

    return cantidad_paginas_disponibles;
}

// obtener el numero de marco disponible en la lista de marcos va a encontrar linealmente
//  el primer marco disponible y lo va a devolver para que se lo asigne a la tabla de paginas
//  un marco esta disponible si su valor en la lista de marcos es -1

uint32_t obtener_nro_marco_disponible()
{
    uint32_t i;
    for (i = 0; i < marcosPaginas->elements_count; i++)
    {
        t_marco *marco = list_get(marcosPaginas, i);
        if (marco->pid == -1)
        {
            return marco->numeroMarco;
        }
    }

    // Devuelve un valor de error si no se encontró ningún marco disponible
    return -1;
}

void liberar_marco(uint32_t nro_marco)
{
    // Establecer el valor del marco en la lista de marcos a -1, indicando que está disponible
    list_replace(marcosPaginas, nro_marco, (void *)-1);
}
