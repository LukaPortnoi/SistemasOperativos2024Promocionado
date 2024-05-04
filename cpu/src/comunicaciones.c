#include "../include/comunicaciones.h"

typedef struct
{
	t_log *log;
	int fd;
	char *server_name;
} t_procesar_conexion_args;

static void procesar_conexion_dispatch(void *void_args)
{
	t_procesar_conexion_args *args = (t_procesar_conexion_args *)void_args;
	t_log *logger = args->log;
	int cliente_socket = args->fd;
	char *server_name = args->server_name;
	free(args);

	op_cod cop;
	t_list *lista;
	while (cliente_socket != -1)
	{
		if (recv(cliente_socket, &cop, sizeof(op_cod), 0) != sizeof(op_cod))
		{
			log_info(logger, "Se desconecto el cliente!\n");
			return;
		}

		log_info(logger, "Procesando conexion en %s server", server_name);

		switch (cop)
		{
		case MENSAJE:
			recibir_mensaje(cliente_socket, logger);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_socket);
			log_info(logger, "Me llegaron los siguientes valores:");
			list_iterate(lista, (void *)iterator);
			break;

		// ----------------------
		// -- KERNEL - CPU --
		// ----------------------
		case HANDSHAKE_kernel:
			recibir_mensaje(cliente_socket, logger);
			log_info(logger, "Este deberia ser el canal mediante el cual nos comunicamos con el KERNEL");
			break;

		case PCB:
			t_pcb *pcb_actual = recibir_pcb(cliente_socket);
			// TODO: Procesar PCB
			while (/* !es_syscall() &&   !page_fault &&*/ !hayInterrupcion && pcb_actual != NULL) // Aca deberia ir el check_interrupt()
			{
				ejecutar_ciclo_instruccion();
			}
			// obtener_motivo_desalojo();
			enviar_pcb(pcb_actual, fd_cpu_dispatch); // Envia el PCB actualizado

			pcb_actual = NULL;

			// pthread_mutex_lock(&mutex_interrupt);
			// limpiar_interrupciones();
			// pthread_mutex_unlock(&mutex_interrupt);
			// liberar_contexto(contexto_actual);
			break;

		// ---------------
		// -- ERRORES --
		// ---------------
		case -1:
			log_error(logger, "Cliente desconectado de %s... con cop -1", server_name);
			break; // hay un return, voy a probar un break
		default:
			log_error(logger, "Algo anduvo mal en el server de %s", server_name);
			log_info(logger, "Cop: %d", cop);
			break; // hay un return, voy a probar un break
		}
	}

	log_warning(logger, "El cliente se desconecto de %s server", server_name);
	return;
}

static void procesar_conexion_interrupt(void *void_args)
{
	t_procesar_conexion_args *args = (t_procesar_conexion_args *)void_args;
	t_log *logger = args->log;
	int cliente_socket = args->fd;
	char *server_name = args->server_name;
	free(args);

	op_cod cop;
	t_list *lista;
	while (cliente_socket != -1)
	{
		if (recv(cliente_socket, &cop, sizeof(op_cod), 0) != sizeof(op_cod))
		{
			log_info(logger, "Se desconecto el cliente!\n");
			return;
		}

		log_info(logger, "Procesando conexion en %s server", server_name);

		switch (cop)
		{
		case MENSAJE:
			recibir_mensaje(cliente_socket, logger);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_socket);
			log_info(logger, "Me llegaron los siguientes valores:");
			list_iterate(lista, (void *)iterator);
			break;

		// ----------------------
		// -- KERNEL - CPU --
		// ----------------------
		case INTERRUPCION:
		hayInterrupcion = recibir_interrupciones();

		case HANDSHAKE_kernel:
			recibir_mensaje(cliente_socket, logger);
			log_info(logger, "Este deberia ser el canal mediante el cual nos comunicamos con el KERNEL");
			break;

		// ---------------
		// -- ERRORES --
		// ---------------
		case -1:
			log_error(logger, "Cliente desconectado de %s... con cop -1", server_name);
			break; // hay un return, voy a probar un break
		default:
			log_error(logger, "Algo anduvo mal en el server de %s", server_name);
			log_info(logger, "Cop: %d", cop);
			break; // hay un return, voy a probar un break
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
		if (strcmp(server_name, "CPU_DISPATCH") == 0)
		{
			pthread_create(&hilo, NULL, (void *)procesar_conexion_dispatch, args);
		}
		else if (strcmp(server_name, "CPU_INTERRUPT") == 0)
		{
			pthread_create(&hilo, NULL, (void *)procesar_conexion_interrupt, args);
		}
		return 1;
	}
	return 0;
}

bool recibir_interrupciones(void) //  TODO: Revisar
{
	bool hayInterrupcion = false;
	while (1)
	{
		t_interrupcion *interrupcion = recibir_interrupciones(fd_cpu_interrupt);

		switch (interrupcion->motivo_interrupcion)
		{
		case INTERRUPCION_FIN_QUANTUM:
			hayInterrupcion = true;
			break;

		case INTERRUPCION_BLOQUEO:
			hayInterrupcion = true;
			break;

		case INTERRUPCION_FINALIZACION:
			hayInterrupcion = true;
			break;
		case INTERRUPCION_ERROR:
			hayInterrupcion = true;
			break;
		default:
			break;
		}
	}
	return hayInterrupcion;
}