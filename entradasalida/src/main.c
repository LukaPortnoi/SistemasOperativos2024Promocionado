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
interfaz *interfaz_actual;

int main(int argc, char** argv)
{
    inicializar_config(argv[2]);
    log_info(LOGGER_INPUT_OUTPUT, "Iniciando la interfaz I/O %s de tipo %s", argv[1], TIPO_INTERFAZ);
    interfaz_actual = malloc(sizeof(interfaz));
    interfaz_actual->longitud_nombre_interfaz = strlen(argv[1]) + 1;
    interfaz_actual->nombre_interfaz = argv[1];
    interfaz_actual->longitud_tipo_interfaz = strlen(TIPO_INTERFAZ) + 1;
    interfaz_actual->tipo_interfaz = TIPO_INTERFAZ;
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
    //enviar_mensaje("Mensaje de I/O para memoria", fd_io_memoria);

    //conexion como cliente a KERNEL
    fd_io_kernel = crear_conexion(IP_KERNEL, PUERTO_KERNEL);
    enviar_datos_interfaz(interfaz_actual, fd_io_kernel);
    //enviar_mensaje("Mensaje de I/O para KERNEL", fd_io_kernel);
}

void finalizar_io()
{
    log_destroy(LOGGER_INPUT_OUTPUT);
    config_destroy(CONFIG_INPUT_OUTPUT);
    liberar_conexion(fd_io_memoria);
    liberar_conexion(fd_io_kernel);
}
