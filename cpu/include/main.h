#ifndef MAIN_H_
#define MAIN_H_

#include "./gestor.h"
#include "./comunicaciones.h"
#include "./utils_cpu.h"

void inicializar_config(void);
void iniciar_conexiones(void);
void escuchar_interrupt(void);
void finalizar_conexiones_cpu();

#endif /* MAIN_H_ */