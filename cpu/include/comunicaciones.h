#ifndef COMUNICACIONES_H_
#define COMUNICACIONES_H_

#include "./gestor.h"
#include "./utils_cpu.h"

int server_escuchar(t_log *logger, char *server_name, int server_socket);
bool recibir_interrupciones(void);

#endif /* COMUNICACIONES_H_ */