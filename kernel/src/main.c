#include "../include/main.h"

t_log *LOGGER_KERNEL;
t_config *CONFIG_KERNEL;

char *PUERTO_ESCUCHA;
char *IP_MEMORIA;
char *PUERTO_MEMORIA;
char *IP_CPU;
char *PUERTO_CPU_DISPATCH;
char *PUERTO_CPU_INTERRUPT;
char *ALGORITMO_PLANIFICACION;
uint32_t QUANTUM;
char **RECURSOS;
char **INSTANCIAS_RECURSOS;
int GRADO_MULTIPROGRAMACION;
char *IP_KERNEL;

int fd_kernel;
int fd_kernel_memoria;
int fd_kernel_cpu_dispatch;
int fd_kernel_cpu_interrupt;

t_pcb *pcb_ejecutandose;
pthread_t hilo_server_kernel;

void sighandler(int s)
{
	terminar_programa(fd_kernel, LOGGER_KERNEL, CONFIG_KERNEL);
	liberar_conexion(fd_kernel_memoria);
	liberar_conexion(fd_kernel_cpu_dispatch);
	liberar_conexion(fd_kernel_cpu_interrupt);
	exit(0);
}

int main()
{
	signal(SIGINT, sighandler);
	inicializar_config();
	iniciar_conexiones();
	iniciar_colas_y_semaforos();
	iniciar_planificador_largo_plazo();
	iniciar_planificador_corto_plazo();

	iniciar_consola_interactiva();

	log_info(LOGGER_KERNEL, "Finalizando Kernel...");

	terminar_programa(fd_kernel, LOGGER_KERNEL, CONFIG_KERNEL);
}

void inicializar_config()
{
	LOGGER_KERNEL = iniciar_logger("kernel.log", "KERNEL");
	CONFIG_KERNEL = iniciar_config("./kernel.config", "KERNEL"); // liberar los get_array_value
	PUERTO_ESCUCHA = config_get_string_value(CONFIG_KERNEL, "PUERTO_ESCUCHA");
	IP_MEMORIA = config_get_string_value(CONFIG_KERNEL, "IP_MEMORIA");
	PUERTO_MEMORIA = config_get_string_value(CONFIG_KERNEL, "PUERTO_MEMORIA");
	IP_CPU = config_get_string_value(CONFIG_KERNEL, "IP_CPU");
	PUERTO_CPU_DISPATCH = config_get_string_value(CONFIG_KERNEL, "PUERTO_CPU_DISPATCH");
	PUERTO_CPU_INTERRUPT = config_get_string_value(CONFIG_KERNEL, "PUERTO_CPU_INTERRUPT");
	ALGORITMO_PLANIFICACION = config_get_string_value(CONFIG_KERNEL, "ALGORITMO_PLANIFICACION");
	QUANTUM = config_get_int_value(CONFIG_KERNEL, "QUANTUM");
	RECURSOS = config_get_array_value(CONFIG_KERNEL, "RECURSOS");
	INSTANCIAS_RECURSOS = config_get_array_value(CONFIG_KERNEL, "INSTANCIAS_RECURSOS");
	GRADO_MULTIPROGRAMACION = config_get_int_value(CONFIG_KERNEL, "GRADO_MULTIPROGRAMACION");
	IP_KERNEL = config_get_string_value(CONFIG_KERNEL, "IP_KERNEL");
}

void iniciar_conexiones()
{
	// inicar server KERNEL
	fd_kernel = iniciar_servidor(LOGGER_KERNEL, "KERNEL", IP_KERNEL, PUERTO_ESCUCHA);
	//log_info(LOGGER_KERNEL, "Kernel listo para recibir clientes");

	// conexion como cliente a MEMORIA
	fd_kernel_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
	//enviar_mensaje("Mensaje de Kernel para memoria", fd_kernel_memoria);

	// conexion como cliente a CPU DISPATCH
	fd_kernel_cpu_dispatch = crear_conexion(IP_CPU, PUERTO_CPU_DISPATCH); // planificar la ejecucion de procesos
	//enviar_mensaje("Mensaje de Kernel para CPU DISPATCH", fd_kernel_cpu_dispatch);

	// conexion como cliente a CPU INTERRUPT
	fd_kernel_cpu_interrupt = crear_conexion(IP_CPU, PUERTO_CPU_INTERRUPT); // planificar la interrupcion de procesos
	//enviar_mensaje("Mensaje de Kernel para CPU INTERRUPT", fd_kernel_cpu_interrupt);

	// hilo servidor
	pthread_create(&hilo_server_kernel, NULL, (void *)escuchar_kernel, NULL);
	pthread_detach(hilo_server_kernel);
}

void escuchar_kernel()
{
	while (server_escuchar(LOGGER_KERNEL, "KERNEL", fd_kernel));
}