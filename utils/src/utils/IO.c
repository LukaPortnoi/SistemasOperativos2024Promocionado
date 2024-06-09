#include "../include/IO.h"

// Envia el nombre interfaz , las unidades de trabajo y el pcb de cpu a kernel

void enviar_interfaz_IO(t_pcb *pcb_actual, char *interfaz, int unidades_de_trabajo, int socket_cliente, nombre_instruccion IO)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(ENVIAR_INTERFAZ);
    serializar_IO_instruccion(paquete, pcb_actual, unidades_de_trabajo, interfaz, IO);
    enviar_paquete(paquete, socket_cliente);
    eliminar_paquete(paquete);
}

void serializar_IO_instruccion(t_paquete *paquete, t_pcb *pcb, int unidades_de_trabajo, char *interfaz, nombre_instruccion IO)
{
    uint32_t interfaz_length = strlen(interfaz) + 1;

    size_t tam_registros = sizeof(uint32_t) +
                           sizeof(uint8_t) * 4 +
                           sizeof(uint32_t) * 6;

    paquete->buffer->size = sizeof(uint32_t) +
                            sizeof(t_estado_proceso) +
                            sizeof(uint32_t) +
                            tam_registros +
                            sizeof(t_motivo_desalojo) +
                            sizeof(t_motivo_finalizacion) +
                            sizeof(int) +
                            sizeof(uint64_t) +
                            sizeof(uint32_t) +
                            interfaz_length +
                            sizeof(nombre_instruccion);

    paquete->buffer->stream = malloc(paquete->buffer->size);
    int desplazamiento = 0;

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->pid), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->estado), sizeof(t_estado_proceso));
    desplazamiento += sizeof(t_estado_proceso);

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->quantum), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->tiempo_q), sizeof(uint64_t));
    desplazamiento += sizeof(uint64_t);

    memcpy(paquete->buffer->stream + desplazamiento, pcb->contexto_ejecucion->registros, tam_registros);
    desplazamiento += tam_registros;

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->contexto_ejecucion->motivo_desalojo), sizeof(t_motivo_desalojo));
    desplazamiento += sizeof(t_motivo_desalojo);

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->contexto_ejecucion->motivo_finalizacion), sizeof(t_motivo_finalizacion));
    desplazamiento += sizeof(t_motivo_finalizacion);

    memcpy(paquete->buffer->stream + desplazamiento, &(unidades_de_trabajo), sizeof(int));
    desplazamiento += sizeof(int);

    memcpy(paquete->buffer->stream + desplazamiento, &(interfaz_length), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, interfaz, interfaz_length);
    desplazamiento += interfaz_length;

    memcpy(paquete->buffer->stream + desplazamiento, &IO, sizeof(nombre_instruccion));
}

t_pcb *recibir_pcb_para_interfaz(int socket_cliente, char **nombre_interfaz, int *unidades_de_trabajo, nombre_instruccion *IO)
{
    t_paquete *paquete = recibir_paquete(socket_cliente);
    t_pcb *pcb = deserializar_pcb_para_interfaz(paquete->buffer, nombre_interfaz, unidades_de_trabajo, IO);
    eliminar_paquete(paquete);
    return pcb;
}

