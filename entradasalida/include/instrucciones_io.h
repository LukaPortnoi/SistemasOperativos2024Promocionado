#ifndef INSTRUCCIONES_IO_H_
#define INSTRUCCIONES_IO_H_

#include "./gestor.h"
#include "./dialfs.h"

void procesar_sleep(int server_socket);
void aviso_de_confirmacion_instruccion(int server_socket);
void aviso_de_rechazo_instruccion(int server_socket);
void procesar_stdin(int socket_cliente);
char *procesarIngresoUsuario(uint32_t tamanioMaximo);
void procesar_stdout(int socket_cliente);
void procesar_dialfs_create(int socket_cliente);
void procesar_dialfs_delete(int socket_cliente);
void procesar_dialfs_truncate(int socket_cliente);
void procesar_dialfs_write(int socket_cliente);
void procesar_dialfs_read(int socket_cliente);

#endif // INSTRUCCIONES_IO_H_