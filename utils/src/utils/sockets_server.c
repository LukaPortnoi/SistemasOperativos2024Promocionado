#include "../include/sockets_server.h"

t_log *logger_recibido;

int iniciar_servidor(t_log *logger, const char *name, char *ip, char *puerto)
{
	logger_recibido = logger;
	int socket_servidor;
	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &servinfo);

	bool conecto = false;

	for (struct addrinfo *p = servinfo; p != NULL; p = p->ai_next)
	{
		socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (socket_servidor == -1)
			continue;

		int enable = 1;
		if (setsockopt(socket_servidor, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
		{
			close(socket_servidor);
			log_error(logger, "setsockopt(SO_REUSEADDR) failed");
			continue;
		}

		if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1)
		{
			close(socket_servidor);
			continue;
		}
		conecto = true;
		break;
	}
	if (!conecto)
	{
		free(servinfo);
		return 0;
	}
	listen(socket_servidor, SOMAXCONN);
	log_trace(logger, "Servidor %s escuchando en %s:%s", name, ip, puerto);

	freeaddrinfo(servinfo);
	return socket_servidor;
}

int esperar_cliente(int socket_servidor, t_log *logger)
{
	int socket_cliente = accept(socket_servidor, NULL, NULL);
	//log_debug(logger, "Se conecto un cliente!");

	return socket_cliente;
}

void iterator(char *value)
{
	log_info(logger_recibido, "%s", value);
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if (recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void *recibir_buffer(int *size, int socket_cliente)
{
	void *buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente, t_log *logger)
{
	int size;
	char *buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje: %s", buffer);
	free(buffer);
}

t_paquete *recibir_paquete(int socket_cliente)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->stream = NULL;
	paquete->buffer->size = 0;
	paquete->codigo_operacion = 0;

	if (recv(socket_cliente, &(paquete->buffer->size), sizeof(uint32_t), 0) != sizeof(uint32_t))
	{
		printf("Error al recibir el tamanio del buffer\n");
		return NULL;
	}

	paquete->buffer->stream = malloc(paquete->buffer->size);

	if (recv(socket_cliente, paquete->buffer->stream, paquete->buffer->size, 0) != paquete->buffer->size)
	{
		printf("Error al recibir el buffer\n");
		return NULL;
	}

	return paquete;
}

t_interrupcion *recibir_interrupcion(int socket_cliente)
{
	t_paquete *paquete = recibir_paquete(socket_cliente);
	t_interrupcion *interrupcion = deserializar_interrupcion(paquete->buffer);
	eliminar_paquete(paquete);
	return interrupcion;
}

t_interrupcion *deserializar_interrupcion(t_buffer *buffer)
{
	t_interrupcion *interrupcion = malloc(sizeof(t_interrupcion));
	void *stream = buffer->stream;
	int desplazamiento = 0;

	memcpy(&(interrupcion->motivo_interrupcion), stream + desplazamiento, sizeof(t_motivo_desalojo));
	desplazamiento += sizeof(t_motivo_desalojo);
	memcpy(&(interrupcion->pid), stream + desplazamiento, sizeof(int));

	return interrupcion;
}