#include "../include/main.h"

t_log *LOGGER_MEMORIA;
t_config *CONFIG_MEMORIA;
char *PUERTO_ESCUCHA_MEMORIA;
char *IP_MEMORIA;
int TAM_MEMORIA;
int TAM_PAGINA;
char *PATH_INSTRUCCIONES;
int RETARDO_RESPUESTA;
int CLIENTE_CPU, CLIENTE_KERNEL, CLIENTE_IN_OU;

/*void sighandler(int s) {
	terminar_programa(fd_kernel, LOGGER_KERNEL, CONFIG_KERNEL);
	exit(0);
}*/

int main(void)
{
	// signal(SIGINT, sighandler);
	inicializar_config();

	int fd_memoria = iniciar_servidor(LOGGER_MEMORIA, "MEMORIA", IP_MEMORIA, PUERTO_ESCUCHA_MEMORIA);
	//log_info(LOGGER_MEMORIA, "Memoria listo para recibir clientes");

	while (server_escuchar(LOGGER_MEMORIA, "MEMORIA", fd_memoria));

	terminar_programa(fd_memoria, LOGGER_MEMORIA, CONFIG_MEMORIA);

	return 0;
}

void inicializar_config()
{
	procesos_totales = list_create();
	LOGGER_MEMORIA = iniciar_logger("memoria.log", "Servidor Memoria");
	CONFIG_MEMORIA = iniciar_config("./memoria.config", "MEMORIA");
	PUERTO_ESCUCHA_MEMORIA = config_get_string_value(CONFIG_MEMORIA, "PUERTO_ESCUCHA");
	TAM_MEMORIA = config_get_int_value(CONFIG_MEMORIA, "TAM_MEMORIA");
	TAM_PAGINA = config_get_int_value(CONFIG_MEMORIA, "TAM_PAGINA");
	PATH_INSTRUCCIONES = config_get_string_value(CONFIG_MEMORIA, "PATH_INSTRUCCIONES");
	RETARDO_RESPUESTA = config_get_int_value(CONFIG_MEMORIA, "RETARDO_RESPUESTA");
	IP_MEMORIA = config_get_string_value(CONFIG_MEMORIA, "IP_MEMORIA");
}