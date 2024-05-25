#include "../include/comunicaciones.h"

void procesar_conexion_IO(int server_socket, t_log *logger)
{
	op_cod cop;
	while (server_socket != -1)
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
			case PEDIDO_IO_GEN_SLEEP:
				procesar_sleep(server_socket, logger);
				break;
			default:
				log_info(logger, "Codigo de operacion no reconocido: %d", cop);
				break;
			}
		}
		else if (strcmp(TIPO_INTERFAZ, "STDIN") == 0)
		{
			switch (cop)
			{
			case PEDIDO_IO_STDIN_READ:
				// procesar_stdin(server_socket, logger);
				break;
			default:
				log_info(logger, "Codigo de operacion no reconocido: %d", cop);
				break;
			}
		}
		/*else if (strcmp(TIPO_INTERFAZ, "STDOUT") == 0)
		{
			switch (cop)
			{
			case PEDIDO_IO_STDOUT_WRITE:
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
			case PEDIDO_IO_FS_CREATE:
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
			log_info(logger, "Tipo de interfaz no reconocido: %s", TIPO_INTERFAZ);
		}
	}
}
