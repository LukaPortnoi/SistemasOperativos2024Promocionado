#include "../include/main.h"

int main()
{
    int conexion;
    char *ip_memoria;
    char *puerto_memoria;

    t_log *logger;
    t_config *config;

    /* ---------------- LOGGING ---------------- */
    logger = iniciar_logger();

    /* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */
    config = iniciar_config();

    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");

    // Loggeamos el valor de config
    log_info(logger, "IP_MEMORIA: %s - PUERTO_MEMORIA: %s \n", ip_memoria, puerto_memoria);

    // Creamos una conexión hacia el servidor
    conexion = crear_conexion(ip_memoria, puerto_memoria);

    // Enviamos al servidor el valor de CLAVE como mensaje
    enviar_mensaje("Mensaje de Kernel para memoria", conexion);

    // Armamos y enviamos el paquete
    paquete(conexion, logger);

    terminar_programa(conexion, logger, config);
}

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

void terminar_programa(int conexion, t_log *logger, t_config *config)
{
	liberar_conexion(conexion);
	log_destroy(logger);
	config_destroy(config);
}