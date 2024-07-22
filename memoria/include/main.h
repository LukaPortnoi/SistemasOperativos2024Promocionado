#ifndef MAIN_H_
#define MAIN_H_

#include "./gestor.h"
#include "./comunicaciones.h"

void sighandler(int s);
void inicializar_config(char *arg);
void iniciar_memoria_usuario(void);
void iniciar_marcos(void);

#endif