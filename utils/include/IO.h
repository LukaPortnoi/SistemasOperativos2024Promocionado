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

void enviar_interfaz_IO(t_pcb *pcb_actual, char *interfaz, int unidades_de_trabajo, int socket_cliente, nombre_instruccion instruccion);
void serializar_IO_instruccion(t_paquete *paquete, t_pcb *pcb_actual, int unidades_de_trabajo, char *interfaz, nombre_instruccion instruccion);
t_pcb *recibir_pcb_para_interfaz(int socket_cliente, char **nombre_interfaz, int *unidades_de_trabajo, nombre_instruccion *instruccion);
t_pcb *deserializar_pcb_para_interfaz(t_buffer *buffer, char **nombre_interfaz, int *unidades_de_trabajo, nombre_instruccion *instruccion);
void enviar_datos_interfaz(interfaz *interfaz, int socket_server);
t_interfaz *recibir_datos_interfaz(int socket_cliente);
t_paquete *crear_paquete_interfaz(interfaz *interfaz);
t_buffer *crear_buffer_interfaz(interfaz *interfaz);
t_interfaz *deserializar_interfaz_recibida(t_buffer *buffer);

#endif // IO_H_