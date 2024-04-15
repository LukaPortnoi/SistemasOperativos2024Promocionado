#include "../include/comunicaciones.h"

int procesar_conexion_io(int server_fd, t_log *logger)
{

	int cliente_fd = esperar_cliente(server_fd, logger);

	t_list *lista;
	while (1)
	{
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op)
		{
		case MENSAJE:
			recibir_mensaje(cliente_fd, logger);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void *)iterator);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger, "Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return EXIT_SUCCESS;
}