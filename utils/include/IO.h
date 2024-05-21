#ifndef IO_H_
#define IO_H_

#include "./gestor.h"
#include "./pcb.h"

/*struct interfaz{

};*/

void enviar_interfaz_IO(t_pcb *pcb_actual, char *interfaz, int unidades_de_trabajo, int socket_cliente);
void serializar_IO_instruccion(t_paquete *paquete, t_pcb *pcb_actual, uint32_t unidades_de_trabajo, char *interfaz);

#endif // IO_H_