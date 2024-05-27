#ifndef IO_H_
#define IO_H_

#include "./gestor.h"
#include "./pcb.h"
typedef enum
{
    GENERICA,
    STDIN,
    STDOUT,
    DIALFS
} t_tipo_interfaz;

typedef struct
{
    char *nombre_interfaz;
    uint32_t tamanio_nombre_interfaz;
    t_tipo_interfaz tipo_interfaz;
} t_interfaz;

typedef struct
{
    int unidades_de_trabajo;
    uint32_t pidPcb;
    char *nombre_interfaz;
} t_interfaz_gen;

typedef struct
{
    int unidades_de_trabajo;
    uint32_t tamanio_nombre_interfaz;
} t_interfaz_stdin;

typedef struct
{
    int unidades_de_trabajo;
    uint32_t tamanio_nombre_interfaz;
} t_interfaz_stdio;

typedef struct
{
    int unidades_de_trabajo;
    uint32_t tamanio_nombre_interfaz;
} t_interfaz_DIALFS;

void enviar_interfaz_IO(t_pcb *pcb_actual, char *interfaz, int unidades_de_trabajo, int socket_cliente, nombre_instruccion instruccion);
void serializar_IO_instruccion(t_paquete *paquete, t_pcb *pcb_actual, int unidades_de_trabajo, char *interfaz, nombre_instruccion instruccion);
t_pcb *recibir_pcb_para_interfaz(int socket_cliente, char **nombre_interfaz, int *unidades_de_trabajo, nombre_instruccion *instruccion);
t_pcb *deserializar_pcb_para_interfaz(t_buffer *buffer, char **nombre_interfaz, int *unidades_de_trabajo, nombre_instruccion *instruccion);
void enviar_datos_interfaz(t_interfaz *interfaz, int socket_server, op_cod codigo_operacion);
t_interfaz *recibir_datos_interfaz(int socket_cliente);
t_paquete *crear_paquete_interfaz(t_interfaz *interfaz, op_cod codigo_operacion);
t_buffer *crear_buffer_interfaz(t_interfaz *interfaz);
t_interfaz *deserializar_interfaz_recibida(t_buffer *buffer);
void enviar_InterfazGenerica(int socket, int unidades_trabajo, uint32_t pid, char *nombre_interfaz);
t_paquete *crear_paquete_InterfazGenerica(t_interfaz_gen *interfaz);
t_buffer *crear_buffer_InterfazGenerica(t_interfaz_gen *interfaz);
t_interfaz_gen *deserializar_InterfazGenerica(t_buffer *buffer);
t_interfaz_gen *recibir_InterfazGenerica(int socket);
void enviar_InterfazGenericaConCodigoOP(int socket, int unidades_trabajo, uint32_t pid, char *nombre_interfaz);
t_paquete *crear_paquete_InterfazGenericaCodOp(t_interfaz_gen *interfaz, op_cod codigo_operacion);

#endif // IO_H_