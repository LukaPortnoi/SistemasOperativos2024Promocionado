#include "../include/comunicaciones.h"

pthread_mutex_t mutex_comunicacion_procesos;

static void procesar_conexion_memoria(void *void_args)
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
		// -- KERNEL - MEMORIA --
		// ----------------------
		case HANDSHAKE_kernel:
			recibir_mensaje(cliente_socket, logger);
			log_info(logger, "Este deberia ser el canal mediante el cual nos comunicamos con el KERNEL");
			break;

		// Recibe PID y PATH del Kernel, y las guarda en conjunto con la lista de instrucciones en el struct PROCESO_MEMORIA (hecho)
		case INICIALIZAR_PROCESO:
		 	pthread_mutex_lock(&mutex_comunicacion_procesos);
			log_info(logger, "Inicializando estructuras para el proceso");
			proceso_memoria = recibir_proceso_memoria(cliente_socket);		//esta perfecto hasta aca
			log_info(logger, "Se recibio el proceso %d con el path %s", proceso_memoria->pid, proceso_memoria->path);
			proceso_memoria = iniciar_proceso_path(proceso_memoria);
			pthread_mutex_unlock(&mutex_comunicacion_procesos);
			//aca quiero hacer un signal para que el hilo de la cpu se desbloquee y pueda seguir con el proceso
			printf("EL PROCESO LLEGO ACA BIEN");
		

			// cop = recibir_operacion(cliente_socket);
			// log_info(logger, "Proceso inicializado OK - Instrucciones - PID [%d]", proceso_memoria->pid); 
			break;

		// -------------------
		// -- CPU - MEMORIA --
		// -------------------
		case HANDSHAKE_cpu:
			recibir_mensaje(cliente_socket, logger);
			log_info(logger, "Este deberia ser el canal mediante el cual nos comunicamos con la CPU");
			break;

		case PEDIDO_INSTRUCCION:
			pthread_mutex_lock(&mutex_comunicacion_procesos);
			printf("CANTIDAD DE PROCESOS TOTALES ANTES DE PEDIRLOS: %d \n", procesos_totales->elements_count);
			uint32_t pid, pc;
			recibir_pedido_instruccion(&pid, &pc, cliente_socket);
			log_info(logger, "Se recibio un pedido de instruccion para el PID %d y PC %d", pid, pc);
			proceso_memoria = obtener_proceso_pid(pid);
			printf("CANTIDAD DE INSTRUCCIONES DEL PROCESO A ENVIAR: %d \n", proceso_memoria->instrucciones->elements_count);
			//pthread_mutex_lock(&mutex_comunicacion_procesos);
			if (proceso_memoria == NULL)
			{
				log_error(logger, "No se encontro el proceso con PID %d", pid);
				break;
			}
			else
			{
				t_instruccion *instruccion = obtener_instruccion_del_proceso_pc(proceso_memoria, pc);
				log_info(logger, "Se envia la instruccion a CPU de PC %d para el PID %d y es: %s - %s - %s", pc, pid, obtener_nombre_instruccion(instruccion->nombre), instruccion->parametro1, instruccion->parametro2);
				if (instruccion != NULL)
				{
					enviar_instruccion(cliente_socket, instruccion);
				}
				else
				{
					log_error(logger, "No se encontro la instruccion con PC %d para el PID %d", pc, pid);
				}
				break;
			}
		// -------------------
		// -- I/O - MEMORIA --
		// -------------------
		case HANDSHAKE_in_out:
			recibir_mensaje(cliente_socket, logger);
			log_info(logger, "Este deberia ser el canal mediante el cual nos comunicamos con el I/O");
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
		pthread_create(&hilo, NULL, (void *)procesar_conexion_memoria, (void *)args);
		pthread_detach(hilo);
		return 1;
	}
	return 0;
}