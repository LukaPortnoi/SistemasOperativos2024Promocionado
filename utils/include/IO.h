#ifndef IO_H_
#define IO_H_

#include "./gestor.h"
#include "./pcb.h"

/*struct interfaz{

};*/

void enviar_interfaz_IO(t_pcb *pcb_actual, char *interfaz, uint32_t *unidades_de_trabajo, int socket_cliente);
void serializar_IO_instruccion(t_paquete *paquete, t_pcb *pcb_actual, uint32_t *unidades_de_trabajo, char *interfaz);
void recibir_interfaz_cpu(int socket_cliente, t_pcb *pcb_a_interfaz, char *nombre_interfaz, uint32_t *unidades_de_trabajo);
void deserializar_interfaz(t_buffer *buffer, t_pcb *pcb_a_interfaz, char *nombre_interfaz, uint32_t *unidades_de_trabajo);


#endif // IO_H_