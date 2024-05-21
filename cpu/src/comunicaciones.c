#include "../include/comunicaciones.h"

typedef struct
{
	t_log *log;
	int fd;
	char *server_name;
} t_procesar_conexion_args;

bool esSyscall = false;
bool interrupciones[5] = {0, 0, 0, 0, 0};

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

		log_info(logger, "Codigo de operacion: %d", cop);

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
			pcb_actual = recibir_pcb(cliente_socket);
			while (!hayInterrupciones() && pcb_actual != NULL && !esSyscall /*pcb_actual->estado != FINALIZADO && !page_fault*/) // Aca deberia ir el check_interrupt()
			{
				ejecutar_ciclo_instruccion(cliente_socket);
			}

			// obtener_motivo_desalojo();
			log_debug(LOGGER_CPU, "PID: %d - Estado: %s, Contexto: %s\n", pcb_actual->pid, estado_to_string(pcb_actual->estado), motivo_desalojo_to_string(pcb_actual->contexto_ejecucion->motivo_desalojo));
			
			enviar_pcb(pcb_actual, cliente_socket); // Envia el PCB actualizado
			esSyscall = false;

			pcb_actual = NULL;

			pthread_mutex_lock(&mutex_interrupt);
			limpiar_interrupciones();
			pthread_mutex_unlock(&mutex_interrupt);
			// liberar_contexto(contexto_actual);
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

static void procesar_conexion_interrupt(void *void_args)
{
	t_procesar_conexion_args *args = (t_procesar_conexion_args *)void_args;
	t_log *logger = args->log;
	int cliente_socket = args->fd;
	char *server_name = args->server_name;
	free(args);

	op_cod cop;
	while (cliente_socket != -1)
	{
		if (recv(cliente_socket, &cop, sizeof(op_cod), 0) != sizeof(op_cod))
		{
			log_info(logger, "Se desconecto el cliente!\n");
			return;
		}
		printf("Codigo de operacion: %d\n", cop);
		switch (cop)
		{
		case MENSAJE:
			recibir_mensaje(cliente_socket, logger);
			break;

		// ----------------------
		// -- KERNEL - CPU --
		// ----------------------
		case INTERRUPCION:
			recibir_interrupciones(cliente_socket, logger);
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

void recibir_interrupciones(int cliente_socket, t_log *logger)
{
	t_interrupcion *interrupcion = recibir_interrupcion(cliente_socket);

	switch (interrupcion->motivo_interrupcion)
	{
	case INTERRUPCION_FIN_QUANTUM:
		interrupciones[0] = true;
		pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_FIN_QUANTUM;
		break;

	case INTERRUPCION_BLOQUEO:
		interrupciones[1] = true;
		break;

	case INTERRUPCION_FINALIZACION:
		interrupciones[2] = true;
		break;

	case INTERRUPCION_ERROR:
		interrupciones[3] = true;
		break;

	case INTERRUPCION_SYSCALL:
		interrupciones[4] = true;
		break;

	default:
		break;
	}
}

bool hayInterrupciones(void)
{
	for (int i = 0; i < 5; i++)
	{
		if (interrupciones[i])
		{
			return true;
		}
	}
	return false;
}

void limpiar_interrupciones(void)
{
	for (int i = 0; i < 5; i++)
	{
		interrupciones[i] = false;
	}
}