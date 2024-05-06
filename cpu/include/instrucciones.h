#ifndef INSTRUCCIONES_H_
#define INSTRUCCIONES_H_

#include <stdlib.h>
#include <stdio.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <readline/readline.h>

#include "../../utils/include/sockets_client.h"
#include "../../utils/include/sockets_server.h"
#include "../../utils/include/pcb.h"
#include "./gestor.h"


int server_escuchar(t_log *logger, char *server_name, int server_socket);

void _set(char *registro, char *valor);
void _sum(char *registro_destino, char *registro_origen);
void _sub(char *registro_destino, char *registro_origen);
void _jnz(char *registro, char *instruccion);
void _sleep(void);
void _wait(void);
void _signal(void);
void _mov_in(char *registro, char *direc_logica);
void _mov_out(char *direc_logica, char *registro);
void _f_open(char *nombre_archivo, char *modo_apertura);
void _f_close(char *nombre_archivo);
void _f_seek(char *nombre_archivo, char *posicion);
void _f_read(char *nombre_archivo, char *direc_logica);
void _f_write(char *nombre_archivo, char *direc_logica);
void _f_truncate(char *nombre_archivo, char *tamanio);
void __exit(void);
void traducir_dl_fs(char *dl);
uint32_t *get_registry(char *registro);

#endif // INSTRUCCIONES_H_