#ifndef COMUNICACIONES_H_
#define COMUNICACIONES_H_

#include "./gestor.h"
#include "./utils_cpu.h"

int server_escuchar(t_log *logger, char *server_name, int server_socket);

extern bool interrupciones[5];

void recibir_interrupciones(t_log *logger);
void limpiar_interrupciones(void);
bool hayInterrupciones(void);

#endif /* COMUNICACIONES_H_ */