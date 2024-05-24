#include "../include/instrucciones_io.h"

void procesar_sleep(int socket_cliente, t_log *logger)
{
    log_info(logger, "Procesando instrucción sleep");
    int tiempo;
    recv(socket_cliente, &tiempo, sizeof(int), 0);
    log_info(logger, "Durmiendo %d segundos", tiempo);
    sleep(tiempo);
    log_info(logger, "Instrucción sleep finalizada");
    // TODO Envío que termine la instrucción de sleep a kernel
}

void aviso_de_confirmacion_instruccion(int server_socket, t_log *logger){}
void aviso_de_rechazo_instruccion(int server_socket, t_log *logger){}