#include "iniciaciones.h"


t_log *iniciar_logger(void)
{
	t_log *nuevo_logger;
	nuevo_logger = log_create("kernel.log", "KERNEL", 1, LOG_LEVEL_INFO);
	if (nuevo_logger == NULL)
	{
		printf("No se pudo crear el logger kernel\n");
		exit(1);
	};
	return nuevo_logger;
}

t_config *iniciar_config(void)
{
	t_config *nuevo_config;
	nuevo_config = config_create("./kernel.config");
	if (nuevo_config == NULL)
	{
		printf("No se pudo leer la config kernel\n");
		exit(2);
	};
	return nuevo_config;
}

void leer_consola(t_log *logger)
{
	char *leido;

	// La primera te la dejo de yapa
	leido = readline("> ");

	// El resto, las vamos leyendo y logueando hasta recibir un string vacío
	while (strcmp(leido, ""))
	{
		log_info(logger, "Se ingresó: %s", leido);
		free(leido);
		leido = readline("> ");
	}

	// ¡No te olvides de liberar las lineas antes de regresar!
	free(leido);
}

void paquete(int conexion, t_log *logger)
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
}
