#include "../include/main.h"

int conexion;
char *puerto_escucha;
char *ip_memoria;
char *puerto_memoria;
char *ip_cpu;
char *puerto_cpu_dispatch;
char *puerto_cpu_interrupt;
char *algoritmo_planificacion;
char *quantum;
char **recursos;
char **instancias_recursos;
int grado_multiprogramacion;

t_log *logger;
t_config *config;

void inicializar_config()
{
	logger = iniciar_logger("kernel.log", "KERNEL");
	config = iniciar_config("./kernel.config", "KERNEL");

	puerto_escucha = config_get_string_value(config, "PUERTO_ESCUCHA");
	ip_memoria = config_get_string_value(config, "IP_MEMORIA");
	puerto_memoria = config_get_string_value(config, "PUERTO_MEMORIA");
	ip_cpu = config_get_string_value(config, "IP_CPU");
	puerto_cpu_dispatch = config_get_string_value(config, "PUERTO_CPU_DISPATCH");
	puerto_cpu_interrupt = config_get_string_value(config, "PUERTO_CPU_INTERRUPT");
	algoritmo_planificacion = config_get_string_value(config, "ALGORITMO_PLANIFICACION");
	quantum = config_get_string_value(config, "QUANTUM");
	recursos = config_get_array_value(config, "RECURSOS");
	instancias_recursos = config_get_array_value(config, "INSTANCIAS_RECURSOS");
	grado_multiprogramacion = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");
}

int main()
{
	inicializar_config();

	// Loggeamos el valor de config
	log_info(logger, "Iniciando Kernel...");

	// Creamos una conexión hacia el servidor
	conexion = crear_conexion(ip_memoria, puerto_memoria);

	// Enviamos al servidor el valor de CLAVE como mensaje
	enviar_mensaje("Mensaje de Kernel para memoria", conexion);

	// Armamos y enviamos el paquete
	paquete(conexion, logger);

	terminar_programa(conexion, logger, config);
}
