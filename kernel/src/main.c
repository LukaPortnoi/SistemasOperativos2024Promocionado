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
char *QUANTUM;
char **RECURSOS;
char **INSTANCIAS_RECURSOS;
int GRADO_MULTIPROGRAMACION;
char *IP_KERNEL;

int fd_kernel;
int fd_kernel_memoria;
int fd_kernel_cpu_dispatch;
int fd_kernel_cpu_interrupt;

int PID_GLOBAL = 1;

pthread_mutex_t procesosNuevosMutex;
pthread_mutex_t procesosListosMutex;
pthread_mutex_t procesosBloqueadosMutex;
pthread_mutex_t procesoAEjecutarMutex;
pthread_mutex_t procesosFinalizadosMutex;
pthread_mutex_t procesoMutex;
pthread_mutex_t procesosEnSistemaMutex;
pthread_mutex_t mutex_pid;

sem_t semMultiprogramacion;
sem_t semNuevo;
sem_t semExit;
sem_t semListos_Ready;
sem_t semReady;
sem_t semExec;
sem_t semDetener;
sem_t semBloqueado;
sem_t semFinalizado;

t_queue *colaNuevos;
t_queue *colaListos;
t_queue *colaBloqueados;
t_queue *colaTerminados;
t_queue *procesosEnSistema;

int main()
{
	inicializar_config();
	log_info(LOGGER_KERNEL, "Iniciando Kernel...");

	iniciar_colas_y_semaforos();
	iniciar_planificador_corto_plazo();
	iniciar_planificador_largo_plazo();

	// inicar server KERNEL
	fd_kernel = iniciar_servidor(LOGGER_KERNEL, "KERNEL", IP_KERNEL, PUERTO_ESCUCHA);
	log_info(LOGGER_KERNEL, "Kernel listo para recibir clientes");

	// conexion como cliente a MEMORIA
	fd_kernel_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
	enviar_mensaje("Mensaje de Kernel para memoria", fd_kernel_memoria);

	// conexion como cliente a CPU DISPATCH
	fd_kernel_cpu_dispatch = crear_conexion(IP_CPU, PUERTO_CPU_DISPATCH); // aqui vamos a planificar la ejecucion de procesos
	enviar_mensaje("Mensaje de Kernel para CPU DISPATCH", fd_kernel_cpu_dispatch);

	// conexion como cliente a CPU INTERRUPT
	fd_kernel_cpu_interrupt = crear_conexion(IP_CPU, PUERTO_CPU_INTERRUPT); // aqui vamos a planificar la interrupcion de procesos
	enviar_mensaje("Mensaje de Kernel para CPU INTERRUPT", fd_kernel_cpu_interrupt);

	iniciar_consola_interactiva(LOGGER_KERNEL);

	while (server_escuchar(LOGGER_KERNEL, "KERNEL", fd_kernel));

	log_info(LOGGER_KERNEL, "Finalizando Kernel...");

	terminar_programa(fd_kernel, LOGGER_KERNEL, CONFIG_KERNEL);
}

void inicializar_config(void)
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
	QUANTUM = config_get_string_value(CONFIG_KERNEL, "QUANTUM");
	RECURSOS = config_get_array_value(CONFIG_KERNEL, "RECURSOS");
	INSTANCIAS_RECURSOS = config_get_array_value(CONFIG_KERNEL, "INSTANCIAS_RECURSOS");
	GRADO_MULTIPROGRAMACION = config_get_int_value(CONFIG_KERNEL, "GRADO_MULTIPROGRAMACION");
	IP_KERNEL = config_get_string_value(CONFIG_KERNEL, "IP_KERNEL");
}

