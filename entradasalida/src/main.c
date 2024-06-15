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

int main(int argc, char **argv)
{
    signal(SIGINT, manejador_signals);
    char *config_path = agregar_prefijo_y_extension(argv[1]);
    inicializar_config(config_path);
    char *nombre_interfaz = extraer_nombre_interfaz(config_path);
    eliminar_extension(nombre_interfaz);
    interfaz_actual = malloc(sizeof(t_interfaz));                           // creamos la interfaz actual
    interfaz_actual->tamanio_nombre_interfaz = strlen(nombre_interfaz) + 1; // argv[1] es el nombre de la interfaz
    interfaz_actual->nombre_interfaz = nombre_interfaz;                     // asignamos el nombre de la interfaz
    log_debug(LOGGER_INPUT_OUTPUT, "Iniciando la interfaz %s de tipo %s", nombre_interfaz, TIPO_INTERFAZ);

    asignar_tipo_interfaz(TIPO_INTERFAZ);

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
    // conexion como cliente a MEMORIA
    fd_io_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);

    // conexion como cliente a KERNEL
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

void eliminar_extension(char *nombre_archivo)
{
    // Encuentra la última aparición de ".config" en el nombre del archivo
    char *pos = strstr(nombre_archivo, ".config");
    if (pos != NULL)
    {
        // Verifica que ".config" esté al final del string
        if (strcmp(pos, ".config") == 0)
        {
            *pos = '\0';
        }
    }
}

char *extraer_nombre_interfaz(char *path)
{
    char *nombre_interfaz = strrchr(path, '/');
    return nombre_interfaz ? nombre_interfaz + 1 : path;
}

char *agregar_prefijo_y_extension(const char *nombre_interfaz)
{
    const char *prefijo = "./cfgs/";
    const char *extension = ".config";

    // Calcular el nuevo tamaño del string con el prefijo y la extensión añadidos
    size_t new_size = strlen(prefijo) + strlen(nombre_interfaz) + strlen(extension) + 1;
    char *new_path = malloc(new_size);
    if (new_path == NULL)
    {
        fprintf(stderr, "Error de memoria.\n");
        exit(1); // Terminar el programa si no hay suficiente memoria
    }

    // Construir la nueva ruta con el prefijo y la extensión añadidos
    strcpy(new_path, prefijo);
    strcat(new_path, nombre_interfaz);
    strcat(new_path, extension);

    return new_path;
}

void asignar_tipo_interfaz(const char *t_interfaz)
{
    if (strcmp(t_interfaz, "GENERICA") == 0)
    {
        interfaz_actual->tipo_interfaz = GENERICA;
    }
    else if (strcmp(t_interfaz, "STDIN") == 0)
    {
        interfaz_actual->tipo_interfaz = STDIN;
    }
    else if (strcmp(t_interfaz, "STDOUT") == 0)
    {
        interfaz_actual->tipo_interfaz = STDOUT;
    }
    else if (strcmp(t_interfaz, "DIALFS") == 0)
    {
        interfaz_actual->tipo_interfaz = DIALFS;
    }
    else
    {
        log_error(LOGGER_INPUT_OUTPUT, "Tipo de interfaz no reconocido");
    }
}