t_pcb *deserializar_pcb_para_interfaz(t_buffer *buffer, char **nombre_interfaz, int *unidades_de_trabajo, nombre_instruccion *IO)
{
    t_pcb *pcb = malloc(sizeof(t_pcb));
    if (pcb == NULL)
    {
        return NULL;
    }

    uint32_t long_interfaz;
    void *stream = buffer->stream;
    int desplazamiento = 0;

    memcpy(&(pcb->pid), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(pcb->estado), stream + desplazamiento, sizeof(t_estado_proceso));
    desplazamiento += sizeof(t_estado_proceso);

    memcpy(&(pcb->quantum), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(pcb->tiempo_q), stream + desplazamiento, sizeof(uint64_t));
    desplazamiento += sizeof(uint64_t);

    pcb->contexto_ejecucion = malloc(sizeof(t_contexto_ejecucion));
    if (pcb->contexto_ejecucion == NULL)
    {
        free(pcb);
        return NULL;
    }

    pcb->contexto_ejecucion->registros = malloc(sizeof(t_registros));
    if (pcb->contexto_ejecucion->registros == NULL)
    {
        free(pcb->contexto_ejecucion);
        free(pcb);
        return NULL;
    }

    memcpy(pcb->contexto_ejecucion->registros, stream + desplazamiento, sizeof(t_registros));
    desplazamiento += sizeof(t_registros);

    memcpy(&(pcb->contexto_ejecucion->motivo_desalojo), stream + desplazamiento, sizeof(t_motivo_desalojo));
    desplazamiento += sizeof(t_motivo_desalojo);

    memcpy(&(pcb->contexto_ejecucion->motivo_finalizacion), stream + desplazamiento, sizeof(t_motivo_finalizacion));
    desplazamiento += sizeof(t_motivo_finalizacion);

    memcpy(unidades_de_trabajo, stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(int);

    memcpy(&(long_interfaz), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(int);

    *nombre_interfaz = malloc(long_interfaz);

    memcpy(*nombre_interfaz, stream + desplazamiento, long_interfaz);
    desplazamiento += long_interfaz;

    memcpy(IO, stream + desplazamiento, sizeof(nombre_instruccion));

    return pcb;
}

// Envia la interfaz que se va a conectar y que existe a kernel
void enviar_datos_interfaz(t_interfaz *interfaz, int socket_server, op_cod codigo_operacion)
{
    t_paquete *paquete = crear_paquete_interfaz(interfaz, codigo_operacion);
    enviar_paquete(paquete, socket_server);
    eliminar_paquete(paquete);
}

t_paquete *crear_paquete_interfaz(t_interfaz *interfaz, op_cod codigo_operacion)
{
    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->codigo_operacion = codigo_operacion;
    paquete->buffer = crear_buffer_interfaz(interfaz);
    return paquete;
}

t_buffer *crear_buffer_interfaz(t_interfaz *interfaz)
{
    interfaz->tamanio_nombre_interfaz = strlen(interfaz->nombre_interfaz) + 1;

    t_buffer *buffer = malloc(sizeof(t_buffer));

    buffer->size = interfaz->tamanio_nombre_interfaz + sizeof(t_tipo_interfaz) + sizeof(uint32_t);

    buffer->stream = malloc(buffer->size);

    int desplazamiento = 0;

    memcpy(buffer->stream + desplazamiento, &(interfaz->tamanio_nombre_interfaz), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, interfaz->nombre_interfaz, interfaz->tamanio_nombre_interfaz);
    desplazamiento += interfaz->tamanio_nombre_interfaz;

    memcpy(buffer->stream + desplazamiento, &(interfaz->tipo_interfaz), sizeof(t_tipo_interfaz));

    return buffer;
}

// Recibir la interfaaz que se va a conectar y que existe en kernel

t_interfaz *recibir_datos_interfaz(int socket_cliente)
{
    t_paquete *paquete = recibir_paquete(socket_cliente);
    t_interfaz *interfaz_received = deserializar_interfaz_recibida(paquete->buffer);
    eliminar_paquete(paquete);
    return interfaz_received;
}

t_interfaz *deserializar_interfaz_recibida(t_buffer *buffer)
{
    t_interfaz *interfaz_received = malloc(sizeof(t_interfaz));

    if (interfaz_received == NULL)
    {
        return NULL;
    }

    void *stream = buffer->stream;
    int desplazamiento = 0;

    uint32_t tamanio_nombre_interfaz;

    memcpy(&(tamanio_nombre_interfaz), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    interfaz_received->tamanio_nombre_interfaz = tamanio_nombre_interfaz;

    interfaz_received->nombre_interfaz = malloc(tamanio_nombre_interfaz);
    if (interfaz_received->nombre_interfaz == NULL)
    {
        free(interfaz_received);
        return NULL;
    }

    memcpy(interfaz_received->nombre_interfaz, stream + desplazamiento, tamanio_nombre_interfaz);
    desplazamiento += tamanio_nombre_interfaz;

    memcpy(&(interfaz_received->tipo_interfaz), stream + desplazamiento, sizeof(t_tipo_interfaz));

    return interfaz_received;
}

// Envia de kernel a Entrada/salida las interfaces para hacerles el sleep
void enviar_InterfazGenerica(int socket, int unidades_trabajo, uint32_t pid, char *nombre_interfaz)
{
    t_interfaz_gen *interfaz = malloc(sizeof(t_interfaz_gen));
    interfaz->unidades_de_trabajo = unidades_trabajo;
    interfaz->pidPcb = pid;
    interfaz->nombre_interfaz = nombre_interfaz;
    t_paquete *paquete = crear_paquete_InterfazGenericaCodOp(interfaz, PEDIDO_IO_GEN_SLEEP);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}

t_buffer *crear_buffer_InterfazGenerica(t_interfaz_gen *interfaz)
{
    uint32_t tamanio_nombre_interfaz = strlen(interfaz->nombre_interfaz) + 1;

    t_buffer *buffer = malloc(sizeof(t_buffer));
    buffer->size = sizeof(int) + sizeof(uint32_t) * 2 + tamanio_nombre_interfaz;

    buffer->stream = malloc(buffer->size);
    int desplazamiento = 0;

    memcpy(buffer->stream + desplazamiento, &(interfaz->unidades_de_trabajo), sizeof(int));
    desplazamiento += sizeof(int);

    memcpy(buffer->stream + desplazamiento, &(interfaz->pidPcb), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, &(tamanio_nombre_interfaz), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t); // Corrección aquí

    memcpy(buffer->stream + desplazamiento, interfaz->nombre_interfaz, tamanio_nombre_interfaz);

    return buffer;
}

// Recibe en Entrada/salida las interfaces para hacerles el sleep
t_interfaz_gen *recibir_InterfazGenerica(int socket_cliente)
{
    t_paquete *paquete = recibir_paquete(socket_cliente);
    t_interfaz_gen *interfaz = deserializar_InterfazGenerica(paquete->buffer);
    eliminar_paquete(paquete);
    return interfaz;
}

t_interfaz_gen *deserializar_InterfazGenerica(t_buffer *buffer)
{
    t_interfaz_gen *interfaz = malloc(sizeof(t_interfaz_gen));
    if (interfaz == NULL)
    {
        return NULL;
    }

    uint32_t long_interfaz;
    void *stream = buffer->stream;
    int desplazamiento = 0;

    memcpy(&(interfaz->unidades_de_trabajo), stream + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    memcpy(&(interfaz->pidPcb), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(long_interfaz), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    interfaz->nombre_interfaz = malloc(long_interfaz);

    memcpy(interfaz->nombre_interfaz, stream + desplazamiento, long_interfaz);

    return interfaz;
}

void enviar_InterfazGenericaConCodigoOP(int socket, int unidades_trabajo, uint32_t pid, char *nombre_interfaz)
{
    t_interfaz_gen *interfaz = malloc(sizeof(t_interfaz_gen));
    interfaz->unidades_de_trabajo = unidades_trabajo;
    interfaz->pidPcb = pid;
    interfaz->nombre_interfaz = nombre_interfaz;
    t_paquete *paquete = crear_paquete_InterfazGenericaCodOp(interfaz, FINALIZACION_INTERFAZ);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}

t_paquete *crear_paquete_InterfazGenericaCodOp(t_interfaz_gen *interfaz, op_cod codigo_operacion)
{
    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->codigo_operacion = codigo_operacion;
    paquete->buffer = crear_buffer_InterfazGenerica(interfaz);
    return paquete;
}

// INTERFAZ STDIN
void enviar_interfaz_IO_stdin(t_pcb *pcb_actual, char *interfaz, uint32_t direccionFisica, uint32_t tamanioMaximoAingresar, int socket_cliente, nombre_instruccion IO)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(ENVIAR_INTERFAZ_STDIN);
    serializar_IO_instruccion_stdin(paquete, pcb_actual, interfaz, direccionFisica, tamanioMaximoAingresar, IO);
    enviar_paquete(paquete, socket_cliente);
    eliminar_paquete(paquete);
}

void enviar_interfaz_IO_stdout(t_pcb *pcb_actual, char *interfaz, uint32_t direccionFisica, uint32_t tamanioMaximoAingresar, int socket_cliente, nombre_instruccion IO)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(ENVIAR_INTERFAZ_STDOUT);
    serializar_IO_instruccion_stdin(paquete, pcb_actual, interfaz, direccionFisica, tamanioMaximoAingresar, IO);
    enviar_paquete(paquete, socket_cliente);
    eliminar_paquete(paquete);
}

void serializar_IO_instruccion_stdin(t_paquete *paquete, t_pcb *pcb, char *interfaz, uint32_t direccionFisica, uint32_t tamanioMaximoAingresar, nombre_instruccion IO)
{
    uint32_t interfaz_length = strlen(interfaz) + 1;

    size_t tam_registros = sizeof(uint32_t) +
                           sizeof(uint8_t) * 4 +
                           sizeof(uint32_t) * 6;

    paquete->buffer->size = sizeof(uint32_t) +
                            sizeof(t_estado_proceso) +
                            sizeof(uint32_t) +
                            tam_registros +
                            sizeof(t_motivo_desalojo) +
                            sizeof(t_motivo_finalizacion) +
                            sizeof(uint32_t) +
                            sizeof(uint64_t) +
                            sizeof(uint32_t) +
                            sizeof(uint32_t) +
                            interfaz_length +
                            sizeof(nombre_instruccion);

    paquete->buffer->stream = malloc(paquete->buffer->size);
    int desplazamiento = 0;

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->pid), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->estado), sizeof(t_estado_proceso));
    desplazamiento += sizeof(t_estado_proceso);

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->quantum), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->tiempo_q), sizeof(uint64_t));
    desplazamiento += sizeof(uint64_t);

    memcpy(paquete->buffer->stream + desplazamiento, pcb->contexto_ejecucion->registros, tam_registros);
    desplazamiento += tam_registros;

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->contexto_ejecucion->motivo_desalojo), sizeof(t_motivo_desalojo));
    desplazamiento += sizeof(t_motivo_desalojo);

    memcpy(paquete->buffer->stream + desplazamiento, &(pcb->contexto_ejecucion->motivo_finalizacion), sizeof(t_motivo_finalizacion));
    desplazamiento += sizeof(t_motivo_finalizacion);

    memcpy(paquete->buffer->stream + desplazamiento, &(direccionFisica), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, &(tamanioMaximoAingresar), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, &(interfaz_length), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, interfaz, interfaz_length);
    desplazamiento += interfaz_length;

    memcpy(paquete->buffer->stream + desplazamiento, &IO, sizeof(nombre_instruccion));
}
t_pcb *recibir_pcb_para_interfaz_stdin(int socket_cliente, char **nombre_interfaz, uint32_t *direccionFisica, uint32_t *tamanioMaximoAingresar, nombre_instruccion *IO)
{
    t_paquete *paquete = recibir_paquete(socket_cliente);
    t_pcb *pcb = deserializar_pcb_para_interfaz_stdin(paquete->buffer, nombre_interfaz, direccionFisica, tamanioMaximoAingresar, IO);
    eliminar_paquete(paquete);
    return pcb;
}

