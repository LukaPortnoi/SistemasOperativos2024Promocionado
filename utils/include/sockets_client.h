#ifndef SOCKETS_CLIENT_H_
#define SOCKETS_CLIENT_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>
#include<commons/config.h>

#include "./sockets_common.h"

/*typedef enum
{
	MENSAJE,
	PAQUETE
}op_code;*/

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	t_handshake codigo_operacion;
	t_buffer* buffer;
} t_paquete;

typedef struct{
	t_handshake codigo_operacion;
	t_buffer* buffer;
}t_paquete_handshake;

int crear_conexion(char* ip, char* puerto);
void enviar_mensaje(char* mensaje, int socket_cliente);
t_paquete_handshake* crear_paquete(void);
void agregar_a_paquete(t_paquete_handshake* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete_handshake* paquete, int socket_cliente);
void *serializar_paquete(t_paquete_handshake *paquete, int bytes);
void liberar_conexion(int socket_cliente);
void eliminar_paquete(t_paquete_handshake* paquete);

#endif