#include "../include/comunicaciones.h"

void procesar_conexion_IO(int server_socket, t_log *logger)
{
    /*t_procesar_conexion_args *args = (t_procesar_conexion_args *)void_args;
	t_log *logger = args->log;
	int server_socket = args->fd;
	char *cliente_name = args->cliente_name;
	free(args);*/

    op_cod cop;
    while(1){
        if (recv(server_socket, &cop, sizeof(op_cod), 0) != sizeof(op_cod))
		{
			log_info(logger, "Se desconecto el cliente!\n");
			return;
		}

		log_info(logger, "Codigo de operacion: %d", cop);

		switch (cop)
        {
            //case IO_GEN_SLEEP:
            //break;
        }
    }
}
