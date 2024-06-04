#ifndef UTILS_MEMORIA_H
#define UTILS_MEMORIA_H

#include "./gestor.h"
#include "../../utils/include/contexto.h"
#include "./manejo_memoria.h"

t_proceso_memoria *recibir_proceso_memoria(int socket);
t_proceso_memoria *deserializar_proceso(t_buffer *buffer);
void recibir_pedido_instruccion(uint32_t *pid, uint32_t *pc, int socket);
t_proceso_memoria *obtener_proceso_pid(uint32_t pid_pedido);
t_instruccion *obtener_instruccion_del_proceso_pc(t_proceso_memoria *proceso, uint32_t pc);
char *instruccion_to_string(nombre_instruccion nombre);
void enviar_instruccion(int socket, t_instruccion *instruccion);
t_paquete *crear_paquete_Instruccion(t_instruccion *instruccion);
void agregar_a_paquete_Instruccion(t_paquete *paquete, t_instruccion *instruccion);
t_buffer *crear_buffer_instruccion(t_instruccion *instruccion);
t_proceso_memoria *iniciar_proceso_path(t_proceso_memoria *proceso_nuevo);
t_list *parsear_instrucciones(char *path);
t_instruccion *armar_estructura_instruccion(nombre_instruccion instruccion, char *parametro1, char *parametro2);
char *leer_archivo(char *path);
void deserializar_pedido_instruccion(uint32_t *pid, uint32_t *pc, t_buffer *buffer);
void recibir_pedido_resize(uint32_t *pid, uint32_t *nueva_cantidad_paginas, int socket);
void deserializar_pedido_resize(uint32_t *pid, uint32_t *nueva_cantidad_paginas, t_buffer *buffer);
void iniciar_semaforos();
void enviar_valor_mov_in_cpu(uint32_t valor, int socket);
uint32_t recibir_mov_in_cpu(int socket_cliente, uint32_t *direccion_fisica);
void recibir_pedido_marco(uint32_t *pagina , uint32_t *pid_proceso, int socket);
void deserializar_pedido_marco(uint32_t *pagina, uint32_t *pid_proceso, t_buffer *buffer);
void enviar_marco(int socket, uint32_t marco);
void serializar_marco(t_paquete *paquete, uint32_t marco);

#endif // UTILS_MEMORIA_H