t_pcb *deserializar_pcb_para_interfaz_stdin(t_buffer *buffer, char **nombre_interfaz, uint32_t *direccionFisica, uint32_t *tamanioMaximoAingresar, nombre_instruccion *IO)
{
    t_pcb *pcb = malloc(sizeof(t_pcb));
    if (pcb == NULL)
    {
        return NULL;
    }

    uint32_t long_interfaz;
    void *stream = buffer->stream;
    int desplazamiento = 0;

    memcpy(&(pcb->pid), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(pcb->estado), stream + desplazamiento, sizeof(t_estado_proceso));
    desplazamiento += sizeof(t_estado_proceso);

    memcpy(&(pcb->quantum), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(pcb->tiempo_q), stream + desplazamiento, sizeof(uint64_t));
    desplazamiento += sizeof(uint64_t);

    pcb->contexto_ejecucion = malloc(sizeof(t_contexto_ejecucion));
    if (pcb->contexto_ejecucion == NULL)
    {
        free(pcb);
        return NULL;
    }

    pcb->contexto_ejecucion->registros = malloc(sizeof(t_registros));
    if (pcb->contexto_ejecucion->registros == NULL)
    {
        free(pcb->contexto_ejecucion);
        free(pcb);
        return NULL;
    }

    memcpy(pcb->contexto_ejecucion->registros, stream + desplazamiento, sizeof(t_registros));
    desplazamiento += sizeof(t_registros);

    memcpy(&(pcb->contexto_ejecucion->motivo_desalojo), stream + desplazamiento, sizeof(t_motivo_desalojo));
    desplazamiento += sizeof(t_motivo_desalojo);

    memcpy(&(pcb->contexto_ejecucion->motivo_finalizacion), stream + desplazamiento, sizeof(t_motivo_finalizacion));
    desplazamiento += sizeof(t_motivo_finalizacion);

    memcpy(direccionFisica, stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(tamanioMaximoAingresar, stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(long_interfaz), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    *nombre_interfaz = malloc(long_interfaz);

    memcpy(*nombre_interfaz, stream + desplazamiento, long_interfaz);
    desplazamiento += long_interfaz;

    memcpy(IO, stream + desplazamiento, sizeof(nombre_instruccion));

    return pcb;
}

void enviar_InterfazStdin(int socket, uint32_t direccionFisica, uint32_t tamanioMaximo, uint32_t pid, char *nombre_interfaz)
{
    t_interfaz_stdin *interfaz = malloc(sizeof(t_interfaz_stdin));
    interfaz->direccionFisica = direccionFisica;
    interfaz->tamanioMaximo = tamanioMaximo;
    interfaz->pidPcb = pid;
    interfaz->nombre_interfaz = nombre_interfaz;
    t_paquete *paquete = crear_paquete_InterfazstdinCodOp(interfaz, PEDIDO_IO_STDIN_READ);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}

t_paquete *crear_paquete_InterfazstdinCodOp(t_interfaz_stdin *interfaz, op_cod codigo_operacion)
{
    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->codigo_operacion = codigo_operacion;
    paquete->buffer = crear_buffer_InterfazStdin(interfaz);
    return paquete;
}

t_buffer *crear_buffer_InterfazStdin(t_interfaz_stdin *interfaz)
{
    uint32_t tamanio_nombre_interfaz = strlen(interfaz->nombre_interfaz) + 1;

    t_buffer *buffer = malloc(sizeof(t_buffer));
    buffer->size = sizeof(uint32_t) * 4+ tamanio_nombre_interfaz;

    buffer->stream = malloc(buffer->size);
    int desplazamiento = 0;

    memcpy(buffer->stream + desplazamiento, &(interfaz->direccionFisica), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, &(interfaz->tamanioMaximo), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, &(interfaz->pidPcb), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, &(tamanio_nombre_interfaz), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, interfaz->nombre_interfaz, tamanio_nombre_interfaz);

    return buffer;
}


void enviar_InterfazStdout(int socket, uint32_t direccionFisica, uint32_t tamanioMaximo, uint32_t pid, char *nombre_interfaz)
{
    t_interfaz_stdout *interfaz = malloc(sizeof(t_interfaz_stdout));
    interfaz->direccionFisica = direccionFisica;
    interfaz->tamanioMaximo = tamanioMaximo;
    interfaz->pidPcb = pid;
    interfaz->nombre_interfaz = nombre_interfaz;
    t_paquete *paquete = crear_paquete_InterfazstdoutCodOp(interfaz, PEDIDO_IO_STDOUT_WRITE);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}

t_paquete *crear_paquete_InterfazstdoutCodOp(t_interfaz_stdout *interfaz, op_cod codigo_operacion)
{
    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->codigo_operacion = codigo_operacion;
    paquete->buffer = crear_buffer_InterfazStdout(interfaz);
    return paquete;
}

t_buffer *crear_buffer_InterfazStdout(t_interfaz_stdout *interfaz)
{
    uint32_t tamanio_nombre_interfaz = strlen(interfaz->nombre_interfaz) + 1;

    t_buffer *buffer = malloc(sizeof(t_buffer));
    buffer->size = sizeof(uint32_t) * 4+ tamanio_nombre_interfaz;

    buffer->stream = malloc(buffer->size);
    int desplazamiento = 0;

    memcpy(buffer->stream + desplazamiento, &(interfaz->direccionFisica), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, &(interfaz->tamanioMaximo), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, &(interfaz->pidPcb), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, &(tamanio_nombre_interfaz), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, interfaz->nombre_interfaz, tamanio_nombre_interfaz);

    return buffer;
}



t_interfaz_stdin *recibir_InterfazStdin(int socket_cliente)
{
    t_paquete *paquete = recibir_paquete(socket_cliente);
    t_interfaz_stdin *interfaz = deserializar_InterfazStdin(paquete->buffer);
    eliminar_paquete(paquete);
    return interfaz;
}

t_interfaz_stdin *deserializar_InterfazStdin(t_buffer *buffer)
{
    t_interfaz_stdin *interfaz = malloc(sizeof(t_interfaz_stdin));
    if (interfaz == NULL)
    {
        return NULL;
    }

    uint32_t long_interfaz;
    void *stream = buffer->stream;
    int desplazamiento = 0;

    memcpy(&(interfaz->direccionFisica), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(interfaz->tamanioMaximo), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(interfaz->pidPcb), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(long_interfaz), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    interfaz->nombre_interfaz = malloc(long_interfaz);

    memcpy(interfaz->nombre_interfaz, stream + desplazamiento, long_interfaz);

    return interfaz;
}


t_interfaz_stdout *recibir_InterfazStdout(int socket_cliente)
{
    t_paquete *paquete = recibir_paquete(socket_cliente);
    t_interfaz_stdout *interfaz = deserializar_InterfazStdout(paquete->buffer);
    eliminar_paquete(paquete);
    return interfaz;
}

t_interfaz_stdout *deserializar_InterfazStdout(t_buffer *buffer)
{
    t_interfaz_stdout *interfaz = malloc(sizeof(t_interfaz_stdout));
    if (interfaz == NULL)
    {
        return NULL;
    }

    uint32_t long_interfaz;
    void *stream = buffer->stream;
    int desplazamiento = 0;

    memcpy(&(interfaz->direccionFisica), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(interfaz->tamanioMaximo), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(interfaz->pidPcb), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(long_interfaz), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    interfaz->nombre_interfaz = malloc(long_interfaz);

    memcpy(interfaz->nombre_interfaz, stream + desplazamiento, long_interfaz);

    return interfaz;
}

/* void enviar_dato_stdin(int socket, uint32_t direccionFisica, char *datoRecibido)
{
    t_paquete *paquete = crear_paquete_dato_stdin(direccionFisica, datoRecibido, RECIBIR_DATO_STDIN);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}

t_paquete *crear_paquete_dato_stdin(uint32_t direccionFisica, char *datoRecibido, op_cod codigo_operacion)
{
    t_paquete *paquete = malloc(sizeof(t_paquete));
    paquete->codigo_operacion = codigo_operacion;
    paquete->buffer = crear_buffer_dato_stdin(direccionFisica, datoRecibido);
    return paquete;
}

t_buffer *crear_buffer_dato_stdin(uint32_t direccionFisica, char *datoRecibido)
{
    uint32_t tamanio_dato = strlen(datoRecibido) + 1;

    t_buffer *buffer = malloc(sizeof(t_buffer));
    buffer->size = sizeof(uint32_t) + tamanio_nombre_interfaz;

    buffer->stream = malloc(buffer->size);
    int desplazamiento = 0;

    memcpy(buffer->stream + desplazamiento, &(direccionFisica), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, &(tamanio_dato), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(buffer->stream + desplazamiento, datoRecibido, tamanio_dato);

    return buffer;
} */

void enviar_InterfazStdinConCodigoOP(int socket, uint32_t direccionFisica, uint32_t tamanioMaximo, uint32_t pid, char *nombre_interfaz)
{
    t_interfaz_stdin *interfaz = malloc(sizeof(t_interfaz_stdin));
    interfaz->direccionFisica = direccionFisica;
    interfaz->tamanioMaximo = tamanioMaximo;
    interfaz->pidPcb = pid;
    interfaz->nombre_interfaz = nombre_interfaz;
    t_paquete *paquete = crear_paquete_InterfazstdinCodOp(interfaz, FINALIZACION_INTERFAZ_STDIN);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}