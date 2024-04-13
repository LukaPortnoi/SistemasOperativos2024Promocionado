#include "../include/main.h"

t_log* logger_memoria;
t_config* config_memoria;
char* puerto_escucha_memoria;
char* ip_memoria;
int cliente_cpu, cliente_kernel, cliente_in_ou;

int main(void)
{
	inicializar_config();
	
	logger_memoria = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);

	int server_fd = iniciar_servidor(logger_memoria, "MEMORIA", ip_memoria, puerto_escucha_memoria);
	log_info(logger_memoria, "Memoria listo para recibir al cliente");

	procesar_conexion(server_fd, logger_memoria);

}

void inicializar_config(void){
	config_memoria = iniciar_config("./memoria.config","MEMORIA");
	puerto_escucha_memoria = config_get_string_value(config_memoria,"PUERTO_ESCUCHA");
	ip_memoria = config_get_string_value(config_memoria,"IP_MEMORIA");
}
