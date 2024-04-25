#ifndef PCB_H
#define PCB_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <assert.h>
#include <pthread.h>
#include "sockets_client.h"
#include "sockets_server.h"
#include "sockets_utils.h"
#include <commons/temporal.h>


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
    uint32_t program_counter;
    uint8_t ax, bx, cx, dx;
    uint32_t eax, ebx, ecx, edx, si, di;

} t_registros;

typedef struct
{
	u_int32_t pid;
	t_estado_proceso estado;
	t_registros registros;
    int quantum;
} t_pcb;


typedef struct {
    t_recurso *archivo;
    char* nombre_archivo;
    int puntero;
} t_archivo_abierto; //ver

// ------------------------------------------------------ Inicializar Registros

void inicializar_registros(t_pcb* pcb);

// ------------------------------------------------------ Funciones PCB

t_pcb* crear_pcb(u_int32_t pid, int quantum);
void destruir_pcb(t_pcb* pcb);

// ------------------------------------------------------ Funciones de Serialización

void* serializar_pcb(t_pcb* pcb, int* size);
t_pcb* deserializar_pcb(void* stream, int size);

// ------------------------------------------------------ Funciones de Envío y Recepción

void enviar_pcb(t_pcb* pcb, int socket_cliente);
t_pcb* recibir_pcb(int socket_cliente);

#endif