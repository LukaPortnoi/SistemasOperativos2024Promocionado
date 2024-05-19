#ifndef COMUNICACIONES_H_
#define COMUNICACIONES_H_

#include "./gestor.h"

typedef struct
{
	t_log *log;
	int fd;
	char *server_name;
} t_procesar_conexion_args;

int server_escuchar(t_log *logger, char *server_name, int server_socket);

#endif // COMUNICACIONES_H_
