#include "../include/manejo_memoria.h"

void iniciar_estructura_proceso_memoria(t_proceso_memoria *proceso_memoria)
{
    proceso_memoria->tabla_paginas = list_create();
    proceso_memoria->tamanio = 0;
    log_info(LOGGER_MEMORIA, "PID: %d - Tamaño: %d", proceso_memoria->pid, proceso_memoria->tamanio);
}

void liberar_estructura_proceso_memoria(t_proceso_memoria *proceso_memoria)
{
    // Liberar elementos de la tabla de páginas
    while (proceso_memoria->tabla_paginas->elements_count > 0)
    {
        uint32_t *nro_marco = (uint32_t *)list_remove(proceso_memoria->tabla_paginas, 0);
        liberar_marco(*nro_marco);
        free(nro_marco);
    }
    list_destroy(proceso_memoria->tabla_paginas);

    // Liberar elementos de la lista de instrucciones
    while (proceso_memoria->instrucciones->elements_count > 0)
    {
        // Suponiendo que los elementos son punteros que han sido asignados dinámicamente
        free(list_remove(proceso_memoria->instrucciones, 0));
    }
    list_destroy(proceso_memoria->instrucciones);

    // tambien deberia eliminarlo de la lista de procesos totales
    bool _buscar_proceso(void *element)
    {
        return element == proceso_memoria;
    }

    list_remove_by_condition(procesos_totales, _buscar_proceso);

    free(proceso_memoria->path);
    free(proceso_memoria);
    log_debug(LOGGER_MEMORIA, "Proceso liberado");
}

/*RESIZE (Tamaño): Solicitará a la Memoria ajustar el tamaño
del proceso al tamaño pasado por parámetro. En caso de que
la respuesta de la memoria sea Out of Memory, se deberá devolver
el contexto de ejecución al Kernel informando de esta situación*/

op_cod resize_proceso_memoria(t_proceso_memoria *proceso_memoria, uint32_t tamanio)
{
    op_cod resultado = MISMO_TAMANIO;
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

op_cod aumentar_tamanio_proceso_memoria(t_proceso_memoria *proceso_memoria, uint32_t tamanio)
{
    op_cod resultado = OUT_OF_MEMORY;
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
            uint32_t *nro_marco_aux = malloc(sizeof(uint32_t));
            *nro_marco_aux = nro_marco;
            list_add(proceso_memoria->tabla_paginas, nro_marco_aux);
        }
        proceso_memoria->tamanio = tamanio;
        resultado = RESIZE_OK;
    }
    else
    {
        resultado = OUT_OF_MEMORY;
    }

    return resultado;
}

// Reducción de un proceso
// Se reducirá el mismo desde el final, liberando, en caso de ser necesario,
// las páginas que ya no sean utilizadas (desde la última hacia la primera).

op_cod disminuir_tamanio_proceso_memoria(t_proceso_memoria *proceso_memoria, uint32_t tamanio)
{
    op_cod resultado = OUT_OF_MEMORY;
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
            uint32_t *nro_marco = (uint32_t *)list_remove(proceso_memoria->tabla_paginas, proceso_memoria->tabla_paginas->elements_count - 1);
            liberar_marco(*nro_marco);
            free(nro_marco);
        }
        proceso_memoria->tamanio = tamanio;
        resultado = RESIZE_OK;
    }
    else
    {
        resultado = OUT_OF_MEMORY;
    }
    return resultado;
}

// cantidad de paginas disponibles en memoria va a entonces ser la cantidad de marcos disponibles
uint32_t cantidad_paginas_disponibles_memoria()
{
    // vamos a buscar entonces la cantidad de marcos disponibles en la lista de marcos
    uint32_t cantidad_paginas_disponibles = 0;
    uint32_t i;

    log_trace(LOGGER_MEMORIA, "Cantidad de marcos en la lista: %d", list_size(marcosPaginas));

    for (i = 0; i < list_size(marcosPaginas); i++)
    {
        t_marco *marco = list_get(marcosPaginas, i);
        if (marco->pid == -1)
        {
            cantidad_paginas_disponibles++;
        }
    }

    return cantidad_paginas_disponibles;
}

// Obtener el numero de marco disponible en la lista de marcos va a encontrar linealmente
// el primer marco disponible y lo va a devolver para que se lo asigne a la tabla de paginas
// un marco esta disponible si su valor en la lista de marcos es -1

uint32_t obtener_nro_marco_disponible()
{
    uint32_t i;
    for (i = 0; i < list_size(marcosPaginas); i++)
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
    t_marco *marco = list_get(marcosPaginas, nro_marco);
    marco->pid = -1;
    list_replace(marcosPaginas, nro_marco, marco);
}

uint32_t obtener_marco_de_pagina(t_proceso_memoria *proceso_memoria, uint32_t nro_pagina)
{
    return *(uint32_t *)list_get(proceso_memoria->tabla_paginas, nro_pagina);
}

void recibir_finalizar_proceso(uint32_t *pid, int socket)
{
    uint32_t size;
    recv(socket, &size, sizeof(uint32_t), 0);
    recv(socket, pid, size, 0);
    log_debug(LOGGER_MEMORIA, "Finalizar proceso %d", *pid);
}