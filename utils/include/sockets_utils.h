#ifndef SOCKETS_UTILS_H_
#define SOCKETS_UTILS_H_

#include "./gestor.h"
#include "./sockets_client.h"
#include "./sockets_server.h"
#include "./contexto.h"

t_log *iniciar_logger(char *file_name, char *name);
t_config *iniciar_config(char *file_name, char *name);
void serializar_nuevo(t_paquete *paquete, int pid, int size, char *path);
void paquete(int conexion, t_log *logger);
void terminar_programa(int conexion, t_log *logger, t_config *config);

#endif /* SOCKETS_UTILS_H_ */