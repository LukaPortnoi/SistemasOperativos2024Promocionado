#ifndef COMUNICACIONES_H_
#define COMUNICACIONES_H_

#include "./gestor.h"
#include "./utils_cpu.h"

extern bool interrupciones[5];

int server_escuchar(t_log *logger, char *server_name, int server_socket);
void recibir_interrupciones(t_log *logger);
void limpiar_interrupciones(void);
bool hayInterrupciones(void);

#endif /* COMUNICACIONES_H_ */