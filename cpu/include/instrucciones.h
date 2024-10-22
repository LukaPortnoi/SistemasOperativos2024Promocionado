#ifndef INSTRUCCIONES_H_
#define INSTRUCCIONES_H_

#include "./gestor.h"
#include "./tlb.h"

void _set(char *registro, char *valor);
void _mov_in(char *registro, char *direc_logica, int socket);
void _mov_out(char *direc_logica, char *registro, int socket);
void _sum(char *registro_destino, char *registro_origen);
void _sub(char *registro_destino, char *registro_origen);
void _jnz(char *registro, char *instruccion);
void _resize(char *tamanioAReasignar);
void _copy_string(char *tamanio, int socket_cliente);
void _wait(char *recurso, int cliente_socket);
void _signal(char *recurso, int cliente_socket);
void _io_gen_sleep(char *interfaz, char *unidades_de_trabajo, int cliente_socket);
void _io_stdin_read(char *interfaz, char *direc_logica, char *tamanio, int cliente_socket);
void _io_stdout_write(char *interfaz, char *direc_logica, char *tamanio, int cliente_socket);
void _io_fs_create(char *interfaz, char *nombre_archivo, int cliente_socket);
void _io_fs_delete(char *interfaz, char *nombre_archivo, int cliente_socket);
void _io_fs_truncate(char *interfaz, char *nombre_archivo, char *tamanio, int cliente_socket);
void _io_fs_write(char *interfaz, char *nombre_archivo, char *direc_logica, char *tamanio, char *puntero_archivo, int cliente_socket);
void _io_fs_read(char *interfaz, char *nombre_archivo, char *direc_logica, char *tamanio, char *puntero_archivo, int cliente_socket);

// ENVIOS
void enviar_recurso(t_pcb *pcb, char *recurso, int cliente_socket, op_cod codigo_operacion);
void serializar_recurso(t_pcb *pcb, char *recurso, t_paquete *paquete);
void enviar_resize_a_memoria(t_pcb *pcb, uint32_t tamanioAReasignar);
void serializar_resize(t_pcb *pcb, uint32_t tamanioAReasignar, t_paquete *paquete);
char *recibir_valor_mov_in_memoria(int socket);
char *deserializar_valor_mov_in_memoria(t_buffer *buffer);
void enviar_valor_mov_in_cpu(t_list *Lista_direccionesFisica, int socket, uint32_t pid);
void serializar_datos_mov_in(t_paquete *paquete, t_list *lista_direcciones, uint32_t pid);
void serializar_datos_mov_out(t_paquete *paquete, t_list *Lista_direccionesFisica, void *valorObtenido, uint32_t pid, bool es8bits);
void enviar_valor_mov_out_cpu(t_list *Lista_direccionesFisica, void *valorObtenido, int socket, uint32_t pid, bool es8bits);
void enviar_datos_copy_string(t_list *Lista_direccionesFisica_escritura, t_list *Lista_direccionesFisica_lectura, uint32_t tamanio, int socket, uint32_t pid);
void serializar_datos_copy_string(t_paquete *paquete, t_list *Lista_direccionesFisica_escritura, t_list *Lista_direccionesFisica_lectura, uint32_t tamanio , uint32_t pid);
t_list *recibir_dato_de_memoria_movIn(int socket, t_log *logger, void **datoObtenido, t_list *Lista_direccionesFisica, int tamanio);
t_list *deserializar_dato_movIN(t_paquete *paquete, void **datoObtenido, t_list *direccionesFisicas, int tamanio);
t_list *recibir_dato_movIN(int socket_cliente, void  **datoObtenido, t_list *Lista_direccionesFisica, int tamanio);


// UTILS
uint32_t *get_registry32(char *registro);
uint8_t *get_registry8(char *registro);
char *instruccion_to_string(nombre_instruccion nombre);
bool revisar_registro(char *registro);
uint32_t obtener_tamanio_registro(char *registro);

#endif // INSTRUCCIONES_H_