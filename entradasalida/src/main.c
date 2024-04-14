#include "../include/main.h"

char* tipo_interfaz
int tiempo_unidad_trabajo
char* ip_kernel
int puerto_kernel
char* ip_memoria
int puerto_memoria
char* path_base_dialfs
int block_size
int block_count

t_log *logger_input_output;
t_config *config_input_output;

int main(int argc, char* argv[]) {
    
    inicializar_config();

    log_info(logger_input_output, "Iniciando Entradas/Salidas...");

}

void inicializar_config(void)
{
    logger_input_output = iniciar_logger("entradasalida.log", "ENTRADA_SALIDA");
    config_input_output = iniciar_config("./entradasalida.config", "ENTRADA_SALIDA");

    tipo_interfaz = config_get_string_value(config_input_output, "TIPO_INTERFAZ");
    tiempo_unidad_trabajo = config_get_int_value(config_input_output, "TIEMPO_UNIDAD_TRABAJO");
    ip_kernel = config_get_string_value(config_input_output, "IP_KERNEL");
    puerto_kernel = config_get_int_value(config_input_output, "PUERTO_KERNEL");
    ip_memoria = config_get_string_value(config_input_output, "IP_MEMORIA");
    puerto_memoria = config_get_int_value(config_input_output, "PUERTO_MEMORIA");
    path_base_dialfs = config_get_string_value(config_input_output, "PATH_BASE_DIALFS");
    block_size = config_get_int_value(config_input_output, "BLOCK_SIZE");
    block_count = config_get_int_value(config_input_output, "BLOCK_COUNT");
}
