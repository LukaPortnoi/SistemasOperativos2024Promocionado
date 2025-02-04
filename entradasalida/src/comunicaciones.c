#include "../include/comunicaciones.h"

void procesar_conexion_IO(int server_socket)
{
	op_cod cop;
	while (server_socket != -1)
	{
		if (recv(server_socket, &cop, sizeof(op_cod), 0) != sizeof(op_cod))
		{
			log_debug(LOGGER_INPUT_OUTPUT, "Cliente desconectado.\n");
			return;
		}

		switch (cop)
		{
		case PEDIDO_IO_GEN_SLEEP:
			procesar_sleep(server_socket);
			break;

		case PEDIDO_IO_STDIN_READ:
			procesar_stdin(server_socket);
			break;

		case PEDIDO_IO_STDOUT_WRITE:
			procesar_stdout(server_socket);
			break;

		case PEDIDO_IO_FS_CREATE:
			procesar_dialfs_create(server_socket);
			break;

		case PEDIDO_IO_FS_DELETE:
			procesar_dialfs_delete(server_socket);
			break;

		case PEDIDO_IO_FS_TRUNCATE:
			procesar_dialfs_truncate(server_socket);
			break;

		case PEDIDO_IO_FS_WRITE:
			procesar_dialfs_write(server_socket);
			break;

		case PEDIDO_IO_FS_READ:
			procesar_dialfs_read(server_socket);
			break;

		default:
			log_error(LOGGER_INPUT_OUTPUT, "Codigo de operacion no reconocido: %d", cop);
			break;
		}
	}
}
