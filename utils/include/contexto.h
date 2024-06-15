#ifndef CONTEXTO_H_
#define CONTEXTO_H_

#include "./gestor.h"

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
    uint32_t longitud_parametro1;
    uint32_t longitud_parametro2;
    char *parametro3;
    char *parametro4;
    char *parametro5;
    uint32_t longitud_parametro4;
    uint32_t longitud_parametro3;
    uint32_t longitud_parametro5;
} t_instruccion;

typedef struct
{
    uint32_t program_counter;
    uint8_t ax, bx, cx, dx;
    uint32_t eax, ebx, ecx, edx, si, di;
} t_registros;

typedef enum
{
    SIN_MOTIVO,
    INTERRUPCION_FIN_QUANTUM,
    INTERRUPCION_BLOQUEO,
    INTERRUPCION_FINALIZACION,
    FINALIZACION,
    INTERRUPCION_ERROR,
    INTERRUPCION_OUT_OF_MEMORY,
    INTERRUPCION_SYSCALL
} t_motivo_desalojo;

typedef struct
{
    t_motivo_desalojo motivo_interrupcion;
    int pid;
} t_interrupcion;

typedef enum
{
    FINALIZACION_SIN_MOTIVO,
    SUCCESS,
    INVALID_RESOURCE, 
    INVALID_INTERFACE,
    OUT_OF_MEMORY,
    INTERRUPTED_BY_USER
} t_motivo_finalizacion;
typedef struct
{
    t_registros *registros;
    t_motivo_desalojo motivo_desalojo;
    t_motivo_finalizacion motivo_finalizacion;
} t_contexto_ejecucion;

char *motivo_desalojo_to_string(t_motivo_desalojo motivo);
char *motivo_finalizacion_to_string(t_motivo_finalizacion motivo);
char *nombre_instruccion_to_string(nombre_instruccion instruccion);

#endif // CONTEXTO_H_