#include "../include/main.h"

int main(int argc, char *argv[])
{
    inicializar_config();
    log_info(LOGGER_INPUT_OUTPUT, "Iniciando Entradas/Salidas...");

    //conexion como cliente a MEMORIA
    int fd_io_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
    enviar_con_handshake(fd_io_memoria, "Hola desde I/O con handshake");    
    paquete(fd_io_memoria, LOGGER_INPUT_OUTPUT);

    //conexion como cliente a KERNEL
    int fd_io_kernel = crear_conexion(IP_KERNEL, PUERTO_KERNEL);
    enviar_con_handshake(fd_io_kernel, "Hola desde I/O con handshake");        
    paquete(fd_io_kernel, LOGGER_INPUT_OUTPUT);

    terminar_programa(fd_io_memoria, LOGGER_INPUT_OUTPUT, CONFIG_INPUT_OUTPUT);
    terminar_programa(fd_io_kernel, LOGGER_INPUT_OUTPUT, CONFIG_INPUT_OUTPUT);
}

void inicializar_config(void)
{
    LOGGER_INPUT_OUTPUT = iniciar_logger("entradasalida.log", "ENTRADA_SALIDA");
    CONFIG_INPUT_OUTPUT = iniciar_config("./entradasalida.config", "ENTRADA_SALIDA");
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

void enviar_con_handshake(int socket_cliente, char* mensaje){
    t_paquete_handshake *paquete = malloc(sizeof(t_paquete_handshake));
    
    paquete->codigo_operacion = HANDSHAKE_in_out;
    paquete->buffer = malloc(sizeof(t_buffer));
    paquete->buffer->size = strlen(mensaje) + 1;
    paquete->buffer->stream = malloc(paquete->buffer->size);
    memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);
    int bytes = paquete->buffer->size + 2 * sizeof(int);
    void *a_enviar = serializar_paquete(paquete, bytes);
    send(socket_cliente, a_enviar, bytes, 0);
    free(a_enviar);
    eliminar_paquete(paquete);
}