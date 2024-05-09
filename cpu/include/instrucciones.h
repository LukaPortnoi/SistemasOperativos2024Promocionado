#ifndef INSTRUCCIONES_H_
#define INSTRUCCIONES_H_

#include "./gestor.h"

void _set(char *registro, char *valor);
void _mov_in(char *registro, char *direc_logica);
void _mov_out(char *direc_logica, char *registro);
void _sum(char *registro_destino, char *registro_origen);
void _sub(char *registro_destino, char *registro_origen);
void _jnz(char *registro, char *instruccion);
void _io_gen_sleep(char *interfaz, int unidades_de_trabajo);
uint32_t *get_registry(char *registro);
char *instruccion_to_string(nombre_instruccion nombre);
#endif // INSTRUCCIONES_H_