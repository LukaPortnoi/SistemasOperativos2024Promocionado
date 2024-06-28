#include "../include/comunicaciones.h"

static void procesar_conexion_memoria(void *void_args)
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
			log_debug(logger, "Cliente desconectado.\n");
			return;
		}

		switch (cop)
		{
		case MENSAJE:
			recibir_mensaje(cliente_socket, logger);
			break;

		// ----------------------
		// -- KERNEL - MEMORIA --
		// ----------------------
		case HANDSHAKE_kernel:
			recibir_mensaje(cliente_socket, logger);
			log_info(logger, "Este deberia ser el canal mediante el cual nos comunicamos con el KERNEL");
			break;

		// Recibe PID y PATH del Kernel, y las guarda en conjunto con la lista de instrucciones en el struct PROCESO_MEMORIA
		case INICIALIZAR_PROCESO:
			proceso_memoria = recibir_proceso_memoria(cliente_socket);
			proceso_memoria = iniciar_proceso_path(proceso_memoria);
			break;

		case FINALIZAR_PROCESO:
			uint32_t pid_a_finalizar;
			recibir_finalizar_proceso(&pid_a_finalizar, cliente_socket);
			proceso_memoria = obtener_proceso_pid(pid_a_finalizar);
			if (proceso_memoria == NULL)
			{
				log_error(logger, "No se encontro el proceso con PID %d", pid_a_finalizar);
				break;
			}
			else
			{
				liberar_estructura_proceso_memoria(proceso_memoria);
				break;
			}

		// -------------------
		// -- CPU - MEMORIA --
		// -------------------
		case HANDSHAKE_cpu:
			recibir_mensaje(cliente_socket, logger);
			log_info(logger, "Este deberia ser el canal mediante el cual nos comunicamos con la CPU");
			break;

		case PEDIDO_INSTRUCCION:
			uint32_t pid, pc;
			recibir_pedido_instruccion(&pid, &pc, cliente_socket);
			// log_debug(logger, "Se recibio un pedido de instruccion para el PID %d y PC %d", pid, pc);
			proceso_memoria = obtener_proceso_pid(pid);
			if (proceso_memoria == NULL)
			{
				log_error(logger, "No se encontro el proceso con PID %d", pid);
				break;
			}
			else
			{
				t_instruccion *instruccion = obtener_instruccion_del_proceso_pc(proceso_memoria, pc);
				if (instruccion != NULL)
				{
					enviar_instruccion(cliente_socket, instruccion);
					// log_debug(logger, "Se envia la instruccion a CPU de PC %d para el PID %d y es: %s - %s - %s", pc, pid, instruccion_to_string(instruccion->nombre), instruccion->parametro1, instruccion->parametro2);
				}
				else
				{
					log_error(logger, "No se encontro la instruccion con PC %d para el PID %d", pc, pid);
				}
				break;
			}

		case PEDIDO_RESIZE:
			uint32_t pidResize, tamanio;
			recibir_pedido_resize(&pidResize, &tamanio, cliente_socket);
			log_debug(logger, "Se recibio un pedido de resize para el PID %d y %d tamanio", pidResize, tamanio);
			proceso_memoria = obtener_proceso_pid(pidResize);
			if (proceso_memoria == NULL)
			{
				log_error(logger, "No se encontro el proceso con PID %d", pidResize);
				break;
			}
			else
			{
				op_cod response = resize_proceso_memoria(proceso_memoria, tamanio);
				enviar_respuesta_resize(cliente_socket, response);
				break;
			}

		case PEDIDO_MOV_IN: // Lee el valor del marco y lo devuelve para guardarlo en el registro (se pide la direccion) - recibo direccion fisica
			t_list *direcciones_fisicas_mov_in = list_create();
			t_list *lista_datos_leidos_mov_in = list_create();
			uint32_t pidMovIn;
			recibir_mov_in_cpu(cliente_socket, direcciones_fisicas_mov_in, &pidMovIn);
			int tamanio_registro = 0;

			for (int i = 0; i < list_size(direcciones_fisicas_mov_in); i++)
			{
				t_direcciones_fisicas *direccionAmostrar = list_get(direcciones_fisicas_mov_in, i);
				tamanio_registro += direccionAmostrar->tamanio;
				// free(direccionAmostrar);
			}

			void *mostrar = malloc(tamanio_registro);
			mostrar = leer_memoria(direcciones_fisicas_mov_in, pidMovIn, tamanio_registro, lista_datos_leidos_mov_in);
			// uint32_t valor = *(uint32_t *)mostrar;

			enviar_dato_movIn(cliente_socket, lista_datos_leidos_mov_in, mostrar, direcciones_fisicas_mov_in, tamanio_registro);
			list_clean_and_destroy_elements(direcciones_fisicas_mov_in, free);
			list_clean_and_destroy_elements(lista_datos_leidos_mov_in, free);
			free(mostrar);
			break;

		case PEDIDO_MOV_OUT:
			// me pasa por parametro un uint32_t y tengo que guardarlo en el marco que me dice

			t_list *direcciones_fisicas_mov_out = list_create();
			void *valor_obtenido_mov_out;
			uint32_t pidMovOut;
			bool es8bits = false;
			recibir_mov_out_cpu(direcciones_fisicas_mov_out, &valor_obtenido_mov_out, cliente_socket, &pidMovOut, &es8bits);
			/*
			if (es8bits) {
				uint8_t valor_obtenido_8bits = *((uint8_t *) valor_obtenido_mov_out);
				printf("Valor obtenido de 8 bits: %d\n", valor_obtenido_8bits);
			} else {
				uint32_t valor_obtenido_32bits = *((uint32_t *) valor_obtenido_mov_out);
				printf("Valor obtenido de 32 bits: %d\n", valor_obtenido_32bits);
			} */
			int tamanioTotal = 0;
			for (int i = 0; i < list_size(direcciones_fisicas_mov_out); i++)
			{
				t_direcciones_fisicas *direccionAmostrar = list_get(direcciones_fisicas_mov_out, i);
				tamanioTotal += direccionAmostrar->tamanio;
				// free(direccionAmostrar);
			}

			escribir_memoria(direcciones_fisicas_mov_out, valor_obtenido_mov_out, pidMovOut, tamanioTotal);

			list_destroy_and_destroy_elements(direcciones_fisicas_mov_out, free);

			break;

		case PEDIDO_COPY_STRING:
			t_list *direcciones_fisicas_escritura = list_create();
			t_list *direcciones_fisicas_lectura = list_create();
			t_list *lista_aux = list_create();
			uint32_t tamanio_copy_string;
			uint32_t pidCopyString;

			recibir_copystring(cliente_socket, direcciones_fisicas_escritura, direcciones_fisicas_lectura, &tamanio_copy_string, &pidCopyString);

			void *dato_leido_copy = leer_memoria(direcciones_fisicas_lectura, pidMovIn, tamanio_copy_string, lista_aux);
			char *valor_copy = (char *)dato_leido_copy;
			escribir_memoria(direcciones_fisicas_escritura, valor_copy, pidCopyString, tamanio_copy_string);

			list_clean_and_destroy_elements(direcciones_fisicas_escritura, free);
			list_clean_and_destroy_elements(direcciones_fisicas_lectura, free);
			list_clean_and_destroy_elements(lista_aux, free);

			break;

		case PEDIDO_ESCRIBIR_DATO_STDIN:
			t_list *direcciones_fisicas_a_escribir = list_create();
			char *dato_obtenido_stdin;
			uint32_t pidStdin;
			dato_obtenido_stdin = recibir_dato_stdin(direcciones_fisicas_a_escribir, cliente_socket, &pidStdin);

			int tamanioTotal_stdin = 0;
			for (int i = 0; i < list_size(direcciones_fisicas_a_escribir); i++)
			{
				t_direcciones_fisicas *direccionAmostrar = list_get(direcciones_fisicas_a_escribir, i);
				tamanioTotal_stdin += direccionAmostrar->tamanio;
				// free(direccionAmostrar);
			}
			int longitud_DATO = strlen(dato_obtenido_stdin);

			escribir_memoria(direcciones_fisicas_a_escribir, dato_obtenido_stdin, pidStdin, longitud_DATO);

			free(dato_obtenido_stdin);
			list_destroy_and_destroy_elements(direcciones_fisicas_a_escribir, free);
			break;

		case PEDIDO_A_LEER_DATO_STDOUT:
			t_list *direcciones_fisicas_a_leer = list_create();
			t_list *lista_datos_a_leer = list_create();
			uint32_t pidStdout;

			recibir_direcciones_de_stdout(cliente_socket, direcciones_fisicas_a_leer, &pidStdout);
			int tamanio_registroTotal_stdout = 1;
			for (int i = 0; i < list_size(direcciones_fisicas_a_leer); i++)
			{
				t_direcciones_fisicas *direccionAmostrar = list_get(direcciones_fisicas_a_leer, i);
				tamanio_registroTotal_stdout += direccionAmostrar->tamanio;
			}

			void *valor_leido_stdout = leer_memoria(direcciones_fisicas_a_leer, pidStdout, tamanio_registroTotal_stdout, lista_datos_a_leer);
			char *valor_stdout = malloc(tamanio_registroTotal_stdout + 1);
			if (valor_stdout == NULL)
			{
				perror("Error al asignar memoria para valor_stdout");
				// Manejo de error
				break;
			}

			memcpy(valor_stdout, valor_leido_stdout, tamanio_registroTotal_stdout);
			valor_stdout[tamanio_registroTotal_stdout] = '\0';

			enviar_dato_leido(cliente_socket, valor_stdout, tamanio_registroTotal_stdout);
			list_destroy_and_destroy_elements(direcciones_fisicas_a_leer, free);
			list_destroy_and_destroy_elements(lista_datos_a_leer, free);

			break;
		case PEDIDO_MARCO:
			uint32_t pid_proceso, pagina;
			recibir_pedido_marco(&pagina, &pid_proceso, cliente_socket);
			proceso_memoria = obtener_proceso_pid(pid_proceso);
			if (proceso_memoria == NULL)
			{
				log_error(logger, "No se encontro el proceso con PID %d", pid_proceso);
				break;
			}
			else
			{
				uint32_t marco = obtener_marco_de_pagina(proceso_memoria, pagina);
				log_info(logger, "PID: %d - PAGINA: %d - MARCO: %d", pid_proceso, pagina, marco);
				enviar_marco(cliente_socket, marco);
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
		case ERROROPCODE:
			log_error(logger, "Cliente desconectado de %s... con cop -1", server_name);
			break;
		default:
			log_error(logger, "Algo anduvo mal en el server de %s, Cod OP: %d", server_name, cop);
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
		pthread_create(&hilo, NULL, (void *)procesar_conexion_memoria, (void *)args);
		pthread_detach(hilo);
		return 1;
	}
	return 0;
}

void enviar_respuesta_resize(int cliente_socket, op_cod response)
{
	send(cliente_socket, &response, sizeof(op_cod), 0);
}