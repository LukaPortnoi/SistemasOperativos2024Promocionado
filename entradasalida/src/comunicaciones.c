#include "../include/comunicaciones.h"

void procesar_conexion_IO(int server_socket, t_log *logger)
{
	op_cod cop;
	while (1)
	{
		if (recv(server_socket, &cop, sizeof(op_cod), 0) != sizeof(op_cod))
		{
			log_info(logger, "Se desconecto el cliente!\n");
			return;
		}

		log_info(logger, "Codigo de operacion: %d", cop);

		if (strcmp(TIPO_INTERFAZ, "GENERICA") == 0)
		{
			switch (cop)
			{
			case PEDIDO_IO_SLEEP:
				aviso_de_confirmacion_instruccion(server_socket, logger); // TODO
				procesar_sleep(server_socket, logger);
				break;
			default:
				log_info(logger, "Codigo de operacion no reconocido: %d", cop);
				// enviar_no_admite_operacion(server_socket);
				break;
			}
		}
		else if (strcmp(TIPO_INTERFAZ, "STDIN") == 0)
		{
			switch (cop)
			{
			case PEDIDO_IO_STDIN:
				aviso_de_confirmacion_instruccion(server_socket, logger); // TODO
				procesar_stdin(server_socket, logger);
				break;
			default:
				log_info(logger, "Codigo de operacion no reconocido: %d", cop);
				// enviar_no_admite_operacion(server_socket);
				break;
			}
		}
		/*else if (strcmp(TIPO_INTERFAZ, "STDOUT") == 0)
		{
			switch (cop)
			{
			case PEDIDO_IO_STDOUT:
				aviso_de_confirmacion_instruccion(server_socket, logger); // TODO
				procesar_stdout(server_socket, logger);
				break;
			default:
				log_info(logger, "Codigo de operacion no reconocido: %d", cop);
				// enviar_no_admite_operacion(server_socket);
				break;
			}
		}
		else if (strcmp(TIPO_INTERFAZ, "DIALFS") == 0)
		{
			switch (cop)
			{
			case PEDIDO_IO_DIALFS:
				aviso_de_confirmacion_instruccion(server_socket, logger); // TODO
				procesar_dialfs(server_socket, logger);
				break;
			default:
				log_info(logger, "Codigo de operacion no reconocido: %d", cop);
				// enviar_no_admite_operacion(server_socket);
				break;
			}
		}*/
		else
		{
			aviso_de_rechazo_instruccion(server_socket, logger); // TODO
		}
	}
}
