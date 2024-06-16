#ifndef MAIN_H_
#define MAIN_H_

#include "./gestor.h"
#include "./comunicaciones.h"
#include "./utils_cpu.h"
#include "./tlb.h"

void inicializar_config(void);
void iniciar_conexiones(void);
void escuchar_interrupt(void);
void manejador_signals(int signum);
void finalizar_cpu();
t_pcb* inicializar_pcb();

#endif /* MAIN_H_ */