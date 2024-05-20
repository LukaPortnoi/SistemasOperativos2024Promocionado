#ifndef IO_H_
#define IO_H_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <commons/log.h>
#include <commons/config.h>
#include <commons/temporal.h>
#include <assert.h>
#include <pthread.h>

#include "sockets_client.h"
#include "sockets_server.h"
#include "sockets_utils.h"
#include "contexto.h"
#include "pcb.h"

struct interfaz{
    
}

void enviar_interfaz_IO(char *interfaz, int unidades_de_trabajo , int socket_cliente);
void serializar_IO_instruccion(t_paquete *paquete, uint32_t unidades_de_trabajo, char *interfaz);

#endif //IO_H_