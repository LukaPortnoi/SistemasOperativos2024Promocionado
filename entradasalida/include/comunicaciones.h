#ifndef COMUNICACIONES_H_
#define COMUNICACIONES_H_

#include "./gestor.h"
#include "./instrucciones_io.h"

void procesar_conexion_IO(int server_socket, t_log *logger);

#endif // COMUNICACIONES_H_