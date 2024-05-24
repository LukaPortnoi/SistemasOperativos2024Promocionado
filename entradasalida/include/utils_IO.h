#ifndef UTILS_IO_H
#define UTILS_IO_H 

#include "./gestor.h"

void enviar_datos_interfaz(interfaz *interfaz, int socket_server);
t_paquete *crear_paquete_interfaz(interfaz *interfaz);
t_buffer *crear_buffer_interfaz(interfaz *interfaz);

#endif