#include "../include/main.h"
#include "../include/planificador.h"


sem_t semProcesoListo;

int main()
{
	inicializar_config();
	log_info(LOGGER_KERNEL, "Iniciando Kernel...");

	// iniciar_planificador_corto_plazo();
	// iniciar_planificador_largo_plazo();

	// server KERNEL
	int fd_kernel = iniciar_servidor(LOGGER_KERNEL, "KERNEL", IP_KERNEL, PUERTO_ESCUCHA);
	log_info(LOGGER_KERNEL, "Kernel listo para recibir clientes");

	// conexion como cliente a MEMORIA
	int fd_kernel_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
	enviar_mensaje("Mensaje de Kernel para memoria", fd_kernel_memoria);

	// conexion como cliente a CPU DISPATCH
	int fd_kernel_cpu_dispatch = crear_conexion(IP_CPU, PUERTO_CPU_DISPATCH); // aqui vamos a planificar la ejecucion de procesos
	enviar_mensaje("Mensaje de Kernel para CPU", fd_kernel_cpu_dispatch);


	iniciar_listas_y_semaforos();

	id_PID = 1;

	
	inicializar_pcb();
	
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

int asignar_pid()
{
	int valor_pid;

	pthread_mutex_lock(&mutex_pid);
	valor_pid = id_PID;
	id_PID++;
	pthread_mutex_unlock(&mutex_pid);

	return valor_pid;
}


void inicializar_pcb(void* conexion)
{

	int pid_pcb = int asignar_pid;
    //bool exit = false;
    //intptr_t conexionConsola = (intptr_t) conexion;
    //t_paquete* paquete = get_paquete(conexionConsola, logger_kernel); 
    t_pcb* pcb = crear_pcb(pid_pcb, LISTO, QUANTUM); 
    //t_procesoMemoria* procesoDeMemoria = solicitarCreacionDeProcesoEnMemoria(pcb->pid);
	
    log_info(logger_kernel, "Se crea el proceso %d en NEW", pcb->pid);
    agregar_pcb_cola_nuevos(pcb);
    //destruir_paquete(paquete);
}