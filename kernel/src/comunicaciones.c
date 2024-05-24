#include "../include/comunicaciones.h"

interfaz_recibida* interfaces = NULL;
interfaz_recibida *interfaz_kernel;

static void procesar_conexion_kernel(void *void_args)
{
	t_procesar_conexion_args *args = (t_procesar_conexion_args *)void_args;
	t_log *logger = args->log;
	int cliente_socket = args->fd;
	char *server_name = args->server_name;
	interfaz *interfaz_recibida_de_IO;
	interfaz_kernel = malloc(sizeof(interfaz_recibida));
	
	free(args);

	op_cod cop;
	t_list *lista;
	while (cliente_socket != -1)
	{
		if (recv(cliente_socket, &cop, sizeof(op_cod), 0) != sizeof(op_cod))
		{
			log_debug(logger, "Se desconecto el cliente!\n");
			return;
		}

		switch (cop)
		{
		case MENSAJE:
			recibir_mensaje(cliente_socket, logger);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_socket);
			log_debug(logger, "Me llegaron los siguientes valores:");
			list_iterate(lista, (void *)iterator);
			break;

		// -------------------
		// -- CPU - KERNEL --
		// -------------------
		
		// -------------------
		// -- I/O - KERNEL --
		// -------------------
		case HANDSHAKE_in_out:
			recibir_mensaje(cliente_socket, logger);
			log_info(logger, "Este deberia ser el canal mediante el cual nos comunicamos con el I/O");
			break;
		
		case DATOS_INTERFAZ:
			interfaz_recibida_de_IO = recibir_datos_interfaz(cliente_socket);
			//agregar_interfaz_a_lista(interfaz_recibida_de_IO, cliente_socket);
			//printf("Cantiad de elementos en la lista de interfaces: %d", list_size(interfaces));
			
			break;
		// -------------------
		// -- Nombre recibido de IO_GEN_SLEEP   --
		// -------------------
		case ENVIAR_INTERFAZ:
			log_info(logger, "El nombre recibido de IO_GEN_SLEEP de parte de CPU es: %s" , nombre_interfaz);
			break;
			
		// ---------------
		// -- ERRORES --
		// ---------------
		default:
			log_error(logger, "Algo anduvo mal en el server de %s", server_name); 
			log_info(logger, "Cop: %d", cop);
			break;
		}
	}

	log_warning(logger, "El cliente se desconecto de %s server", server_name);
	return;
}

int server_escuchar(t_log *logger, char *server_name, int server_socket)
{
	int cliente_socket = esperar_cliente(server_socket, logger);

	if (cliente_socket != -1)
	{
		pthread_t hilo;
		t_procesar_conexion_args *args = malloc(sizeof(t_procesar_conexion_args));
		args->log = logger;
		args->fd = cliente_socket;
		args->server_name = server_name;
		pthread_create(&hilo, NULL, (void *)procesar_conexion_kernel, (void *)args);
		pthread_detach(hilo);
		return 1;
	}
	return 0;
}

void agregar_interfaz_a_lista(interfaz *interfaz_recibida, int cliente_socket){
	interfaz_kernel->nombre_interfaz_recibida = interfaz_recibida->nombre_interfaz;
	interfaz_kernel->tipo_interfaz_recibida = interfaz_recibida->tipo_interfaz;
	interfaz_kernel->socket_interfaz_recibida = cliente_socket;
	list_add(interfaces, interfaz_kernel);
}

