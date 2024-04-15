#include "../include/main.h"

t_log* logger_memoria;
t_config* config_memoria;
char* puerto_escucha_memoria;
char* ip_memoria;
int tam_memoria;
int tam_pagina;
char *path_instrucciones;
int retardo_respuesta;
int cliente_cpu, cliente_kernel, cliente_in_ou;

int main(void)
{
	inicializar_config();
	
	int fd_socket_memoria = iniciar_servidor(logger_memoria, "MEMORIA", ip_memoria, puerto_escucha_memoria);
	log_info(logger_memoria, "Memoria listo para recibir al cliente");

	while(server_escuchar(logger_memoria, "MEMORIA", fd_socket_memoria));

	terminar_programa(fd_socket_memoria, logger_memoria, config_memoria);
	
	return 0;
}

void inicializar_config(void){
	logger_memoria = iniciar_logger("memoria.log", "Servidor Memoria");
	config_memoria = iniciar_config("./memoria.config","MEMORIA");
	
	puerto_escucha_memoria = config_get_string_value(config_memoria,"PUERTO_ESCUCHA");
	tam_memoria = config_get_int_value(config_memoria,"TAM_MEMORIA");
	tam_pagina = config_get_int_value(config_memoria,"TAM_PAGINA");
	path_instrucciones = config_get_string_value(config_memoria,"PATH_INSTRUCCIONES");
	retardo_respuesta = config_get_int_value(config_memoria,"RETARDO_RESPUESTA");
	ip_memoria = config_get_string_value(config_memoria,"IP_MEMORIA");
}
