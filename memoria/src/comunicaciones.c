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
			log_info(logger, "Se desconecto el cliente!\n");
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
				uint32_t response = resize_proceso_memoria(proceso_memoria, tamanio);
				// enviar_respuesta_resize(cliente_socket, response);
				break;
			}

		case PEDIDO_MOV_IN: // Lee el valor del marco y lo devuelve para guardarlo en el registro (se pide la direccion) - recibo direccion fisica
			t_list *direcciones_fisicas_mov_in = list_create();
			char *valor_leido_mov_in;
			recibir_mov_in_cpu(cliente_socket, direcciones_fisicas_mov_in);
			for (int i = 0; i < list_size(direcciones_fisicas_mov_in); i++)
			{
				t_direcciones_fisicas *direccionAmostrar = list_get(direcciones_fisicas_mov_in, i);
				valor_leido_mov_in = leer_memoria(direccionAmostrar->direccion_fisica, direccionAmostrar->tamanio);
			}
			list_clean_and_destroy_elements(direcciones_fisicas_mov_in, free);
			break;

		case PEDIDO_MOV_OUT: // me pasa por parametro un uint32_t y tengo que guardarlo en el marco que me dice
			t_list *direcciones_fisicas_mov_out = list_create();
			uint32_t valorObtenido_mov_out;
			recibir_mov_out_cpu(direcciones_fisicas_mov_out, &valorObtenido_mov_out, cliente_socket);
			int valor_leido = (int)valorObtenido_mov_out;
    		char* valor_entero_a_escribir = int_to_char(valor_leido);
			int k=0;
			for (int i = 0; i < list_size(direcciones_fisicas_mov_out); i++)
			{
				t_direcciones_fisicas *direccionAmostrar = list_get(direcciones_fisicas_mov_out, i);
				char* valor_parcial_a_pasar = malloc(direccionAmostrar->tamanio + 1);
    			memset(valor_parcial_a_pasar, 0, direccionAmostrar->tamanio + 1);
				for(int j=0; j < direccionAmostrar->tamanio; j++){
					valor_parcial_a_pasar[j] = valor_entero_a_escribir[k];
					k++;
				}
				valor_parcial_a_pasar[direccionAmostrar->tamanio] = '\0';
				escribir_memoria(direccionAmostrar->direccion_fisica, direccionAmostrar->tamanio, valor_parcial_a_pasar);
			}
			list_clean_and_destroy_elements(direcciones_fisicas_mov_out, free);
			break;

		case PEDIDO_COPY_STRING:
			t_list *direcciones_fisicas_escritura = list_create();
			t_list *direcciones_fisicas_lectura = list_create();
			uint32_t tamanio_copy_string;
			recibir_copystring(cliente_socket, direcciones_fisicas_escritura, direcciones_fisicas_lectura, &tamanio_copy_string);
			char* valor_leido_parcial;
			char* valor_leido_completo = malloc(tamanio_copy_string + 1); 
			memset(valor_leido_completo, 0, tamanio_copy_string + 1);

			for(int i=0; i <list_size(direcciones_fisicas_lectura); i++){
				t_direcciones_fisicas *direccion_fisica_actual = list_get(direcciones_fisicas_lectura, i);
				valor_leido_parcial = leer_memoria(direccion_fisica_actual->direccion_fisica, direccion_fisica_actual->tamanio);
				strcat(valor_leido_completo, valor_leido_parcial);
			}
			for (int i = 0; i < list_size(direcciones_fisicas_escritura); i++)
			{
				int z=0; // Reiniciar z a 0 en cada iteración del bucle externo
				t_direcciones_fisicas *direccion_fisica_actual = list_get(direcciones_fisicas_escritura, i);
				char* valor_parcial_a_pasar = malloc(direccion_fisica_actual->tamanio + 1);
				memset(valor_parcial_a_pasar, 0, direccion_fisica_actual->tamanio + 1);
				for(int j=0; j < direccion_fisica_actual->tamanio; j++){
					valor_parcial_a_pasar[j] = valor_leido_completo[z];
					z++;
				}
				valor_parcial_a_pasar[direccion_fisica_actual->tamanio] = '\0';
				escribir_memoria(direccion_fisica_actual->direccion_fisica, direccion_fisica_actual->tamanio, valor_parcial_a_pasar);
			}
			break;

		case PEDIDO_ESCRIBIR_DATO_STDIN:
			t_list *direcciones_fisicas_a_escribir = list_create();
			char *dato_obtenido_stdin;
			dato_obtenido_stdin = recibir_dato_stdin(direcciones_fisicas_a_escribir, cliente_socket);
			int longitud_DATO = strlen(dato_obtenido_stdin);

			int h=0;
			for (int i = 0; i < list_size(direcciones_fisicas_a_escribir); i++)
			{
				t_direcciones_fisicas *direccionAmostrar = list_get(direcciones_fisicas_a_escribir, i);
				char* valor_parcial_a_pasar = malloc(direccionAmostrar->tamanio + 1);
    			memset(valor_parcial_a_pasar, 0, direccionAmostrar->tamanio + 1);
				for(int j=0; j < direccionAmostrar->tamanio; j++){
					if (h<=longitud_DATO)
					valor_parcial_a_pasar[j] = dato_obtenido_stdin[h];
					h++;
				}
				valor_parcial_a_pasar[direccionAmostrar->tamanio] = '\0';
				escribir_memoria(direccionAmostrar->direccion_fisica, direccionAmostrar->tamanio, valor_parcial_a_pasar);
			}
			list_clean_and_destroy_elements(direcciones_fisicas_a_escribir, free);

		case PEDIDO_A_LEER_DATO_STDOUT:
			t_list *direcciones_fisicas_a_leer = list_create();
			char *valor_leido_stdout;
			uint32_t tamanio_registroTotal_stdout;
			recibir_direcciones_de_stdout(cliente_socket, direcciones_fisicas_a_leer);
			for (int i = 0; i < list_size(direcciones_fisicas_a_leer); i++)
			{
				t_direcciones_fisicas *direccionAmostrar = list_get(direcciones_fisicas_a_leer, i);
				valor_leido_stdout = leer_memoria(direccionAmostrar->direccion_fisica, direccionAmostrar->tamanio);
				tamanio_registroTotal_stdout = tamanio_registroTotal_stdout + sizeof(valor_leido_stdout);
				list_add(lista_datos_a_leer, strdup(valor_leido_stdout));
				log_debug(LOGGER_MEMORIA, "Cadena final leída: %s \n", valor_leido_stdout);
			}
			char *valorTotalaDeLeer=concatenar_lista_de_cadenas(lista_datos_a_leer);
			
			//valorTotalaDeLeer= valorTotalaDeLeer + tamanio_registroTotal_stdout;
			printf("Valor leido de memoria: %s \n", valorTotalaDeLeer);
			enviar_dato_leido(cliente_socket, valorTotalaDeLeer);
			list_clean_and_destroy_elements(direcciones_fisicas_a_leer, free);

			break;

		case PEDIDO_MARCO:
			uint32_t pid_proceso, pagina;
			recibir_pedido_marco(&pagina, &pid_proceso, cliente_socket);
			proceso_memoria = obtener_proceso_pid(pid_proceso);
			if (proceso_memoria == NULL)
			{
				log_error(logger, "No se encontro el proceso con PID %d", pid_proceso); // El parametro decia "pidMarco" y se cambio a "pid_proceso" porque no existia, consultar que se quiso hacer ahi
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
		pthread_create(&hilo, NULL, (void *)procesar_conexion_memoria, (void *)args);
		pthread_detach(hilo);
		return 1;
	}
	return 0;
}

