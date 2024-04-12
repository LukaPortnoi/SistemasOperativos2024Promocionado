#include "../include/main.h"

int conexion;
char *ip_memoria;
char *puerto_memoria;
char *puerto_escucha_dispatch;
char *puerto_escucha_interrupt;
char *cantidad_entradas_tlb;
char *algoritmo_tlb;

t_log *logger;
t_config *config;

int main(int argc, char* argv[]) {
   inicializar_config();

	// Loggeamos el valor de config
	log_info(logger, "Iniciando CPU...");

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip_memoria, puerto_memoria);

	// Enviamos al servidor el valor de CLAVE como mensaje
	enviar_mensaje("Mensaje de CPU para memoria", conexion);

	// Armamos y enviamos el paquete
	paquete(conexion, logger);

	terminar_programa(conexion, logger, config);
}

t_config *iniciar_config_c(void)
{
	t_config *nuevo_config;
	nuevo_config = config_create("./cpu.config");
	if (nuevo_config == NULL)
	{
		printf("No se pudo leer la config CPU\n");
		exit(2);
	};
	return nuevo_config;
}

t_log *iniciar_logger_c(void)
{
	t_log *nuevo_logger;
	nuevo_logger = log_create("cpu.log", "CPU", 1, LOG_LEVEL_INFO);
	if (nuevo_logger == NULL)
	{
		printf("No se pudo crear el logger CPU\n");
		exit(1);
	};
	return nuevo_logger;
}


void inicializar_config(){
    logger = iniciar_logger_c();
	config = iniciar_config_c();

    ip_memoria = config_get_string_value(config, "IP_MEMORIA");
    puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
    puerto_escucha_dispatch = config_get_string_value(config, "PUERTO_ESCUCHA_DISPATCH");
    puerto_escucha_interrupt = config_get_string_value(config, "PUERTO_ESCUCHA_INTERRUPT");
    cantidad_entradas_tlb = config_get_string_value(config, "CANTIDAD_ENTRADAS_TLB");
    algoritmo_tlb = config_get_string_value(config, "ALGORITMO_TLB");
}

