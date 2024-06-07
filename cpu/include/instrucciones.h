#ifndef INSTRUCCIONES_H_
#define INSTRUCCIONES_H_

#include "./gestor.h"
#include "./tlb.h"

void _set(char *registro, char *valor);
void _mov_in(char *registro, char *direc_logica);
void _mov_out(char *direc_logica, char *registro);
void _sum(char *registro_destino, char *registro_origen);
void _sub(char *registro_destino, char *registro_origen);
void _jnz(char *registro, char *instruccion);
void _resize(char *tamanioAReasignar);
void _copy_string(char *tamanio);
void _wait(char *recurso);
void _signal(char *recurso);
void _io_gen_sleep(char *interfaz, char *unidades_de_trabajo, int cliente_socket);
void _io_stdin_read(char *interfaz, char *direc_logica, char *tamanio, int cliente_socket);

// ENVIOS
void enviar_recurso(t_pcb *pcb, char *recurso, int cliente_socket, op_cod codigo_operacion);
void serializar_recurso(t_pcb *pcb, char *recurso, t_paquete *paquete);
void enviar_resize_a_memoria(t_pcb *pcb, uint32_t tamanioAReasignar);
void serializar_resize(t_pcb *pcb, uint32_t tamanioAReasignar, t_paquete *paquete);

// UTILS
uint32_t *get_registry32(char *registro);
uint8_t *get_registry8(char *registro);
char *instruccion_to_string(nombre_instruccion nombre);
bool revisar_registro(char *registro);

#endif // INSTRUCCIONES_H_