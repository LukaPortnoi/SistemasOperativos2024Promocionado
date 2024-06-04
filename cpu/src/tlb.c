#include "../include/tlb.h"

t_tlb *tlb;

// Inicializar TLB
t_tlb *inicializar_tlb()
{
    tlb = (t_tlb *)malloc(sizeof(t_tlb));
    tlb->entradas = (entrada_tlb *)malloc(sizeof(entrada_tlb) * CANTIDAD_ENTRADAS_TLB);
    tlb->algoritmo = (algoritmos_tlb)malloc(sizeof(algoritmos_tlb));
    tlb->size_tlb = CANTIDAD_ENTRADAS_TLB;
    tlb->size_actual_tlb = 0; // Inicialmente la TLB esta vacia
    if (strcmp(ALGORITMO_TLB, "FIFO") == 0)
    {
        tlb->algoritmo = FIFO;
    }
    else
    {
        tlb->algoritmo = LRU;
    }
    log_debug(LOGGER_CPU, "Iniciando TLB");
    return tlb;
}

// Busqueda en la TLB
uint32_t buscar_en_tlb(uint32_t pid, uint32_t pagina)
{
    for (int i = 0; i < tlb->size_actual_tlb; i++)
    {
        if (tlb->entradas[i].pid == pid && tlb->entradas[i].pagina == pagina)
        {
            return tlb->entradas[i].marco; // TLB-HIT
        }
    }
    return -1; // TLB-MISS
}

// Reemplazo por FIFO
void reemplazo_algoritmo_FIFO(uint32_t pid, uint32_t pagina, uint32_t marco)
{
    for (int i = 1; i < tlb->size_actual_tlb; i++)
    {
        tlb->entradas[i - 1] = tlb->entradas[i];
    }
    tlb->entradas[tlb->size_actual_tlb - 1].pid = pid;
    tlb->entradas[tlb->size_actual_tlb - 1].pagina = pagina;
    tlb->entradas[tlb->size_actual_tlb - 1].marco = marco;
}

// Reemplazo por LRU
void reemplazo_algoritmo_LRU(uint32_t pid, uint32_t pagina, uint32_t marco)
{
    int lruIndex = 0;
    for (int i = 1; i < tlb->size_actual_tlb; i++)
    {
        if (tlb->entradas[i].tiempo_lru < tlb->entradas[lruIndex].tiempo_lru)
        {
            lruIndex = i;
        }
    }
    tlb->entradas[lruIndex].pid = pid;
    tlb->entradas[lruIndex].pagina = pagina;
    tlb->entradas[lruIndex].marco = marco;
    // tlb->entradas[lruIndex].tiempo_lru = tiempo_transcurrido;
}

// Actualizar TLB
void actualizar_TLB(uint32_t pid, uint32_t pagina, uint32_t marco)
{
    if (tlb->size_actual_tlb < tlb->size_tlb)
    {
        tlb->entradas[tlb->size_actual_tlb].pid = pid;
        tlb->entradas[tlb->size_actual_tlb].pagina = pagina;
        tlb->entradas[tlb->size_actual_tlb].marco = marco;
        // tlb->entradas[tlb->size_actual_tlb].tiempo_lru = tiempo_transcurrido;
        tlb->size_actual_tlb++;
    }
    else
    {
        if (tlb->algoritmo == FIFO)
        {
            reemplazo_algoritmo_FIFO(pid, pagina, marco);
        }
        else
        {
            reemplazo_algoritmo_LRU(pid, pagina, marco);
        }
    }
}

int traducir_direccion(uint32_t pid, uint32_t logicalAddress, uint32_t pageSize)
{
    uint32_t pagina = logicalAddress / pageSize;
    uint32_t offset = logicalAddress - pagina * pageSize;
    uint32_t direccionFisica = 0;
    // Buscar en la TLB

    uint32_t marco = buscar_en_tlb(pid, pagina);
    if (marco != -1)
    {
        // TLB Hit
        printf("TLB Hit\n");
        direccionFisica = marco * pageSize + offset;
        return direccionFisica;
    }
    else
    {
        // TLB Miss
        printf("TLB Miss\n");

        // pido marco a memoria enviando la pagina y el pid y me devuelve un marco
        enviar_Pid_Pagina_Memoria(pid, pagina);
        marco = recibir_marco_memoria(fd_cpu_memoria);
        actualizar_TLB(pid, pagina, marco);
        direccionFisica = marco * pageSize + offset;
        return direccionFisica;
    }
}

