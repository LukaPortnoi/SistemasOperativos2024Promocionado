#ifndef IO_H_
#define IO_H_

#include "./gestor.h"
#include "./pcb.h"

/*struct interfaz{

};*/

void enviar_interfaz_IO(t_pcb *pcb_actual, char *interfaz, int unidades_de_trabajo, int socket_cliente);
void serializar_IO_instruccion(t_paquete *paquete, t_pcb *pcb_actual, int unidades_de_trabajo, char *interfaz);
t_pcb *recibir_interfaz_cpu(int socket_cliente, char *nombre_interfaz, int unidades_de_trabajo);
t_pcb *deserializar_interfaz(t_buffer *buffer, char *nombre_interfaz, int unidades_de_trabajo);


#endif // IO_H_