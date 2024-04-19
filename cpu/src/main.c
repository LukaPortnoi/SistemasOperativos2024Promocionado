#include "../include/main.h"

int main(int argc, char* argv[]) 
{
    inicializar_config();
    log_info(LOGGER_CPU, "Iniciando CPU...");

    //server CPU DISPATCH
    int fd_cpu_dispatch = iniciar_servidor(LOGGER_CPU, "CPU - DISPATCH", IP_CPU, PUERTO_ESCUCHA_DISPATCH);
    log_info(LOGGER_CPU, "CPU listo para recibir cliente en DISPATCH");

    //server CPU INTERRUPT
    int fd_cpu_interrupt = iniciar_servidor(LOGGER_CPU, "CPU - INTERRUPT", IP_CPU, PUERTO_ESCUCHA_INTERRUPT);
    log_info(LOGGER_CPU, "CPU listo para recibir cliente en INTERRUPT");
    
    //conexion como cliente a MEMORIA
    int fd_cpu_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
    //enviar_mensaje("Mensaje de CPU para memoria", CONEXION_CPU_MEMORIA);
    enviar_con_handshake(fd_cpu_memoria, "Hola desde CPU con handshake");
    paquete(fd_cpu_memoria, LOGGER_CPU);
    
    while(server_escuchar(LOGGER_CPU, "CPU", fd_cpu_dispatch));
    while(server_escuchar(LOGGER_CPU, "CPU", fd_cpu_interrupt));

    terminar_programa(fd_cpu_dispatch, LOGGER_CPU, CONFIG);
    terminar_programa(fd_cpu_interrupt, LOGGER_CPU, CONFIG);
}

void inicializar_config(){
    LOGGER_CPU = iniciar_logger("cpu.log", "CPU");
    CONFIG = iniciar_config("./cpu.config", "CPU");
    IP_MEMORIA = config_get_string_value(CONFIG, "IP_MEMORIA");
    IP_CPU = config_get_string_value(CONFIG, "IP_CPU");
    PUERTO_MEMORIA = config_get_string_value(CONFIG, "PUERTO_MEMORIA");
    PUERTO_ESCUCHA_DISPATCH = config_get_string_value(CONFIG, "PUERTO_ESCUCHA_DISPATCH");
    PUERTO_ESCUCHA_INTERRUPT = config_get_string_value(CONFIG, "PUERTO_ESCUCHA_INTERRUPT");
    CANTIDAD_ENTRADAS_TLB = config_get_string_value(CONFIG, "CANTIDAD_ENTRADAS_TLB");
    ALGORITMO_TLB = config_get_string_value(CONFIG, "ALGORITMO_TLB");
}