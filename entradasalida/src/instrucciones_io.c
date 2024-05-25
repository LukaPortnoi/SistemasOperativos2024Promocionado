#include "../include/instrucciones_io.h"

void procesar_sleep(int socket_cliente, t_log *logger)
{
    log_info(logger, "Procesando instrucción sleep");
    int tiempo;
    recv(socket_cliente, &tiempo, sizeof(int), 0);
    log_info(logger, "Durmiendo %d segundos", tiempo);
    sleep(tiempo);
    log_info(logger, "Instrucción sleep finalizada");
    // Le aviso al cliente que terminé
    bool terminado = true;
    send(socket_cliente, &terminado, sizeof(bool), 0);
}

void aviso_de_confirmacion_instruccion(int server_socket, t_log *logger){
    //tengoo que mandarle un bool confirmando que puedo seguir con la instruccion
    bool confirmacion = true;
    send(server_socket, &confirmacion, sizeof(bool), 0);
}
void aviso_de_rechazo_instruccion(int server_socket, t_log *logger){
    //tengoo que mandarle un bool confirmando que puedo seguir con la instruccion
    bool confirmacion = false;
    send(server_socket, &confirmacion, sizeof(bool), 0);
}