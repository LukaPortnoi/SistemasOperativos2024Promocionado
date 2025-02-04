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
int RETRASO_COMPACTACION;

int fd_io_memoria;
int fd_io_kernel;

t_log *LOGGER_INPUT_OUTPUT;
t_config *CONFIG_INPUT_OUTPUT;
t_interfaz *interfaz_actual;

char BITMAP_PATH[256];
char BLOQUES_PATH[256];
t_list *ARCHIVOS_EN_FS;

int main(int argc, char **argv)
{
    signal(SIGINT, manejador_signals);
    iniciar_io(argv[1]);
    iniciar_conexiones();
    procesar_conexion_IO(fd_io_kernel);
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

void iniciar_io(char *arg)
{
    char *config_path = agregar_prefijo_y_extension(arg);
    inicializar_config(config_path);
    char *nombre_interfaz = extraer_nombre_interfaz(config_path);
    eliminar_extension(nombre_interfaz);
    interfaz_actual = crear_interfaz(nombre_interfaz, obtener_tipo_interfaz(TIPO_INTERFAZ));
    log_info(LOGGER_INPUT_OUTPUT, "Interfaz %s - %s iniciada", interfaz_actual->nombre_interfaz, TIPO_INTERFAZ);

    if (interfaz_actual->tipo_interfaz == DIALFS)
    {
        RETRASO_COMPACTACION = config_get_int_value(CONFIG_INPUT_OUTPUT, "RETRASO_COMPACTACION");
        snprintf(BITMAP_PATH, sizeof(BITMAP_PATH), "%s/bitmap.dat", PATH_BASE_DIALFS);
        snprintf(BLOQUES_PATH, sizeof(BLOQUES_PATH), "%s/bloques.dat", PATH_BASE_DIALFS);
        ARCHIVOS_EN_FS = list_create();
        manejar_archivos_fs();
        actualizar_lista_archivos_en_fs();
        log_debug(LOGGER_INPUT_OUTPUT, "Tamaño de lista de archivos: %d", list_size(ARCHIVOS_EN_FS));
    }
}

void finalizar_io()
{
    if (interfaz_actual->tipo_interfaz == DIALFS)
    {
        list_destroy_and_destroy_elements(ARCHIVOS_EN_FS, (void (*)(void *))destruir_archivo);
    }
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
    const char *prefijo = "./config/";
    const char *extension = ".config";

    // Calcular el nuevo tamaño del string con el prefijo y la extensión añadidos
    size_t new_size = strlen(prefijo) + strlen(nombre_interfaz) + strlen(extension) + 1;
    char *new_path = malloc(new_size);
    if (new_path == NULL)
    {
        log_error(LOGGER_INPUT_OUTPUT, "Error de memoria al agregar prefijo y extensión al nombre de la interfaz");
        exit(1); // Terminar el programa si no hay suficiente memoria
    }

    // Construir la nueva ruta con el prefijo y la extensión añadidos
    strcpy(new_path, prefijo);
    strcat(new_path, nombre_interfaz);
    strcat(new_path, extension);

    return new_path;
}

t_tipo_interfaz obtener_tipo_interfaz(const char *t_interfaz)
{
    if (strcmp(t_interfaz, "GENERICA") == 0)
    {
        return GENERICA;
    }
    else if (strcmp(t_interfaz, "STDIN") == 0)
    {
        return STDIN;
    }
    else if (strcmp(t_interfaz, "STDOUT") == 0)
    {
        return STDOUT;
    }
    else if (strcmp(t_interfaz, "DIALFS") == 0)
    {
        return DIALFS;
    }
    else
    {
        log_error(LOGGER_INPUT_OUTPUT, "Tipo de interfaz no reconocido");
        return -1;
    }
}