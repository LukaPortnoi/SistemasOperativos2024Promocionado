#include "../include/main.h"

int main(void)
{
	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);

	int server_fd = iniciar_servidor();
	log_info(logger, "Memoria listo para recibir al cliente");

	procesar_conexion(server_fd);

}