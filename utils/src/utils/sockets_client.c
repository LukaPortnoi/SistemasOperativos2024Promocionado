#include "../include/sockets_client.h"

void *serializar_paquete(t_paquete *paquete, int bytes)
{
	void *magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento += sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento += paquete->buffer->size;

	return magic;
}

int crear_conexion(char *ip, char *puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Ahora vamos a crear el socket.
	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	// Ahora que tenemos el socket, vamos a conectarlo
	connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);

	freeaddrinfo(server_info);

	return socket_cliente;
}

void enviar_mensaje(char *mensaje, int socket_cliente)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	void *a_enviar = malloc(paquete->buffer->size + sizeof(op_cod) + sizeof(uint32_t));
	int offset = 0;

	memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(op_cod));
	offset += sizeof(op_cod);
	memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);
	if (send(socket_cliente, a_enviar, paquete->buffer->size + sizeof(op_cod) + sizeof(uint32_t), 0) == -1)
	{
		free(a_enviar);
	}
	free(a_enviar);
	eliminar_paquete(paquete);
}

void crear_buffer(t_paquete *paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete *paquete, int socket_cliente)
{
	void *a_enviar = malloc(paquete->buffer->size + sizeof(op_cod) + sizeof(int));
	int offset = 0;

	memcpy(a_enviar + offset, &(paquete->codigo_operacion), sizeof(op_cod));
	offset += sizeof(op_cod);
	printf("Codigo de operacion ENVIADO: %d\n", paquete->codigo_operacion);

	memcpy(a_enviar + offset, &(paquete->buffer->size), sizeof(int));
	offset += sizeof(int);
	printf("Tamaño del buffer ENVIADO: %d\n", paquete->buffer->size);

	memcpy(a_enviar + offset, paquete->buffer->stream, paquete->buffer->size);

	if (send(socket_cliente, a_enviar, paquete->buffer->size + sizeof(op_cod) + sizeof(int), 0) == -1)
	{
		free(a_enviar);
	}

	free(a_enviar);
}

void eliminar_paquete(t_paquete *paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

t_paquete *crear_paquete_con_codigo_de_operacion(op_cod codigo)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo;
	crear_buffer(paquete);
	return paquete;
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}

void enviar_interrupcion(int socket_cliente, t_interrupcion *interrupcion)
{
	t_paquete *paquete = crear_paquete_interrupcion(interrupcion);
	enviar_paquete(paquete, socket_cliente);
	eliminar_paquete(paquete);
}

t_paquete *crear_paquete_interrupcion(t_interrupcion *interrupcion)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = INTERRUPCION;
	paquete->buffer = crear_buffer_interrupcion(interrupcion);
	return paquete;
}

t_buffer *crear_buffer_interrupcion(t_interrupcion *interrupcion)
{
	t_buffer *buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(t_interrupcion);
	buffer->stream = malloc(buffer->size);

	int offset = 0;

	memcpy(buffer->stream + offset, &(interrupcion->motivo_interrupcion), sizeof(t_motivo_desalojo));
	offset += sizeof(t_motivo_desalojo);

	memcpy(buffer->stream + offset, &(interrupcion->pid), sizeof(int));

	return buffer;
}

