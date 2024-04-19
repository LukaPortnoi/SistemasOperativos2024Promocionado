#include "../include/main.h"

int main(void)
{
	inicializar_config();

	int fd_memoria = iniciar_servidor(LOGGER_MEMORIA, "MEMORIA", IP_MEMORIA, PUERTO_ESCUCHA_MEMORIA);
	log_info(LOGGER_MEMORIA, "Memoria listo para recibir clientes");

	while (server_escuchar(LOGGER_MEMORIA, "MEMORIA", fd_memoria));

	terminar_programa(fd_memoria, LOGGER_MEMORIA, CONFIG_MEMORIA);

	return 0;
}

void inicializar_config()
{
	LOGGER_MEMORIA = iniciar_logger("memoria.log", "Servidor Memoria");
	CONFIG_MEMORIA = iniciar_config("./memoria.config", "MEMORIA");

	PUERTO_ESCUCHA_MEMORIA = config_get_string_value(CONFIG_MEMORIA, "PUERTO_ESCUCHA");
	TAM_MEMORIA = config_get_int_value(CONFIG_MEMORIA, "TAM_MEMORIA");
	TAM_PAGINA = config_get_int_value(CONFIG_MEMORIA, "TAM_PAGINA");
	PATH_INSTRUCCIONES = config_get_string_value(CONFIG_MEMORIA, "PATH_INSTRUCCIONES");
	RETARDO_RESPUESTA = config_get_int_value(CONFIG_MEMORIA, "RETARDO_RESPUESTA");
	IP_MEMORIA = config_get_string_value(CONFIG_MEMORIA, "IP_MEMORIA");
}