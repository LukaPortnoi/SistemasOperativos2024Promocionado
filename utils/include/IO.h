#ifndef IO_H_
#define IO_H_

#include "./gestor.h"
#include "./pcb.h"
#include "./memoria.h"
typedef enum
{
    GENERICA,
    STDIN,
    STDOUT,
    DIALFS
} t_tipo_interfaz;

typedef struct
{
    char *nombre_interfaz;
    uint32_t tamanio_nombre_interfaz;
    t_tipo_interfaz tipo_interfaz;
} t_interfaz;

typedef struct
{
    int unidades_de_trabajo;
    uint32_t pidPcb;
    char *nombre_interfaz;
} t_interfaz_gen;

typedef struct
{
    uint32_t pidPcb;
    t_list *direccionesFisicas;
    char *nombre_interfaz;
} t_interfaz_stdin;

typedef struct
{
    uint32_t pidPcb;
    t_list *direccionesFisicas;
    char *nombre_interfaz;
} t_interfaz_stdout;

typedef struct
{
    uint32_t pidPcb;
    char *nombre_interfaz;
    char *nombre_archivo;
    uint32_t tamanio;
    uint32_t unidades_de_trabajo;
} t_interfaz_dialfs;

// dialfs
void destroyInterfazDialfs(t_interfaz_dialfs *interfaz);

// ENVIO RECEPCION CON DIALFS

//EN CPU
void enviar_fs_create_delete(t_pcb* pcb, char* interfaz, char* path, int socket, nombre_instruccion instruccion);   
void serializar_fs_create_delete(t_paquete* paquete, t_pcb* pcb, char* interfaz, char* path, nombre_instruccion instruccion); 

//EN KERNEL A IO
void enviar_interfaz_dialFS_create_delete(int socket, char *nombre_archivo, uint32_t pid, char *nombre_interfaz, nombre_instruccion instruccion);
void serializar_interfaz_dialFS_create_delete(t_paquete *paquete, char *nombre_archivo, uint32_t pid, char *nombre_interfaz);

//EN KERNEL DE IO
t_interfaz_dialfs *recibir_InterfazDialfs_iO_create_delete(int socket);
void deserializar_InterfazDialfs_iO_create_delete(t_buffer *buffer, t_interfaz_dialfs *interfaz);

//EN IO DE KERNEL
t_interfaz_dialfs *crearInterfazDialfs();
void recibir_InterfazDialfs(int socket, t_interfaz_dialfs *interfaz, op_cod codigo_operacion);
void deserializar_InterfazDialfs(t_buffer *buffer, t_interfaz_dialfs *interfaz, op_cod codigo_operacion);
void deserializar_interfaz_dialfs_create_delete(t_buffer *buffer, t_interfaz_dialfs *interfaz);

//EN IO A KERNEL
void enviar_create_delete_terminado(int socket, uint32_t pid, char *nombre_interfaz);
void serializar_create_delete_terminado(t_paquete *paquete, uint32_t pid, char *nombre_interfaz);

t_interfaz *crear_interfaz(char *nombre_interfaz, t_tipo_interfaz tipo_interfaz);
void destruir_interfaz(t_interfaz *interfaz);

// generica
void enviar_interfaz_IO(t_pcb *pcb_actual, char *interfaz, int unidades_de_trabajo, int socket_cliente, nombre_instruccion instruccion);
void serializar_IO_instruccion(t_paquete *paquete, t_pcb *pcb_actual, int unidades_de_trabajo, char *interfaz, nombre_instruccion instruccion);
void recibir_pcb_para_interfaz(t_pcb *pcb, int socket_cliente, char **nombre_interfaz, int *unidades_de_trabajo, nombre_instruccion *instruccion);
void deserializar_pcb_para_interfaz(t_pcb *pcb, t_buffer *buffer, char **nombre_interfaz, int *unidades_de_trabajo, nombre_instruccion *instruccion);
void enviar_datos_interfaz(t_interfaz *interfaz, int socket_server, op_cod codigo_operacion);
t_interfaz *recibir_datos_interfaz(int socket_cliente);
t_paquete *crear_paquete_interfaz(t_interfaz *interfaz, op_cod codigo_operacion);
t_buffer *crear_buffer_interfaz(t_interfaz *interfaz);
t_interfaz *deserializar_interfaz_recibida(t_buffer *buffer);
void enviar_InterfazGenerica(int socket, int unidades_trabajo, uint32_t pid, char *nombre_interfaz);
t_paquete *crear_paquete_InterfazGenerica(t_interfaz_gen *interfaz);
t_buffer *crear_buffer_InterfazGenerica(t_interfaz_gen *interfaz);
t_interfaz_gen *deserializar_InterfazGenerica(t_buffer *buffer);
t_interfaz_gen *recibir_InterfazGenerica(int socket);
void enviar_InterfazGenericaConCodigoOP(int socket, int unidades_trabajo, uint32_t pid, char *nombre_interfaz);
t_paquete *crear_paquete_InterfazGenericaCodOp(t_interfaz_gen *interfaz, op_cod codigo_operacion);

