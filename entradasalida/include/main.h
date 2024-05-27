#ifndef MAINH
#define MAINH

#include "./gestor.h"
#include "./comunicaciones.h"

void inicializar_config(char *config_path);
void iniciar_conexiones();
void manejador_signals(int signum);
void finalizar_io();

#endif // MAIN_H