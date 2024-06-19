#ifndef UTILS_MEMORIA_H
#define UTILS_MEMORIA_H

#include "./gestor.h"
#include "../../utils/include/contexto.h"
#include "./manejo_memoria.h"
#include <math.h>

t_proceso_memoria *recibir_proceso_memoria(int socket);
t_proceso_memoria *deserializar_proceso(t_buffer *buffer);
void recibir_pedido_instruccion(uint32_t *pid, uint32_t *pc, int socket);
t_proceso_memoria *obtener_proceso_pid(uint32_t pid_pedido);
t_instruccion *obtener_instruccion_del_proceso_pc(t_proceso_memoria *proceso, uint32_t pc);
char *instruccion_to_string(nombre_instruccion nombre);
void enviar_instruccion(int socket, t_instruccion *instruccion);
void serializar_instruccion(t_paquete *paquete, t_instruccion *instruccion);
t_proceso_memoria *iniciar_proceso_path(t_proceso_memoria *proceso_nuevo);
t_list *parsear_instrucciones(char *path);
t_instruccion *armar_estructura_instruccion(nombre_instruccion instruccion, char *parametro1, char *parametro2, char *parametro3, char *parametro4, char *parametro5);
char *leer_archivo(char *path);
void deserializar_pedido_instruccion(uint32_t *pid, uint32_t *pc, t_buffer *buffer);
void recibir_pedido_resize(uint32_t *pid, uint32_t *nueva_cantidad_paginas, int socket);
void deserializar_pedido_resize(uint32_t *pid, uint32_t *nueva_cantidad_paginas, t_buffer *buffer);
void iniciar_semaforos();
void recibir_pedido_marco(uint32_t *pagina, uint32_t *pid_proceso, int socket);
void deserializar_pedido_marco(uint32_t *pagina, uint32_t *pid_proceso, t_buffer *buffer);
void enviar_marco(int socket, uint32_t marco);
void serializar_marco(t_paquete *paquete, uint32_t marco);
void enviar_valor_mov_in_memoria(char *valor, int socket);
void serializar_valor_leido_mov_in(t_paquete *paquete, char *valor);
void recibir_copystring(int socket_cliente, t_list *Lista_direccionesFisica_escritura, t_list *Lista_direccionesFisica_lectura, uint32_t *tamanio);
void deserializar_datos_copystring(t_paquete *paquete, t_list *Lista_direccionesFisica_escritura, t_list *Lista_direccionesFisica_lectura, uint32_t *tamanio);
char *leer_memoria_IO(uint32_t dir_fisica, uint32_t tamanio_registro);

void escribir_memoria(uint32_t dir_fisica, uint32_t tamanio_registro, char *valorObtenido);

void escribir_memoria_mov_out(uint32_t dir_fisica, uint32_t tamanio_registro, char *valorObtenido);

char *leer_memoria(uint32_t dir_fisica, uint32_t tamanio_registro);
char *int_to_char(int num);
char *concatenar_lista_de_cadenas(t_list *lista, int tamanio);
char* decimal_a_binario(int numero);
int binario_a_decimal(int binario); 

// mov in y mov out
void recibir_mov_in_cpu(int socket_cliente, t_list *lista_direcciones);
void deserializar_datos_mov_in(t_paquete *paquete, t_list *lista_direcciones);
void recibir_mov_out_cpu(t_list *lista_direcciones, uint32_t *valorObtenido, int cliente_socket);
void deserializar_datos_mov_out(t_paquete *paquete, t_list *lista_direcciones, uint32_t *valorObtenido);

#endif // UTILS_MEMORIA_H