// stdin
void enviar_interfaz_IO_stdin(t_pcb *pcb_actual, char *interfaz, t_list *Lista_direccionesFisica, int socket_cliente, nombre_instruccion IO);
void serializar_IO_instruccion_stdin(t_paquete *paquete, t_pcb *pcb, char *interfaz, t_list *Lista_direccionesFisica, nombre_instruccion IO);
void recibir_pcb_para_interfaz_in_out(t_pcb *pcb, int socket_cliente, char **nombre_interfaz, t_list *direcciones_fisicas, nombre_instruccion *IO);
void deserializar_pcb_para_interfaz_stdin(t_pcb *pcb, t_buffer *buffer, char **nombre_interfaz, t_list *direcciones_fisicas, nombre_instruccion *IO);
void enviar_InterfazStdinConCodigoOPaKernel(int socket, t_list *direcciones_fisicas, uint32_t pid, char *nombre_interfaz);
t_interfaz_stdin *recibir_InterfazStdin(int socket);
t_interfaz_stdin *deserializar_InterfazStdin(t_buffer *buffer);
void enviar_dato_stdin(int socket, t_list *direcciones_fisicas, char *datoRecibido, uint32_t pid);
void serializar_dato_stdin(t_paquete *paquete, t_list *direcciones_fisicas, char *Dato_a_exscribir_en_memoria, uint32_t pid);
void serializarInterfazStdin_de_Kernale_a_Memoria(t_paquete *paquete, t_interfaz_stdin *interfaz);
void enviar_InterfazStdin(int socket, t_list *direcciones_fisicas, uint32_t pid, char *nombre_interfaz);
char *recibir_dato_stdin(t_list *direcciones_fisicas, int socket_cliente, uint32_t *pid);
char *deserializar_dato_interfaz_STDIN(t_paquete *paquete, t_list *lista_datos, uint32_t *pid);

// stdout
void enviar_InterfazStdout(int socket, t_list *direcciones_fisicas, uint32_t pid, char *nombre_interfaz);
t_interfaz_stdout *recibir_InterfazStdout(int socket_cliente);
t_interfaz_stdout *deserializar_InterfazStdout(t_buffer *buffer);
void enviar_interfaz_IO_stdout(t_pcb *pcb_actual, char *interfaz, t_list *Lista_direccionesFisica, int socket_cliente, nombre_instruccion IO);
void serializarInterfazStdout_de_Kernale_a_Memoria(t_paquete *paquete, t_interfaz_stdout *interfaz);
void enviar_InterfazStdoutConCodigoOPaKernel(int socket, t_list *direcciones_fisicas, uint32_t pid, char *nombre_interfaz);
void serializar_direcciones_stdout(t_paquete *paquete, t_list *direcciones_fisicas, uint32_t pid);
void enviar_direcciones_stdout(int socket, t_list *direcciones_fisicas, uint32_t pid);
void recibir_direcciones_de_stdout(int socket_cliente, t_list *lista_direcciones, uint32_t *pid);
char *recibir_dato_stdout(int socket_cliente);
char *recibir_dato(int socket, t_log *logger);
char *deserializar_dato_interfaz_STDOUT(t_paquete *paquete);

void enviar_dato_leido(int socket, char *dato, int tamanio);
void serializar_dato_leido(t_paquete *paquete, char *dato, int tamanio);
void deserializar_direcciones_de_stdout(t_paquete *paquete, t_list *lista_datos, uint32_t *pid);

// MOV IN
void enviar_dato_movIn(int socket, t_list *lista);
void serializar_datos_leidos(t_paquete *paquete, t_list *lista);

#endif // IO_H_