#include "../include/main.h"

int conexion_memoria;
int conexion_kernel;

char *tipo_interfaz;
int tiempo_unidad_trabajo;
char *ip_kernel;
char *puerto_kernel;
char *ip_memoria;
char *puerto_memoria;
char *path_base_dialfs;
int block_size;
int block_count;

t_log *logger_input_output;
t_config *config_input_output;

int main(int argc, char *argv[])
{
    inicializar_config();

    log_info(logger_input_output, "Iniciando Entradas/Salidas...");

    conexion_memoria = crear_conexion(ip_memoria, puerto_memoria);
    enviar_mensaje("Mensaje de I/O para memoria", conexion_memoria);
    paquete(conexion_memoria, logger_input_output);

    conexion_kernel = crear_conexion(ip_kernel, puerto_kernel);
    enviar_mensaje("Mensaje de I/O para Kernel", conexion_kernel);
    paquete(conexion_kernel, logger_input_output);

    terminar_programa(conexion_kernel, logger_input_output, config_input_output);
}

void inicializar_config(void)
{
    logger_input_output = iniciar_logger("entradasalida.log", "ENTRADA_SALIDA");
    config_input_output = iniciar_config("./entradasalida.config", "ENTRADA_SALIDA");

    tipo_interfaz = config_get_string_value(config_input_output, "TIPO_INTERFAZ");
    tiempo_unidad_trabajo = config_get_int_value(config_input_output, "TIEMPO_UNIDAD_TRABAJO");
    ip_kernel = config_get_string_value(config_input_output, "IP_KERNEL");
    puerto_kernel = config_get_string_value(config_input_output, "PUERTO_KERNEL");
    ip_memoria = config_get_string_value(config_input_output, "IP_MEMORIA");
    puerto_memoria = config_get_string_value(config_input_output, "PUERTO_MEMORIA");
    path_base_dialfs = config_get_string_value(config_input_output, "PATH_BASE_DIALFS");
    block_size = config_get_int_value(config_input_output, "BLOCK_SIZE");
    block_count = config_get_int_value(config_input_output, "BLOCK_COUNT");
}
