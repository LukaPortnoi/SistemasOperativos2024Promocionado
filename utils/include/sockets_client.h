#ifndef SOCKETS_CLIENT_H_
#define SOCKETS_CLIENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>

#include "./sockets_common.h"
#include "./contexto.h"

typedef struct
{
	int size;
	void *stream;
} t_buffer;

typedef struct
{
	op_cod codigo_operacion;
	t_buffer *buffer;
} t_paquete;

int crear_conexion(char *ip, char *puerto);
void enviar_mensaje(char *mensaje, int socket_cliente);
void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio);
void enviar_paquete(t_paquete *paquete, int socket_cliente);
void crear_buffer(t_paquete *paquete);
void *serializar_paquete(t_paquete *paquete, int bytes);
void enviar_interrupcion(int socket_cliente, t_interrupcion *interrupcion);
t_paquete *crear_paquete_interrupcion(t_interrupcion *interrupcion);
t_buffer *crear_buffer_interrupcion(t_interrupcion *interrupcion);
void liberar_conexion(int socket_cliente);
void eliminar_paquete(t_paquete *paquete);
t_paquete *crear_paquete_con_codigo_de_operacion(op_cod codigo);

#endif /* SOCKETS_CLIENT_H_ */