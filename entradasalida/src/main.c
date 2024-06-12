#include "../include/main.h"

char *TIPO_INTERFAZ;
int TIEMPO_UNIDAD_TRABAJO;
char *IP_KERNEL;
char *PUERTO_KERNEL;
char *IP_MEMORIA;
char *PUERTO_MEMORIA;
char *PATH_BASE_DIALFS;
int BLOCK_SIZE;
int BLOCK_COUNT;
int fd_io_memoria;
int fd_io_kernel;

t_log *LOGGER_INPUT_OUTPUT;
t_config *CONFIG_INPUT_OUTPUT;
t_interfaz *interfaz_actual;

int main(int argc, char** argv)
{
    signal(SIGINT, manejador_signals);

    inicializar_config(argv[2]); //argv[2] es el path del archivo de configuracion
    log_debug(LOGGER_INPUT_OUTPUT, "Iniciando la interfaz %s de tipo %s", argv[1], TIPO_INTERFAZ);
    interfaz_actual = malloc(sizeof(t_interfaz)); //creamos la interfaz actual
    interfaz_actual->tamanio_nombre_interfaz = strlen(argv[1]) + 1; //argv[1] es el nombre de la interfaz
    interfaz_actual->nombre_interfaz = argv[1]; //asignamos el nombre de la interfaz

    if (strcmp(TIPO_INTERFAZ, "GENERICA") == 0) {
        interfaz_actual->tipo_interfaz = GENERICA;
    } else if (strcmp(TIPO_INTERFAZ, "STDIN") == 0) {
        interfaz_actual->tipo_interfaz = STDIN;
    } else if (strcmp(TIPO_INTERFAZ, "STDOUT") == 0) {
        interfaz_actual->tipo_interfaz = STDOUT;
    } else if (strcmp(TIPO_INTERFAZ, "DIALFS") == 0) {
        interfaz_actual->tipo_interfaz = DIALFS;
    } else {
        log_error(LOGGER_INPUT_OUTPUT, "Tipo de interfaz no reconocido");
    }

    
    iniciar_conexiones();
    procesar_conexion_IO(fd_io_kernel, LOGGER_INPUT_OUTPUT);

    finalizar_io();
}

void inicializar_config(char *config_path)
{
    LOGGER_INPUT_OUTPUT = iniciar_logger("entradasalida.log", "ENTRADA_SALIDA");
    CONFIG_INPUT_OUTPUT = iniciar_config(config_path, "ENTRADA_SALIDA");
    TIPO_INTERFAZ = config_get_string_value(CONFIG_INPUT_OUTPUT, "TIPO_INTERFAZ");
    TIEMPO_UNIDAD_TRABAJO = config_get_int_value(CONFIG_INPUT_OUTPUT, "TIEMPO_UNIDAD_TRABAJO");
    IP_KERNEL = config_get_string_value(CONFIG_INPUT_OUTPUT, "IP_KERNEL");
    PUERTO_KERNEL = config_get_string_value(CONFIG_INPUT_OUTPUT, "PUERTO_KERNEL");
    IP_MEMORIA = config_get_string_value(CONFIG_INPUT_OUTPUT, "IP_MEMORIA");
    PUERTO_MEMORIA = config_get_string_value(CONFIG_INPUT_OUTPUT, "PUERTO_MEMORIA");
    PATH_BASE_DIALFS = config_get_string_value(CONFIG_INPUT_OUTPUT, "PATH_BASE_DIALFS");
    BLOCK_SIZE = config_get_int_value(CONFIG_INPUT_OUTPUT, "BLOCK_SIZE");
    BLOCK_COUNT = config_get_int_value(CONFIG_INPUT_OUTPUT, "BLOCK_COUNT");
}

void iniciar_conexiones()
{   
    //conexion como cliente a MEMORIA
    fd_io_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
    
    //conexion como cliente a KERNEL
    fd_io_kernel = crear_conexion(IP_KERNEL, PUERTO_KERNEL);
    enviar_datos_interfaz(interfaz_actual, fd_io_kernel, CONEXION_INTERFAZ);
}

void finalizar_io()
{
    log_destroy(LOGGER_INPUT_OUTPUT);
    config_destroy(CONFIG_INPUT_OUTPUT);
    liberar_conexion(fd_io_memoria);
    liberar_conexion(fd_io_kernel);
}

void manejador_signals(int signum)
{
	switch (signum)
	{
	case SIGUSR1:
		log_trace(LOGGER_INPUT_OUTPUT, "Se recibio la señal SIGUSR1\n");
		break;

	case SIGUSR2:
		log_trace(LOGGER_INPUT_OUTPUT, "Se recibio la señal SIGUSR2\n");
		break;

	case SIGINT:
		log_trace(LOGGER_INPUT_OUTPUT, "Se recibio la señal SIGINT\n");
        log_trace(LOGGER_INPUT_OUTPUT, "Se recibió SIGINT, cerrando conexiones y liberando recursos...");
        enviar_datos_interfaz(interfaz_actual, fd_io_kernel, DESCONEXION_INTERFAZ);
        finalizar_io();
        exit(0);
		break;

	case SIGTERM:
		log_trace(LOGGER_INPUT_OUTPUT, "Se recibio la señal SIGTERM\n");
		break;

	default:
		log_trace(LOGGER_INPUT_OUTPUT, "Se recibio una señal no manejada\n");
		break;
	}
}