void enviar_Pid_Pagina_Memoria(uint32_t pid_proceso, uint32_t pagina_nueva)
{
    t_paquete *paquete_nueva_pagina = crear_paquete_con_codigo_de_operacion(PEDIDO_MARCO);
    serializar_nueva_pagina(paquete_nueva_pagina, pid_proceso, pagina_nueva);
    enviar_paquete(paquete_nueva_pagina, fd_cpu_memoria);
    eliminar_paquete(paquete_nueva_pagina);
}

void serializar_nueva_pagina(t_paquete *paquete, uint32_t pid_proceso, uint32_t pagina_nueva)
{
    int buffer_size = sizeof(uint32_t) + sizeof(uint32_t);
    void *stream = malloc(buffer_size);
    if (stream == NULL)
    {
        return;
    }

    int offset = 0;
    memcpy(stream + offset, &pid_proceso, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(stream + offset, &pagina_nueva, sizeof(uint32_t));

    t_buffer *buffer = malloc(sizeof(t_buffer));
    if (buffer == NULL)
    {
        free(stream);
        return;
    }

    buffer->size = buffer_size;
    buffer->stream = stream;

    paquete->buffer = buffer;
}

char *obtener_valor_direccion_fisica(uint32_t direccion_fisica)
{

    if (direccion_fisica == -1)
    {
        return direccion_fisica;
    }

    enviar_direccion_fisica_memoria(direccion_fisica);

    char resultado = recibir_direccion_fisica(fd_cpu_memoria);


   //char valor="AX";

   // log_info(cpu_logger_info, "PID: %d - Acción: LEER - Dirección Física: %d - Valor: %d", contexto_actual->pid, df, valor);

    return resultado;
}

uint32_t recibir_direccion_fisica(int socket_cliente)
{
    t_paquete *paquete = recibir_paquete(socket_cliente);
    uint32_t valor_direccion_fisica = deserializar_direccion_fisica(paquete->buffer, direccion_fisica);
    eliminar_paquete(paquete);
    return valor_direccion_fisica;
}


void enviar_direccion_fisica_memoria(uint32_t direccion_fisica)
{
    t_paquete *paquete_direccion_fisica = crear_paquete_con_codigo_de_operacion(ENVIAR_DIRECCION_FISICA);
    serializar_direccion_fisica(paquete_direccion_fisica, direccion_fisica);
    enviar_paquete(paquete_direccion_fisica, fd_cpu_memoria);
    eliminar_paquete(paquete_direccion_fisica);
}


uint32_t recibir_marco_memoria(int fd_cpu_memoria){
    op_cod cop;
    uint32_t marcoRecibido;

    recv(fd_cpu_memoria, &cop, sizeof(op_cod), 0);
    switch (cop)
    {
    case ENVIAR_MARCO:
        marcoRecibido = recibir_marco(fd_cpu_memoria);
        break;

    default:
        log_error(LOGGER_KERNEL, "No se pudo recibir el marco");
        break;
    }

    if (marcoRecibido == NULL)
    {
        log_error(LOGGER_KERNEL, "Error al recibir MARCO de MEMORIA");
        return NULL;
    }

    return marcoRecibido;

}
    

uint32_t recibir_marco(int socket_cliente)
{
    t_paquete *paquete = recibir_paquete(socket_cliente);
    uint32_t marco = deserializar_marco(paquete->buffer);
    eliminar_paquete(paquete);
    return marco;
}


uint32_t deserializar_marco(t_buffer *buffer)
{
    uint32_t marco = malloc(sizeof(uint32_t));

    if (marco == NULL)
    {
        return NULL;
    }

    void *stream = buffer->stream;

    memcpy(&marco, buffer->stream, sizeof(uint32_t));
    return marco;
}


