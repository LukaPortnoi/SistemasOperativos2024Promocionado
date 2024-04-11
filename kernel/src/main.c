#include "../include/main.h"

    /*void sigint_handler(int sig)*/
	
	/* VARIABLES*/
	
	int conexion_memoria;
    char *ip_memoria;
    char *puerto_memoria;

    t_log *logger_kernel;
    t_config *config_kernel;

int main()
{
	levantar_config();
	
	log_info(logger_kernel, "IP_MEMORIA: %s - PUERTO_MEMORIA: %s \n", ip_memoria, puerto_memoria);
    conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
    enviar_mensaje("Mensaje de Kernel para memoria", conexion_memoria);
    paquete(conexion_memoria, logger_kernel);


	
    terminar_programa(conexion_memoria, logger_kernel, config_kernel);
}


void levantar_config(void)
{
	/* ---------------- LOGGING ---------------- */
    logger_kernel = iniciar_logger();

    /* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */
    config_kernel = iniciar_config();

	/* ---------------- VARIABLES GLOBALES ---------------- */
    ip_memoria = config_get_string_value(config_kernel, "IP_MEMORIA");
    puerto_memoria = config_get_string_value(config_kernel, "PUERTO_MEMORIA");
}