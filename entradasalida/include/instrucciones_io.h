#ifndef INSTRUCCIONES_IO_H_
#define INSTRUCCIONES_IO_H_

#include "./gestor.h"

void procesar_sleep(int server_socket, t_log *logger);
void aviso_de_confirmacion_instruccion(int server_socket, t_log *logger);
void aviso_de_rechazo_instruccion(int server_socket, t_log *logger);
void procesar_stdin(int socket_cliente, t_log *logger);
char *procesarIngresoUsuario(uint32_t tamanioMaximo);
void procesar_stdout(int socket_cliente, t_log *logger);
void procesar_dialfs_create(int socket_cliente, t_log *logger);
void procesar_dialfs_delete(int socket_cliente, t_log *logger);
void procesar_dialfs_truncate(int socket_cliente, t_log *logger);
void procesar_dialfs_write(int socket_cliente, t_log *logger);
void procesar_dialfs_read(int socket_cliente, t_log *logger);

#endif // INSTRUCCIONES_IO_H_