#ifndef CONTEXTO_H_
#define CONTEXTO_H_

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

#include "../include/sockets_client.h"
#include "../include/sockets_server.h"
#include "../include/sockets_utils.h"

typedef enum
{
    SET,
    MOV_IN,
    MOV_OUT,
    SUM,
    SUB,
    JNZ,
    RESIZE,
    COPY_STRING,
    WAIT,
    SIGNAL,
    IO_GEN_SLEEP,
    IO_STDIN_READ,
    IO_STDOUT_WRITE,
    IO_FS_CREATE,
    IO_FS_DELETE,
    IO_FS_TRUNCATE,
    IO_FS_WRITE,
    IO_FS_READ,
    EXIT
} nombre_instruccion;

typedef struct
{
    nombre_instruccion nombre;
    char *parametro1;
    char *parametro2;
    uint32_t *longitud_parametro1;
    uint32_t *longitud_parametro2;
    char *parametro3;
    char *parametro4;
} t_instruccion;

typedef struct
{
    uint32_t program_counter;
    uint8_t ax, bx, cx, dx;
    uint32_t eax, ebx, ecx, edx, si, di;
} t_registros;

typedef enum
{
    INTERRUPCION_FIN_QUANTUM,
    INTERRUPCION_BLOQUEO,
    INTERRUPCION_FINALIZACION,
    INTERRUPCION_ERROR,
    INTERRUPCION_SYSCALL
} t_motivo_desalojo;

typedef struct
{
    t_motivo_desalojo motivo_interrupcion;
    int pid;
} t_interrupcion;

typedef struct
{
    t_registros *registros;
    //t_instruccion *instrucciones; // no vamos a guardarlo aca, sino en una lista en la memoria que va a tener todas las instrucciones
    t_motivo_desalojo motivo_desalojo;
} t_contexto_ejecucion;

void enviar_contexto(int socket, t_contexto_ejecucion *contexto_a_enviar);

void serializar_contexto(t_paquete *paquete, t_contexto_ejecucion *ctx);

t_contexto_ejecucion *recibir_contexto(int socket);

#endif // CONTEXTO_H_