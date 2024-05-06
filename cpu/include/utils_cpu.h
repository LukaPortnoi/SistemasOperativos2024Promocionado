#ifndef UTILS_CPU_H_
#define UTILS_CPU_H_

#include "./gestor.h"
#include "./comunicaciones.h"
#include "./instrucciones.h"

void ejecutar_ciclo_instruccion();
t_instruccion *fetch(int pid, int pc);
void execute(t_instruccion *instruccion);
void pedir_instruccion_memoria(int pid, int pc, int socket);
t_instruccion *deserializar_instruccion(int socket);

#endif // UTILS_CPU_H_