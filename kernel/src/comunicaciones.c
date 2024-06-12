#include "../include/comunicaciones.h"

t_interfaz_recibida *interfaz_aux;

static void procesar_conexion_kernel(void *void_args)
{
	t_procesar_conexion_args *args = (t_procesar_conexion_args *)void_args;
	t_log *logger = args->log;
	int cliente_socket = args->fd;
	char *server_name = args->server_name;

	interfaz_aux = malloc(sizeof(t_interfaz_recibida));

	free(args);

	op_cod cop;
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

		// -------------------
		// -- I/O - KERNEL --
		// -------------------
		case HANDSHAKE_in_out:
			recibir_mensaje(cliente_socket, logger);
			log_info(logger, "Este deberia ser el canal mediante el cual nos comunicamos con el I/O");
			break;

		case CONEXION_INTERFAZ:
			t_interfaz *interfaz_recibida_de_IO = recibir_datos_interfaz(cliente_socket);
			agregar_interfaz_a_lista(interfaz_recibida_de_IO, cliente_socket);
			break;

		case DESCONEXION_INTERFAZ:
			t_interfaz *interfaz_desconectada = recibir_datos_interfaz(cliente_socket);
			log_debug(logger, "Desconexion de %s", interfaz_desconectada->nombre_interfaz);
			eliminar_interfaz_de_lista(interfaz_desconectada->nombre_interfaz);
			break;

		case FINALIZACION_INTERFAZ:
			log_trace(logger, "Finalizacion de instruccion de interfaz");
			t_interfaz_gen *interfazRecibidaIO = recibir_InterfazGenerica(cliente_socket);
			if (pcb_a_finalizar == NULL)
			{
				desbloquear_proceso(interfazRecibidaIO->pidPcb);
				t_interfaz_recibida *interfaz_recibida = buscar_interfaz_por_nombre(interfazRecibidaIO->nombre_interfaz);
				squeue_pop(interfaz_recibida->cola_procesos_bloqueados);
			}
			else if (interfazRecibidaIO->pidPcb != pcb_a_finalizar->pid)
			{
				desbloquear_proceso(interfazRecibidaIO->pidPcb);
				t_interfaz_recibida *interfaz_recibida = buscar_interfaz_por_nombre(interfazRecibidaIO->nombre_interfaz);
				squeue_pop(interfaz_recibida->cola_procesos_bloqueados);
			}
			else if (interfazRecibidaIO->pidPcb == pcb_a_finalizar->pid)
			{
				finalizar_proceso(pcb_a_finalizar);
				log_trace(logger, "Se finalizo el proceso luego de I/O %d", pcb_a_finalizar->pid);
			}
			break;

		case FINALIZACION_INTERFAZ_STDIN:
			log_trace(logger, "Finalizacion de instruccion de interfaz");
			t_interfaz_stdin *interfazRecibidaIOstdin = recibir_InterfazStdin(cliente_socket);
			if (pcb_a_finalizar == NULL)
			{
				desbloquear_proceso(interfazRecibidaIOstdin->pidPcb);
				t_interfaz_recibida *interfaz_recibida = buscar_interfaz_por_nombre(interfazRecibidaIOstdin->nombre_interfaz);
				squeue_pop(interfaz_recibida->cola_procesos_bloqueados);
			}
			else if (interfazRecibidaIOstdin->pidPcb != pcb_a_finalizar->pid)
			{
				desbloquear_proceso(interfazRecibidaIOstdin->pidPcb);
				t_interfaz_recibida *interfaz_recibida = buscar_interfaz_por_nombre(interfazRecibidaIOstdin->nombre_interfaz);
				squeue_pop(interfaz_recibida->cola_procesos_bloqueados);
			}
			else if (interfazRecibidaIOstdin->pidPcb == pcb_a_finalizar->pid)
			{
				finalizar_proceso(pcb_a_finalizar);
				log_trace(logger, "Se finalizo el proceso luego de I/O %d", pcb_a_finalizar->pid);
			}
			break;
		case FINALIZACION_INTERFAZ_STDOUT:
			log_trace(logger, "Finalizacion de instruccion de interfaz");
			t_interfaz_stdout *interfazRecibidaIOstdout = recibir_InterfazStdout(cliente_socket);
			if (pcb_a_finalizar == NULL)
			{
				desbloquear_proceso(interfazRecibidaIOstdout->pidPcb);
				t_interfaz_recibida *interfaz_recibida = buscar_interfaz_por_nombre(interfazRecibidaIOstdout->nombre_interfaz);
				squeue_pop(interfaz_recibida->cola_procesos_bloqueados);
			}
			else if (interfazRecibidaIOstdout->pidPcb != pcb_a_finalizar->pid)
			{
				desbloquear_proceso(interfazRecibidaIOstdout->pidPcb);
				t_interfaz_recibida *interfaz_recibida = buscar_interfaz_por_nombre(interfazRecibidaIOstdout->nombre_interfaz);
				squeue_pop(interfaz_recibida->cola_procesos_bloqueados);
			}
			else if (interfazRecibidaIOstdout->pidPcb == pcb_a_finalizar->pid)
			{
				finalizar_proceso(pcb_a_finalizar);
				log_trace(logger, "Se finalizo el proceso luego de I/O %d", pcb_a_finalizar->pid);
			}
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

void agregar_interfaz_a_lista(t_interfaz *interfaz_recibida, int cliente_socket)
{
	interfaz_aux->nombre_interfaz_recibida = interfaz_recibida->nombre_interfaz;
	interfaz_aux->tipo_interfaz_recibida = interfaz_recibida->tipo_interfaz;
	interfaz_aux->socket_interfaz_recibida = cliente_socket;
	interfaz_aux->cola_procesos_bloqueados = squeue_create();

	pthread_mutex_lock(&mutex_lista_interfaces);
	list_add(interfaces_conectadas, interfaz_aux);
	pthread_mutex_unlock(&mutex_lista_interfaces);
}

void eliminar_interfaz_de_lista(char *nombre_interfaz)
{
	bool _buscar_interfaz_por_nombre(t_interfaz_recibida * interfaz)
	{
		return string_equals_ignore_case(interfaz->nombre_interfaz_recibida, nombre_interfaz);
	}

	pthread_mutex_lock(&mutex_lista_interfaces);
	t_interfaz_recibida *interfaz_a_eliminar = list_remove_by_condition(interfaces_conectadas, (void *)_buscar_interfaz_por_nombre);
	pthread_mutex_unlock(&mutex_lista_interfaces);

	free(interfaz_a_eliminar);
}