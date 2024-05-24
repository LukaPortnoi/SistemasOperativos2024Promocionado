#ifndef IO_H_
#define IO_H_

#include "./gestor.h"
#include "./pcb.h"

typedef struct{
    char* nombre_interfaz;
    char* tipo_interfaz;
    uint32_t longitud_nombre_interfaz;
    uint32_t longitud_tipo_interfaz;
}interfaz;

void enviar_interfaz_IO(t_pcb *pcb_actual, char *interfaz, int unidades_de_trabajo, int socket_cliente);
void serializar_IO_instruccion(t_paquete *paquete, t_pcb *pcb_actual, int unidades_de_trabajo, char *interfaz);
t_pcb *recibir_interfaz_cpu(int socket_cliente, char **nombre_interfaz, int *unidades_de_trabajo);
t_pcb *deserializar_interfaz(t_buffer *buffer, char **nombre_interfaz, int *unidades_de_trabajo);
interfaz *recibir_datos_interfaz(int socket_cliente);
interfaz *deserializar_interfaz_recibida(t_buffer *buffer);


#endif // IO_H_