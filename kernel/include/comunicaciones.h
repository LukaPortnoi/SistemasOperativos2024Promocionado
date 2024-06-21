#ifndef COMUNICACIONES_H_
#define COMUNICACIONES_H_

#include "./gestor.h"
#include "./planificador.h"

typedef struct
{
	t_log *log;
	int fd;
	char *server_name;
} t_procesar_conexion_args;

int server_escuchar(t_log *logger, char *server_name, int server_socket);

// INTERFACES
void finalizar_proceso_post_io(t_interfaz_gen *interfazRecibidaIO, t_log *logger);
void agregar_interfaz_a_lista(t_interfaz *interfaz_recibida, int cliente_socket);
void eliminar_interfaz_de_lista(char *nombre_interfaz);
void liberar_interfaz(t_interfaz_recibida *interfaz_a_eliminar);

#endif // COMUNICACIONES_H_
