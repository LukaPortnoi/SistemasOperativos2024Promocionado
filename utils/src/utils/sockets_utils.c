#include "../include/sockets_utils.h"

t_log *iniciar_logger(char *file_name, char *name)
{
	t_log *nuevo_logger;
	nuevo_logger = log_create(file_name, name, 1, LOG_LEVEL_TRACE);
	if (nuevo_logger == NULL)
	{
		printf("No se pudo crear el logger %s\n", name);
		exit(1);
	};
	return nuevo_logger;
}

t_config *iniciar_config(char *file_name, char *name)
{
	t_config *nuevo_config;
	nuevo_config = config_create(file_name);
	if (nuevo_config == NULL)
	{
		printf("No se pudo leer la config %s\n", name);
		exit(2);
	};
	return nuevo_config;
}

void leer_consola(t_log *logger)
{
	char *leido;

	leido = readline("> ");

	while (strcmp(leido, ""))
	{
		log_info(logger, "Se ingresó: %s", leido);
		free(leido);
		leido = readline("> ");
	}

	free(leido);
}

/*void paquete(int conexion, t_log *logger)		//DESHABILITADO PORQUE NO SE USA Y CREAR_PAQUETE() NO ESTÁ DEFINIDO
{
	
	char *leido;
	t_paquete *paquete = crear_paquete();

	leido = readline("> ");
	while (strcmp(leido, ""))
	{
		log_info(logger, "Se ingresó: %s", leido);
		agregar_a_paquete(paquete, leido, strlen(leido) + 1);
		free(leido);
		leido = readline("> ");
	}
	enviar_paquete(paquete, conexion);

	eliminar_paquete(paquete);
}*/

void terminar_programa(int conexion, t_log *logger, t_config *config)
{
	liberar_conexion(conexion);
	log_destroy(logger);
	config_destroy(config);
}