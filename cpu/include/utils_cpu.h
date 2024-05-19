#ifndef UTILS_CPU_H_
#define UTILS_CPU_H_

#include "./gestor.h"
#include "./comunicaciones.h"
#include "./instrucciones.h"

void ejecutar_ciclo_instruccion();
t_instruccion *fetch(uint32_t pid, uint32_t pc);
void execute(t_instruccion *instruccion);
void loguear_y_sumar_pc(t_instruccion *instruccion);
void pedir_instruccion_memoria(uint32_t pid, uint32_t pc, int socket);
t_instruccion *deserializar_instruccion(int socket);
void log_instruccion_ejecutada(nombre_instruccion nombre, char *param1, char *param2);
void iniciar_semaforos_etc();
#endif // UTILS_CPU_H_