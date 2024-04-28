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
#include "instrucciones.h"
#include <commons/temporal.h>
#include "instrucciones.h"

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
	int pid;
	t_estado_proceso estado;
	t_registros registros;
    int quantum;
    //t_contexto_ejecucion *contexto_ejecucion;
} t_pcb;


typedef struct
{
    int pid;
    int program_counter;
    t_registros *registros;
    t_instruccion *instruccion_ejecutada;
    nombre_instruccion codigo_ultima_instru;
} t_contexto_ejecucion;

/*typedef struct {
    t_recurso *archivo;
    char* nombre_archivo;
    int puntero;
} t_archivo_abierto; //ver

typedef struct {
    char** recursos;
} t_recurso;*/

// ------------------------------------------------------ Inicializar Registros

void inicializar_registros(t_pcb* pcb);

// ------------------------------------------------------ Funciones PCB

t_pcb* crear_pcb(int pid, t_estado_proceso estado, int quantum);
void destruir_pcb(t_pcb* pcb);
int asignar_pid(void);

// ------------------------------------------------------ Funciones de Serialización

t_buffer* crear_buffer_pcb(t_pcb* pcb);
t_pcb* deserializar_pcb(t_paquete* paquete);

// ------------------------------------------------------ Funciones de Envío y Recepción

void enviar_pcb(t_pcb* pcb, int socket_cliente);
t_pcb* recibir_pcb(int socket_cliente);



#endif