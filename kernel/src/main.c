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

// GLOBALES PARA MANEJO DE RECURSOS
t_list *RECURSOS_DISPONIBLES;
char *RECURSO_A_USAR;
nombre_instruccion INSTRUCCION_RECURSO_A_USAR;

// GLOBALES PARA MANEJO DE INTERFACES
char *nombre_interfaz;
nombre_instruccion instruccion_de_IO_a_ejecutar;

// PARA INTERFAZ GENERICA
int unidades_de_trabajo;

// PARA INTERFAZ STDIN/STDOUT
t_list *direcciones_fisicas;

// PARA INTERFAZ DIALFS
char *nombre_archivo;
char *direccion_logica_fs;
char *direccion_fisica_fs;
uint32_t tamanio_fs;
uint32_t tamanio_fs_recibir;
char *puntero_fs;

t_pcb *pcb_ejecutandose;
t_pcb *pcb_a_finalizar = NULL;

pthread_t hilo_server_kernel;

int main()
{
	inicializar_config();
	iniciar_conexiones();
	iniciar_colas_y_semaforos();
	iniciar_planificador_largo_plazo();
	iniciar_planificador_corto_plazo();
	iniciar_consola_interactiva();
	finalizar_kernel();
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

	// conexion como cliente a MEMORIA
	fd_kernel_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);

	// conexion como cliente a CPU DISPATCH
	fd_kernel_cpu_dispatch = crear_conexion(IP_CPU, PUERTO_CPU_DISPATCH);

	// conexion como cliente a CPU INTERRUPT
	fd_kernel_cpu_interrupt = crear_conexion(IP_CPU, PUERTO_CPU_INTERRUPT);

	// hilo servidor
	pthread_create(&hilo_server_kernel, NULL, (void *)escuchar_kernel, NULL);
	pthread_detach(hilo_server_kernel);
}

void escuchar_kernel()
{
	while (server_escuchar(LOGGER_KERNEL, "KERNEL", fd_kernel));
}

void finalizar_kernel()
{
	log_destroy(LOGGER_KERNEL);
	config_destroy(CONFIG_KERNEL);
	liberar_conexion(fd_kernel);
	liberar_conexion(fd_kernel_memoria);
	liberar_conexion(fd_kernel_cpu_dispatch);
	liberar_conexion(fd_kernel_cpu_interrupt);
}