#ifndef INSTRUCCIONES_IO_H_
#define INSTRUCCIONES_IO_H_

#include "./gestor.h"

void procesar_sleep(int server_socket, t_log *logger);
void aviso_de_confirmacion_instruccion(int server_socket, t_log *logger);
void aviso_de_rechazo_instruccion(int server_socket, t_log *logger);
void procesar_stdin(int socket_cliente, t_log *logger);
char *procesarIngresoUsuario(uint32_t tamanioMaximo);
void imprimir_dato_recibido_de_memoria(char *dato);

#endif // INSTRUCCIONES_IO_H_