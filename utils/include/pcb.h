#ifndef PCB_H_
#define PCB_H_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/temporal.h>
#include <assert.h>
#include <pthread.h>

#include "sockets_client.h"
#include "sockets_server.h"
#include "sockets_utils.h"
#include "contexto.h"

typedef enum
{
    NUEVO,
    LISTO,
    EJECUTANDO,
    BLOQUEADO,
    FINALIZADO,
    ERROR
} t_estado_proceso;

typedef struct
{
    uint32_t pid;
    t_estado_proceso estado;
    uint32_t quantum;
    t_contexto_ejecucion *contexto_ejecucion;
} t_pcb;

// Inicializar Registros y Contexto
void inicializar_contexto_y_registros(t_pcb *pcb);

// Funciones PCB
t_pcb *crear_pcb(uint32_t pid, t_estado_proceso estado, uint32_t quantum);
void destruir_pcb(t_pcb *pcb);

// Funciones de Serialización
t_paquete *crear_paquete_PCB(t_pcb *pcb);
t_buffer *crear_buffer_pcb(t_pcb *pcb);
t_pcb *deserializar_pcb(t_buffer *buffer);

// Funciones de Envío y Recepción
void enviar_pcb(t_pcb *pcb, int socket_cliente);
void enviar_paquete_pcb(t_paquete *paquete, int socket_cliente);
t_pcb *recibir_pcb(int socket_cliente);

// Funciones instrucciones
uint32_t str_to_uint32(char *);

// Funciones adicionales
t_pcb *recibir_pcbTOP(int socket_cliente);
void *recibir_bufferTOP(int socket_cliente, int *size);

#endif // PCB_H_