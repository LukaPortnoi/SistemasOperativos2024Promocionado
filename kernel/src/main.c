#include "../include/main.h"

int conexion_memoria;
int conexion_cpu_dispatch;
int conexion_cpu_interrupt;
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

char *ip_kernel;

t_log *logger_kernel;
t_config *config_kernel;

int main()
{
	inicializar_config();

	log_info(logger_kernel, "Iniciando Kernel...");

	/*conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
	enviar_mensaje("Mensaje de Kernel para memoria", conexion_memoria);
	paquete(conexion_memoria, logger_kernel);*/

	/*conexion_cpu_dispatch = crear_conexion(ip_cpu, puerto_cpu_dispatch); //aqui vamos a planificar la ejecucion de procesos
	enviar_mensaje("Mensaje de Kernel para CPU", conexion_cpu_dispatch);
	paquete(conexion_cpu_dispatch, logger_kernel);*/

	int server_escucha_kernel = iniciar_servidor(logger_kernel, "KERNEL", ip_kernel, puerto_escucha);
	log_info(logger_kernel, "Kernel listo para recibir al cliente");
	procesar_conexion(server_escucha_kernel, logger_kernel);
	/*conexion_cpu_interrupt = crear_conexion(ip_cpu, puerto_cpu_interrupt); //aqui vamos a planificar la ejecucion de procesos
	enviar_mensaje("Mensaje de Kernel para CPU", conexion_cpu_interrupt);
	paquete(conexion_cpu_interrupt, logger_kernel);*/

	//log_info(logger_kernel, "Se cerrara la conexion");
	terminar_programa(conexion_cpu_dispatch, logger_kernel, config_kernel);
}

void inicializar_config(void)
{
	logger_kernel = iniciar_logger("kernel.log", "KERNEL");
	config_kernel = iniciar_config("./kernel.config", "KERNEL");

	//liberar los get_array_value
	puerto_escucha = config_get_string_value(config_kernel, "PUERTO_ESCUCHA");
	ip_memoria = config_get_string_value(config_kernel, "IP_MEMORIA");
	puerto_memoria = config_get_string_value(config_kernel, "PUERTO_MEMORIA");
	ip_cpu = config_get_string_value(config_kernel, "IP_CPU");
	puerto_cpu_dispatch = config_get_string_value(config_kernel, "PUERTO_CPU_DISPATCH");
	puerto_cpu_interrupt = config_get_string_value(config_kernel, "PUERTO_CPU_INTERRUPT");
	algoritmo_planificacion = config_get_string_value(config_kernel, "ALGORITMO_PLANIFICACION");
	quantum = config_get_string_value(config_kernel, "QUANTUM");
	recursos = config_get_array_value(config_kernel, "RECURSOS");
	instancias_recursos = config_get_array_value(config_kernel, "INSTANCIAS_RECURSOS");
	grado_multiprogramacion = config_get_int_value(config_kernel, "GRADO_MULTIPROGRAMACION");
	ip_kernel = config_get_string_value(config_kernel, "IP_KERNEL");
}


