#include "../include/IO.h"

void destroyInterfazDialfs(t_interfaz_dialfs *interfaz)
{
    free(interfaz->nombre_archivo);
    free(interfaz->nombre_interfaz);
    list_destroy_and_destroy_elements(interfaz->direcciones, free);
    free(interfaz);
}

void deserializar_interfaz_dialfs_write_read(t_buffer *buffer, t_interfaz_dialfs *interfaz)
{
    uint32_t long_nombre_archivo;
    uint32_t long_nombre_interfaz;
    uint32_t long_puntero_archivo;

    void *stream = buffer->stream;
    int desplazamiento = 0;

    memcpy(&(interfaz->pidPcb), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(long_nombre_archivo), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    interfaz->nombre_archivo = malloc(long_nombre_archivo);
    memcpy(interfaz->nombre_archivo, stream + desplazamiento, long_nombre_archivo);
    desplazamiento += long_nombre_archivo;

    // ahora el long_nombre_interfaz
    memcpy(&(long_nombre_interfaz), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    interfaz->nombre_interfaz = malloc(long_nombre_interfaz);
    memcpy(interfaz->nombre_interfaz, stream + desplazamiento, long_nombre_interfaz);
    desplazamiento += long_nombre_interfaz;

    memcpy(&(interfaz->tamanio), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    uint32_t tamanio_direcciones;
    memcpy(&tamanio_direcciones, stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    for (int i = 0; i < tamanio_direcciones; i++)
    {
        uint32_t *direccion = malloc(sizeof(uint32_t));
        memcpy(direccion, stream + desplazamiento, sizeof(uint32_t));
        list_add(interfaz->direcciones, direccion);
        desplazamiento += sizeof(uint32_t);
    }

    memcpy(&(long_puntero_archivo), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    interfaz->puntero_archivo = malloc(long_puntero_archivo);
    memcpy(interfaz->puntero_archivo, stream + desplazamiento, long_puntero_archivo);
}

void enviar_interfaz_dialFS_write_read(int socket, char *nombre_archivo, uint32_t pid, char *nombre_interfaz, uint32_t tamanio_fs_recibir, t_list *direcciones_fisicas, char *puntero_fs, nombre_instruccion instruccion)
{
    t_paquete *paquete;
    if (instruccion == IO_FS_WRITE)
    {
        paquete = crear_paquete_con_codigo_de_operacion(PEDIDO_IO_FS_WRITE);
    }
    else
    {
        paquete = crear_paquete_con_codigo_de_operacion(PEDIDO_IO_FS_READ);
    }
    serializar_interfaz_dialFS_write_read(paquete, nombre_archivo, pid, nombre_interfaz, tamanio_fs_recibir, direcciones_fisicas, puntero_fs);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}

void serializar_interfaz_dialFS_write_read(t_paquete *paquete, char *nombre_archivo, uint32_t pid, char *nombre_interfaz, uint32_t tamanio_fs_recibir, t_list *direcciones_fisicas, char *puntero_fs)
{
    uint32_t long_nombre_archivo = strlen(nombre_archivo) + 1;
    uint32_t long_nombre_interfaz = strlen(nombre_interfaz) + 1;
    uint32_t long_puntero_fs = strlen(puntero_fs) + 1;

    size_t tam_direcciones = 0;
    for (int i = 0; i < list_size(direcciones_fisicas); i++)
    {
        tam_direcciones += sizeof(uint32_t);
    }

    paquete->buffer->size = sizeof(uint32_t) +
                            sizeof(uint32_t) +
                            long_nombre_archivo +
                            sizeof(uint32_t) +
                            long_nombre_interfaz +
                            sizeof(uint32_t) +
                            sizeof(uint32_t) +
                            tam_direcciones +
                            sizeof(uint32_t) +
                            long_puntero_fs +
                            sizeof(uint32_t) +
                            sizeof(nombre_instruccion);

    paquete->buffer->stream = malloc(paquete->buffer->size);
    int desplazamiento = 0;

    memcpy(paquete->buffer->stream + desplazamiento, &pid, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, &long_nombre_archivo, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, nombre_archivo, long_nombre_archivo);
    desplazamiento += long_nombre_archivo;

    memcpy(paquete->buffer->stream + desplazamiento, &long_nombre_interfaz, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, nombre_interfaz, long_nombre_interfaz);
    desplazamiento += long_nombre_interfaz;

    memcpy(paquete->buffer->stream + desplazamiento, &tamanio_fs_recibir, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, &tam_direcciones, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    for (int i = 0; i < list_size(direcciones_fisicas); i++)
    {
        uint32_t *direccion = list_get(direcciones_fisicas, i);
        memcpy(paquete->buffer->stream + desplazamiento, direccion, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
    }

    memcpy(paquete->buffer->stream + desplazamiento, &long_puntero_fs, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, puntero_fs, long_puntero_fs);
}

void recibir_pcb_fs_write_read(t_pcb *pcb, int socket_cliente, char **nombre_interfaz, char **nombre_archivo, t_list *direcciones, uint32_t *tamanio_fs_recibir, char **puntero_fs, nombre_instruccion *instruccion)
{
    t_paquete *paquete = recibir_paquete(socket_cliente);
    deserializar_pcb_fs_write_read(pcb, paquete->buffer, nombre_interfaz, nombre_archivo, direcciones, tamanio_fs_recibir, puntero_fs, instruccion);
    eliminar_paquete(paquete);
}

void deserializar_pcb_fs_write_read(t_pcb *pcb, t_buffer *buffer, char **nombre_interfaz, char **nombre_archivo, t_list *direcciones, uint32_t *tamanio_fs_recibir, char **puntero_fs, nombre_instruccion *instruccion)
{
    uint32_t tamanio_interfaz;
    uint32_t tamanio_nombre_archivo;
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

    memcpy(pcb->contexto_ejecucion->registros, stream + desplazamiento, sizeof(t_registros));
    desplazamiento += sizeof(t_registros);

    memcpy(&(pcb->contexto_ejecucion->motivo_desalojo), stream + desplazamiento, sizeof(t_motivo_desalojo));
    desplazamiento += sizeof(t_motivo_desalojo);

    memcpy(&(pcb->contexto_ejecucion->motivo_finalizacion), stream + desplazamiento, sizeof(t_motivo_finalizacion));
    desplazamiento += sizeof(t_motivo_finalizacion);

    memcpy(&tamanio_interfaz, stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    *nombre_interfaz = malloc(tamanio_interfaz);
    memcpy(*nombre_interfaz, stream + desplazamiento, tamanio_interfaz);
    desplazamiento += tamanio_interfaz;

    memcpy(&tamanio_nombre_archivo, stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    *nombre_archivo = malloc(tamanio_nombre_archivo);
    memcpy(*nombre_archivo, stream + desplazamiento, tamanio_nombre_archivo);
    desplazamiento += tamanio_nombre_archivo;

    uint32_t tamanio_direcciones;
    memcpy(&tamanio_direcciones, stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    for (int i = 0; i < tamanio_direcciones; i++) {
        t_direcciones_fisicas* direccion = malloc(sizeof(t_direcciones_fisicas));
        memcpy(&direccion->direccion_fisica, stream + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
        memcpy(&direccion->tamanio, stream + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
        list_add(direcciones, direccion);
    }

    memcpy(tamanio_fs_recibir, stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    uint32_t tamanio_puntero_fs;
    memcpy(&tamanio_puntero_fs, stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    *puntero_fs = malloc(tamanio_puntero_fs);
    memcpy(*puntero_fs, stream + desplazamiento, tamanio_puntero_fs);
    desplazamiento += tamanio_puntero_fs;

    memcpy(instruccion, stream + desplazamiento, sizeof(nombre_instruccion));
}

void enviar_interfaz_fs_write_read(t_pcb *pcb, char *interfaz, char *nombre_archivo, t_list *direcciones, uint32_t tamanioEscribir, char *puntero_archivo, int socket, nombre_instruccion instruccion)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(FS_WRITE_READ);
    serializar_interfaz_fs_write_read(paquete, pcb, interfaz, nombre_archivo, direcciones, tamanioEscribir, puntero_archivo, instruccion);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}

void serializar_interfaz_fs_write_read(t_paquete *paquete, t_pcb *pcb, char *interfaz, char *nombre_archivo, t_list *direcciones, uint32_t tamanioEscribir, char *puntero_archivo, nombre_instruccion instruccion)
{
    uint32_t tamanio_interfaz = strlen(interfaz) + 1;
    uint32_t tamanio_nombre_archivo = strlen(nombre_archivo) + 1;
    uint32_t tamanio_puntero_archivo = strlen(puntero_archivo) + 1;

    size_t tam_registros = sizeof(uint32_t) +
                           sizeof(uint8_t) * 4 +
                           sizeof(uint32_t) * 6;

    size_t tam_direcciones = 0;
    for (int i = 0; i < list_size(direcciones); i++)
    {
        tam_direcciones += sizeof(uint32_t);
        tam_direcciones += sizeof(uint32_t);
    }

    paquete->buffer->size = sizeof(uint32_t) +
                            sizeof(t_estado_proceso) +
                            sizeof(uint32_t) +
                            sizeof(uint64_t) +
                            tam_registros +
                            sizeof(t_motivo_desalojo) +
                            sizeof(t_motivo_finalizacion) +
                            sizeof(uint32_t) +
                            tamanio_interfaz +
                            sizeof(uint32_t) +
                            tamanio_nombre_archivo +
                            sizeof(uint32_t) +
                            tam_direcciones +
                            sizeof(uint32_t) +
                            sizeof(uint32_t) +
                            tamanio_puntero_archivo +
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

    memcpy(paquete->buffer->stream + desplazamiento, &tamanio_interfaz, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, interfaz, tamanio_interfaz);
    desplazamiento += tamanio_interfaz;

    memcpy(paquete->buffer->stream + desplazamiento, &tamanio_nombre_archivo, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, nombre_archivo, tamanio_nombre_archivo);
    desplazamiento += tamanio_nombre_archivo;

    memcpy(paquete->buffer->stream + desplazamiento, &tam_direcciones, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    for (int i = 0; i < list_size(direcciones); i++)
    {
        t_direcciones_fisicas *direccion = list_get(direcciones, i);
        memcpy(paquete->buffer->stream + desplazamiento, &direccion->direccion_fisica, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
        memcpy(paquete->buffer->stream + desplazamiento, &direccion->tamanio, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
    }

    memcpy(paquete->buffer->stream + desplazamiento, &tamanioEscribir, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, &tamanio_puntero_archivo, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, puntero_archivo, tamanio_puntero_archivo);
    desplazamiento += tamanio_puntero_archivo;

    memcpy(paquete->buffer->stream + desplazamiento, &instruccion, sizeof(nombre_instruccion));
}

void enviar_fs_truncate(t_pcb *pcb, char *interfaz, char *nombre_archivo, uint32_t tamanio, int socket)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(FS_TRUNCATE);
    serializar_fs_truncate(paquete, pcb, interfaz, nombre_archivo, tamanio);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}

void serializar_fs_truncate(t_paquete *paquete, t_pcb *pcb, char *interfaz, char *nombre_archivo, uint32_t tamanio)
{
    uint32_t tamanio_interfaz = strlen(interfaz) + 1;
    uint32_t tamanio_nombre_archivo = strlen(nombre_archivo) + 1;

    size_t tam_registros = sizeof(uint32_t) +
                           sizeof(uint8_t) * 4 +
                           sizeof(uint32_t) * 6;

    paquete->buffer->size = sizeof(uint32_t) +
                            sizeof(t_estado_proceso) +
                            sizeof(uint32_t) +
                            sizeof(uint64_t) +
                            tam_registros +
                            sizeof(t_motivo_desalojo) +
                            sizeof(t_motivo_finalizacion) +
                            sizeof(uint32_t) +
                            tamanio_interfaz +
                            sizeof(uint32_t) +
                            tamanio_nombre_archivo +
                            sizeof(uint32_t);

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

    memcpy(paquete->buffer->stream + desplazamiento, &tamanio_interfaz, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, interfaz, tamanio_interfaz);
    desplazamiento += tamanio_interfaz;

    memcpy(paquete->buffer->stream + desplazamiento, &tamanio_nombre_archivo, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, nombre_archivo, tamanio_nombre_archivo);
    desplazamiento += tamanio_nombre_archivo;

    memcpy(paquete->buffer->stream + desplazamiento, &tamanio, sizeof(uint32_t));
}

void recibir_pcb_fs_truncate(t_pcb *pcb, int socket_cliente, char **nombre_interfaz, char **nombre_archivo, uint32_t *tamanio_fs_recibir, nombre_instruccion *instruccion)
{
    t_paquete *paquete = recibir_paquete(socket_cliente);
    deserializar_pcb_fs_truncate(pcb, paquete->buffer, nombre_interfaz, nombre_archivo, tamanio_fs_recibir, instruccion);
    eliminar_paquete(paquete);
}

void deserializar_pcb_fs_truncate(t_pcb *pcb, t_buffer *buffer, char **nombre_interfaz, char **nombre_archivo, uint32_t *tamanio_fs_recibir, nombre_instruccion *instruccion)
{
    uint32_t tamanio_interfaz;
    uint32_t tamanio_nombre_archivo;
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

    memcpy(pcb->contexto_ejecucion->registros, stream + desplazamiento, sizeof(t_registros));
    desplazamiento += sizeof(t_registros);

    memcpy(&(pcb->contexto_ejecucion->motivo_desalojo), stream + desplazamiento, sizeof(t_motivo_desalojo));
    desplazamiento += sizeof(t_motivo_desalojo);

    memcpy(&(pcb->contexto_ejecucion->motivo_finalizacion), stream + desplazamiento, sizeof(t_motivo_finalizacion));
    desplazamiento += sizeof(t_motivo_finalizacion);

    memcpy(&tamanio_interfaz, stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    *nombre_interfaz = malloc(tamanio_interfaz);
    memcpy(*nombre_interfaz, stream + desplazamiento, tamanio_interfaz);
    desplazamiento += tamanio_interfaz;

    memcpy(&tamanio_nombre_archivo, stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    *nombre_archivo = malloc(tamanio_nombre_archivo);
    memcpy(*nombre_archivo, stream + desplazamiento, tamanio_nombre_archivo);
    desplazamiento += tamanio_nombre_archivo;

    memcpy(tamanio_fs_recibir, stream + desplazamiento, sizeof(uint32_t));

    *instruccion = IO_FS_TRUNCATE;
}

void enviar_interfaz_dialFS_truncate(int socket, char *nombre_archivo, uint32_t pid, char *nombre_interfaz, uint32_t tamanio_fs_recibir)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(PEDIDO_IO_FS_TRUNCATE);
    serializar_interfaz_dialFS_truncate(paquete, nombre_archivo, pid, nombre_interfaz, tamanio_fs_recibir);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}

void serializar_interfaz_dialFS_truncate(t_paquete *paquete, char *nombre_archivo, uint32_t pid, char *nombre_interfaz, uint32_t tamanio_fs_recibir)
{
    uint32_t long_nombre_archivo = strlen(nombre_archivo) + 1;
    uint32_t long_nombre_interfaz = strlen(nombre_interfaz) + 1;

    paquete->buffer->size = sizeof(uint32_t) + sizeof(uint32_t) + long_nombre_archivo + sizeof(uint32_t) + long_nombre_interfaz + sizeof(uint32_t);
    paquete->buffer->stream = malloc(paquete->buffer->size);

    int offset = 0;

    memcpy(paquete->buffer->stream + offset, &pid, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + offset, &long_nombre_archivo, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + offset, nombre_archivo, long_nombre_archivo);
    offset += long_nombre_archivo;

    memcpy(paquete->buffer->stream + offset, &long_nombre_interfaz, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + offset, nombre_interfaz, long_nombre_interfaz);
    offset += long_nombre_interfaz;

    memcpy(paquete->buffer->stream + offset, &tamanio_fs_recibir, sizeof(uint32_t));
}

void enviar_fs_create_delete(t_pcb *pcb, char *interfaz, char *path, int socket, nombre_instruccion instruccion)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(FS_CREATE_DELETE);
    serializar_fs_create_delete(paquete, pcb, interfaz, path, instruccion);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}

void serializar_fs_create_delete(t_paquete *paquete, t_pcb *pcb, char *interfaz, char *path, nombre_instruccion instruccion)
{

    uint32_t tamanio_interfaz = strlen(interfaz) + 1;
    uint32_t tamanio_path = strlen(path) + 1;

    size_t tam_registros = sizeof(uint32_t) +
                           sizeof(uint8_t) * 4 +
                           sizeof(uint32_t) * 6;

    paquete->buffer->size = sizeof(uint32_t) +
                            sizeof(t_estado_proceso) +
                            sizeof(uint32_t) +
                            sizeof(uint64_t) +
                            tam_registros +
                            sizeof(t_motivo_desalojo) +
                            sizeof(t_motivo_finalizacion) +
                            sizeof(uint32_t) +
                            tamanio_interfaz +
                            sizeof(uint32_t) +
                            tamanio_path +
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

    memcpy(paquete->buffer->stream + desplazamiento, &tamanio_interfaz, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, interfaz, tamanio_interfaz);
    desplazamiento += tamanio_interfaz;

    memcpy(paquete->buffer->stream + desplazamiento, &tamanio_path, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, path, tamanio_path);
    desplazamiento += tamanio_path;

    memcpy(paquete->buffer->stream + desplazamiento, &instruccion, sizeof(nombre_instruccion));
}

void recibir_pcb_fs_create_delete(t_pcb *pcb, int socket, char **nombre_interfaz, char **nombre_archivo, nombre_instruccion *instruccion)
{
    t_paquete *paquete = recibir_paquete(socket);
    deserializar_pcb_fs_create_delete(pcb, paquete->buffer, nombre_interfaz, nombre_archivo, instruccion);
    eliminar_paquete(paquete);
}

void deserializar_pcb_fs_create_delete(t_pcb *pcb, t_buffer *buffer, char **nombre_interfaz, char **nombre_archivo, nombre_instruccion *instruccion)
{
    uint32_t long_nombre_interfaz;
    uint32_t long_nombre_archivo;
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

    memcpy(pcb->contexto_ejecucion->registros, stream + desplazamiento, sizeof(t_registros));
    desplazamiento += sizeof(t_registros);

    memcpy(&(pcb->contexto_ejecucion->motivo_desalojo), stream + desplazamiento, sizeof(t_motivo_desalojo));
    desplazamiento += sizeof(t_motivo_desalojo);

    memcpy(&(pcb->contexto_ejecucion->motivo_finalizacion), stream + desplazamiento, sizeof(t_motivo_finalizacion));
    desplazamiento += sizeof(t_motivo_finalizacion);

    memcpy(&long_nombre_interfaz, stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    *nombre_interfaz = malloc(long_nombre_interfaz);

    memcpy(*nombre_interfaz, stream + desplazamiento, long_nombre_interfaz);
    desplazamiento += long_nombre_interfaz;

    memcpy(&long_nombre_archivo, stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    *nombre_archivo = malloc(long_nombre_archivo);

    memcpy(*nombre_archivo, stream + desplazamiento, long_nombre_archivo);
    desplazamiento += long_nombre_archivo;

    memcpy(instruccion, stream + desplazamiento, sizeof(nombre_instruccion));
}

void enviar_interfaz_dialFS_create_delete(int socket, char *nombre_archivo, uint32_t pid, char *nombre_interfaz, nombre_instruccion instruccion)
{
    t_paquete *paquete;

    if (instruccion == IO_FS_CREATE)
    {
        paquete = crear_paquete_con_codigo_de_operacion(PEDIDO_IO_FS_CREATE);
    }
    else
    {
        paquete = crear_paquete_con_codigo_de_operacion(PEDIDO_IO_FS_DELETE);
    }
    serializar_interfaz_dialFS_create_delete(paquete, nombre_archivo, pid, nombre_interfaz);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}

void serializar_interfaz_dialFS_create_delete(t_paquete *paquete, char *nombre_archivo, uint32_t pid, char *nombre_interfaz)
{
    uint32_t long_nombre_archivo = strlen(nombre_archivo) + 1;
    uint32_t long_nombre_interfaz = strlen(nombre_interfaz) + 1;

    paquete->buffer->size = sizeof(uint32_t) + sizeof(uint32_t) + long_nombre_archivo + sizeof(uint32_t) + long_nombre_interfaz;
    paquete->buffer->stream = malloc(paquete->buffer->size);

    int offset = 0;

    memcpy(paquete->buffer->stream + offset, &pid, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + offset, &long_nombre_archivo, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + offset, nombre_archivo, long_nombre_archivo);
    offset += long_nombre_archivo;

    memcpy(paquete->buffer->stream + offset, &long_nombre_interfaz, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + offset, nombre_interfaz, long_nombre_interfaz);
}

t_interfaz_dialfs *crearInterfazDialfs()
{
    t_interfaz_dialfs *interfaz = malloc(sizeof(t_interfaz_dialfs));
    if (interfaz == NULL)
    {
        return NULL;
    }

    interfaz->pidPcb = 0;
    interfaz->nombre_interfaz = NULL;
    interfaz->nombre_archivo = NULL;
    interfaz->tamanio = 0;
    interfaz->unidades_de_trabajo = 0;
    interfaz->direcciones = list_create();
    interfaz->puntero_archivo = NULL;

    return interfaz;
}

void recibir_InterfazDialfs(int socket, t_interfaz_dialfs *interfaz, op_cod codigo_operacion)
{
    t_paquete *paquete = recibir_paquete(socket);
    deserializar_InterfazDialfs(paquete->buffer, interfaz, codigo_operacion);
    eliminar_paquete(paquete);
}

void deserializar_InterfazDialfs(t_buffer *buffer, t_interfaz_dialfs *interfaz, op_cod codigo_operacion)
{

    switch (codigo_operacion)
    {
    case PEDIDO_IO_FS_CREATE:
    case PEDIDO_IO_FS_DELETE:
        deserializar_interfaz_dialfs_create_delete(buffer, interfaz);
        break;
    case PEDIDO_IO_FS_TRUNCATE:
        deserializar_interfaz_dialfs_truncate(buffer, interfaz);
        break;
    case PEDIDO_IO_FS_WRITE:
    case PEDIDO_IO_FS_READ:
        deserializar_interfaz_dialfs_write_read(buffer, interfaz);
        break;
    default:
        printf("Codigo de operacion no valido\n");
        break;
    }
}

void deserializar_interfaz_dialfs_truncate(t_buffer *buffer, t_interfaz_dialfs *interfaz)
{
    uint32_t long_nombre_archivo;
    uint32_t long_nombre_interfaz;
    void *stream = buffer->stream;
    int desplazamiento = 0;

    memcpy(&(interfaz->pidPcb), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(long_nombre_archivo), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    interfaz->nombre_archivo = malloc(long_nombre_archivo);
    if (interfaz->nombre_archivo == NULL)
    {
        free(interfaz);
        return;
    }

    memcpy(interfaz->nombre_archivo, stream + desplazamiento, long_nombre_archivo);
    desplazamiento += long_nombre_archivo;

    memcpy(&(long_nombre_interfaz), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    interfaz->nombre_interfaz = malloc(long_nombre_interfaz);
    if (interfaz->nombre_interfaz == NULL)
    {
        free(interfaz->nombre_archivo);
        free(interfaz);
        return;
    }

    memcpy(interfaz->nombre_interfaz, stream + desplazamiento, long_nombre_interfaz);
    desplazamiento += long_nombre_interfaz;

    memcpy(&(interfaz->tamanio), stream + desplazamiento, sizeof(uint32_t));
}

void deserializar_interfaz_dialfs_create_delete(t_buffer *buffer, t_interfaz_dialfs *interfaz)
{
    uint32_t long_nombre_archivo;
    uint32_t long_nombre_interfaz;
    void *stream = buffer->stream;
    int desplazamiento = 0;

    memcpy(&(interfaz->pidPcb), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(long_nombre_archivo), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    interfaz->nombre_archivo = malloc(long_nombre_archivo);
    if (interfaz->nombre_archivo == NULL)
    {
        free(interfaz);
        return;
    }

    memcpy(interfaz->nombre_archivo, stream + desplazamiento, long_nombre_archivo);
    desplazamiento += long_nombre_archivo;

    memcpy(&(long_nombre_interfaz), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    interfaz->nombre_interfaz = malloc(long_nombre_interfaz);
    if (interfaz->nombre_interfaz == NULL)
    {
        free(interfaz->nombre_archivo);
        free(interfaz);
        return;
    }

    memcpy(interfaz->nombre_interfaz, stream + desplazamiento, long_nombre_interfaz);
}

t_interfaz_dialfs *recibir_InterfazDialfs_terminada(int socket)
{
    t_interfaz_dialfs *interfaz = crearInterfazDialfs();
    t_paquete *paquete = recibir_paquete(socket);
    deserializar_InterfazDialfs_terminada(paquete->buffer, interfaz);
    eliminar_paquete(paquete);
    return interfaz;
}

void deserializar_InterfazDialfs_terminada(t_buffer *buffer, t_interfaz_dialfs *interfaz)
{
    uint32_t long_nombre_interfaz;
    void *stream = buffer->stream;
    int desplazamiento = 0;

    memcpy(&(interfaz->pidPcb), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(long_nombre_interfaz), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    interfaz->nombre_interfaz = malloc(long_nombre_interfaz);
    if (interfaz->nombre_interfaz == NULL)
    {
        free(interfaz->nombre_archivo);
        free(interfaz);
        return;
    }

    memcpy(interfaz->nombre_interfaz, stream + desplazamiento, long_nombre_interfaz);
}

// enviar_dialfs_terminado(socket_cliente, interfazRecibida->pidPcb, interfazRecibida->nombre_interfaz);

void enviar_dialfs_terminado(int socket, uint32_t pid, char *nombre_interfaz)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(FINALIZACION_INTERFAZ_DIALFS);
    serializar_dialfs_terminado(paquete, pid, nombre_interfaz);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}

void serializar_dialfs_terminado(t_paquete *paquete, uint32_t pid, char *nombre_interfaz)
{
    uint32_t long_nombre_interfaz = strlen(nombre_interfaz) + 1;

    paquete->buffer->size = sizeof(uint32_t) + sizeof(uint32_t) + long_nombre_interfaz;
    paquete->buffer->stream = malloc(paquete->buffer->size);

    int offset = 0;

    memcpy(paquete->buffer->stream + offset, &pid, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + offset, &long_nombre_interfaz, sizeof(uint32_t));
    offset += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + offset, nombre_interfaz, long_nombre_interfaz);
}

t_interfaz *crear_interfaz(char *nombre_interfaz, t_tipo_interfaz tipo_interfaz)
{
    t_interfaz *interfaz = malloc(sizeof(t_interfaz));
    interfaz->nombre_interfaz = nombre_interfaz;
    interfaz->tamanio_nombre_interfaz = strlen(nombre_interfaz) + 1;
    interfaz->tipo_interfaz = tipo_interfaz;
    return interfaz;
}

void destruir_interfaz(t_interfaz *interfaz)
{
    free(interfaz->nombre_interfaz);
    free(interfaz);
}

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

void recibir_pcb_para_interfaz(t_pcb *pcb, int socket_cliente, char **nombre_interfaz, int *unidades_de_trabajo, nombre_instruccion *IO)
{
    t_paquete *paquete = recibir_paquete(socket_cliente);
    deserializar_pcb_para_interfaz(pcb, paquete->buffer, nombre_interfaz, unidades_de_trabajo, IO);
    eliminar_paquete(paquete);
}

void deserializar_pcb_para_interfaz(t_pcb *pcb, t_buffer *buffer, char **nombre_interfaz, int *unidades_de_trabajo, nombre_instruccion *IO)
{
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
    free(interfaz);
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

void enviar_interfaz_IO_stdin(t_pcb *pcb_actual, char *interfaz, t_list *Lista_direccionesFisica, int socket_cliente, nombre_instruccion IO)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(ENVIAR_INTERFAZ_STDIN);
    serializar_IO_instruccion_stdin(paquete, pcb_actual, interfaz, Lista_direccionesFisica, IO);
    enviar_paquete(paquete, socket_cliente);
    eliminar_paquete(paquete);
}

void serializar_IO_instruccion_stdin(t_paquete *paquete, t_pcb *pcb, char *interfaz, t_list *Lista_direccionesFisica, nombre_instruccion IO)
{
    uint32_t interfaz_length = strlen(interfaz) + 1;
    uint32_t tamanioLista = list_size(Lista_direccionesFisica);

    size_t tam_registros = sizeof(uint32_t) +
                           sizeof(uint8_t) * 4 +
                           sizeof(uint32_t) * 6;

    paquete->buffer->size = sizeof(uint32_t) +                                    // pcb->pid
                            sizeof(t_estado_proceso) +                            // pcb->estado
                            sizeof(uint32_t) +                                    // pcb->quantum
                            sizeof(uint64_t) +                                    // pcb->tiempo_q
                            tam_registros +                                       // pcb->contexto_ejecucion->registros
                            sizeof(t_motivo_desalojo) +                           // pcb->contexto_ejecucion->motivo_desalojo
                            sizeof(t_motivo_finalizacion) +                       // pcb->contexto_ejecucion->motivo_finalizacion
                            sizeof(uint32_t) +                                    // interfaz_length
                            interfaz_length +                                     // interfaz
                            sizeof(nombre_instruccion) +                          // IO
                            sizeof(uint32_t) +                                    // tamanioLista
                            tamanioLista * (sizeof(uint32_t) + sizeof(uint32_t)); // Lista_direccionesFisica

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

    memcpy(paquete->buffer->stream + desplazamiento, &(interfaz_length), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, interfaz, interfaz_length);
    desplazamiento += interfaz_length;

    memcpy(paquete->buffer->stream + desplazamiento, &IO, sizeof(nombre_instruccion));
    desplazamiento += sizeof(nombre_instruccion);

    memcpy(paquete->buffer->stream + desplazamiento, &(tamanioLista), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    for (int i = 0; i < list_size(Lista_direccionesFisica); i++)
    {
        t_direcciones_fisicas *dato = list_get(Lista_direccionesFisica, i);
        memcpy(paquete->buffer->stream + desplazamiento, &(dato->direccion_fisica), sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
        memcpy(paquete->buffer->stream + desplazamiento, &(dato->tamanio), sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
    }
}

void recibir_pcb_para_interfaz_in_out(t_pcb *pcb, int socket_cliente, char **nombre_interfaz, t_list *direcciones_fisicas, nombre_instruccion *IO)
{
    t_paquete *paquete = recibir_paquete(socket_cliente);
    deserializar_pcb_para_interfaz_stdin(pcb, paquete->buffer, nombre_interfaz, direcciones_fisicas, IO);
    eliminar_paquete(paquete);
}

void deserializar_pcb_para_interfaz_stdin(t_pcb *pcb, t_buffer *buffer, char **nombre_interfaz, t_list *direcciones_fisicas, nombre_instruccion *IO)
{
    uint32_t long_interfaz;
    uint32_t tamanioListDirecciones;
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

    size_t tam_registros = sizeof(uint32_t) +
                           sizeof(uint8_t) * 4 +
                           sizeof(uint32_t) * 6;

    memcpy(pcb->contexto_ejecucion->registros, stream + desplazamiento, tam_registros);
    desplazamiento += tam_registros;

    memcpy(&(pcb->contexto_ejecucion->motivo_desalojo), stream + desplazamiento, sizeof(t_motivo_desalojo));
    desplazamiento += sizeof(t_motivo_desalojo);

    memcpy(&(pcb->contexto_ejecucion->motivo_finalizacion), stream + desplazamiento, sizeof(t_motivo_finalizacion));
    desplazamiento += sizeof(t_motivo_finalizacion);

    memcpy(&(long_interfaz), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    *nombre_interfaz = malloc(long_interfaz);
    if (*nombre_interfaz == NULL)
    {
        free(pcb->contexto_ejecucion->registros);
        free(pcb->contexto_ejecucion);
        free(pcb);
    }
    memcpy(*nombre_interfaz, stream + desplazamiento, long_interfaz);
    desplazamiento += long_interfaz;

    memcpy(IO, stream + desplazamiento, sizeof(nombre_instruccion));
    desplazamiento += sizeof(nombre_instruccion);

    memcpy(&(tamanioListDirecciones), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    for (size_t i = 0; i < tamanioListDirecciones; i++)
    {
        t_direcciones_fisicas *dato = malloc(sizeof(t_direcciones_fisicas));
        if (dato == NULL)
        {
            // Manejo de errores en caso de falla de memoria
            list_clean_and_destroy_elements(direcciones_fisicas, free);
            free(*nombre_interfaz);
            free(pcb->contexto_ejecucion->registros);
            free(pcb->contexto_ejecucion);
            free(pcb);
        }
        memcpy(&(dato->direccion_fisica), stream + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
        memcpy(&(dato->tamanio), stream + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        list_add(direcciones_fisicas, dato);
    }
}

void enviar_InterfazStdin(int socket, t_list *direcciones_fisicas, uint32_t pid, char *nombre_interfaz)
{
    t_interfaz_stdin *interfaz = malloc(sizeof(t_interfaz_stdin));
    if (interfaz == NULL)
    {
        // Manejo de error si la asignación de memoria falla
        return;
    }
    interfaz->direccionesFisicas = direcciones_fisicas;
    interfaz->pidPcb = pid;
    interfaz->nombre_interfaz = strdup(nombre_interfaz); // Copiar el nombre de interfaz

    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(PEDIDO_IO_STDIN_READ);
    if (paquete == NULL)
    {
        // Manejo de error si la creación del paquete falla
        free(interfaz->nombre_interfaz); // Liberar memoria asignada
        free(interfaz);
        return;
    }

    serializarInterfazStdin_de_Kernale_a_Memoria(paquete, interfaz);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);

    // Liberar la memoria asignada para el nombre de la interfaz
    free(interfaz->nombre_interfaz);
    // Liberar la memoria asignada para la estructura de interfaz
    free(interfaz);
}

void serializarInterfazStdin_de_Kernale_a_Memoria(t_paquete *paquete, t_interfaz_stdin *interfaz)
{
    uint32_t tamanio_nombre_interfaz = strlen(interfaz->nombre_interfaz) + 1;
    uint32_t tamanioLista = list_size(interfaz->direccionesFisicas);

    paquete->buffer->size = sizeof(uint32_t) * 3 + tamanio_nombre_interfaz + tamanioLista * (2 * sizeof(uint32_t));
    paquete->buffer->stream = malloc(paquete->buffer->size);
    if (paquete->buffer->stream == NULL)
    {
        // Manejo de error si la asignación de memoria falla
        return;
    }

    int desplazamiento = 0;

    memcpy(paquete->buffer->stream + desplazamiento, &(interfaz->pidPcb), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, &(tamanio_nombre_interfaz), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, interfaz->nombre_interfaz, tamanio_nombre_interfaz);
    desplazamiento += tamanio_nombre_interfaz;

    memcpy(paquete->buffer->stream + desplazamiento, &tamanioLista, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    for (int i = 0; i < tamanioLista; i++)
    {
        t_direcciones_fisicas *dato = list_get(interfaz->direccionesFisicas, i);
        memcpy(paquete->buffer->stream + desplazamiento, &(dato->direccion_fisica), sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
        memcpy(paquete->buffer->stream + desplazamiento, &(dato->tamanio), sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
    }
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
    uint32_t tamanioListDirecciones;
    void *stream = buffer->stream;
    int desplazamiento = 0;

    memcpy(&(interfaz->pidPcb), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(long_interfaz), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    interfaz->nombre_interfaz = malloc(long_interfaz);
    if (interfaz->nombre_interfaz == NULL)
    {
        free(interfaz);
        return NULL;
    }

    memcpy(interfaz->nombre_interfaz, stream + desplazamiento, long_interfaz);
    desplazamiento += long_interfaz;

    memcpy(&(tamanioListDirecciones), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    interfaz->direccionesFisicas = list_create();
    if (interfaz->direccionesFisicas == NULL)
    {
        free(interfaz->nombre_interfaz);
        free(interfaz);
        return NULL;
    }

    for (size_t i = 0; i < tamanioListDirecciones; i++)
    {
        t_direcciones_fisicas *dato = malloc(sizeof(t_direcciones_fisicas));
        if (dato == NULL)
        {
            list_clean_and_destroy_elements(interfaz->direccionesFisicas, free);
            free(interfaz->nombre_interfaz);
            free(interfaz);
            return NULL;
        }
        memcpy(&(dato->direccion_fisica), stream + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
        memcpy(&(dato->tamanio), stream + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        list_add(interfaz->direccionesFisicas, dato);
    }

    return interfaz;
}

void enviar_InterfazStdinConCodigoOPaKernel(int socket, t_list *direcciones_fisicas, uint32_t pid, char *nombre_interfaz)
{
    t_interfaz_stdin *interfaz = malloc(sizeof(t_interfaz_stdin));
    if (interfaz == NULL)
    {
        // Manejo de error si la asignación de memoria falla
        return;
    }
    interfaz->direccionesFisicas = direcciones_fisicas;
    interfaz->pidPcb = pid;
    interfaz->nombre_interfaz = strdup(nombre_interfaz); // Copiar el nombre de interfaz

    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(FINALIZACION_INTERFAZ_STDIN);
    if (paquete == NULL)
    {
        // Manejo de error si la creación del paquete falla
        free(interfaz->nombre_interfaz); // Liberar memoria asignada
        free(interfaz);
        return;
    }

    serializarInterfazStdin_de_Kernale_a_Memoria(paquete, interfaz);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);

    // Liberar la memoria asignada para el nombre de la interfaz
    free(interfaz->nombre_interfaz);
    // Liberar la memoria asignada para la estructura de interfaz
    free(interfaz);
}

void enviar_dato_stdin(int socket, t_list *direcciones_fisicas, char *Dato_a_exscribir_en_memoria, uint32_t pid)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(PEDIDO_ESCRIBIR_DATO_STDIN);
    serializar_dato_stdin(paquete, direcciones_fisicas, Dato_a_exscribir_en_memoria, pid);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}

void serializar_dato_stdin(t_paquete *paquete, t_list *direcciones_fisicas, char *Dato_a_exscribir_en_memoria, uint32_t pid)
{
    uint32_t tamanio_Dato = strlen(Dato_a_exscribir_en_memoria) + 1;
    uint32_t tamanioLista = list_size(direcciones_fisicas);

    paquete->buffer->size = sizeof(uint32_t) + sizeof(uint32_t) + tamanio_Dato + sizeof(uint32_t) + tamanioLista * (2 * sizeof(uint32_t));
    paquete->buffer->stream = malloc(paquete->buffer->size);
    if (paquete->buffer->stream == NULL)
    {
        // Manejo de error si la asignación de memoria falla
        return;
    }

    int desplazamiento = 0;

    memcpy(paquete->buffer->stream + desplazamiento, &(tamanio_Dato), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, Dato_a_exscribir_en_memoria, tamanio_Dato);
    desplazamiento += tamanio_Dato;

    memcpy(paquete->buffer->stream + desplazamiento, &tamanioLista, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    for (int i = 0; i < tamanioLista; i++)
    {
        t_direcciones_fisicas *dato = list_get(direcciones_fisicas, i);
        memcpy(paquete->buffer->stream + desplazamiento, &(dato->direccion_fisica), sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
        memcpy(paquete->buffer->stream + desplazamiento, &(dato->tamanio), sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
    }

    memcpy(paquete->buffer->stream + desplazamiento, &pid, sizeof(uint32_t));
}

char *recibir_dato_stdin(t_list *direcciones_fisicas, int socket_cliente, uint32_t *pid)
{
    char *datoObtenido;
    t_paquete *paquete = recibir_paquete(socket_cliente);
    datoObtenido = deserializar_dato_interfaz_STDIN(paquete, direcciones_fisicas, pid);
    eliminar_paquete(paquete);
    return datoObtenido;
}

char *deserializar_dato_interfaz_STDIN(t_paquete *paquete, t_list *lista_datos, uint32_t *pid)
{
    int desplazamiento = 0;
    char *datoObtenido = NULL; // Inicializar el puntero

    // Deserializar el tamaño del dato
    uint32_t tamanio_Dato;
    memcpy(&tamanio_Dato, paquete->buffer->stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    // Asignar memoria para datoObtenido
    datoObtenido = malloc(tamanio_Dato);
    if (datoObtenido == NULL)
    {
        perror("Error al asignar memoria para datoObtenido");
        return NULL; // O manejar el error según tu lógica de aplicación
    }

    // Deserializar el dato
    memcpy(datoObtenido, paquete->buffer->stream + desplazamiento, tamanio_Dato);
    desplazamiento += tamanio_Dato;

    // Deserializar el tamaño de la lista
    uint32_t tamanioLista;
    memcpy(&tamanioLista, paquete->buffer->stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    // Iterar sobre el buffer y deserializar cada elemento de la lista
    for (size_t i = 0; i < tamanioLista; i++)
    {
        t_direcciones_fisicas *direccion = malloc(sizeof(t_direcciones_fisicas));
        if (direccion == NULL)
        {
            // Manejo de error si la asignación de memoria falla
            perror("Error al asignar memoria");
            return NULL; // O manejar el error según tu lógica de aplicación
        }
        memcpy(&(direccion->direccion_fisica), paquete->buffer->stream + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
        memcpy(&(direccion->tamanio), paquete->buffer->stream + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        list_add(lista_datos, direccion);
    }

    memcpy(pid, paquete->buffer->stream + desplazamiento, sizeof(uint32_t));

    return datoObtenido;
}

// STDOUT

void enviar_interfaz_IO_stdout(t_pcb *pcb_actual, char *interfaz, t_list *direcciones_fisicas, int socket_cliente, nombre_instruccion IO)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(ENVIAR_INTERFAZ_STDOUT);
    serializar_IO_instruccion_stdin(paquete, pcb_actual, interfaz, direcciones_fisicas, IO);
    enviar_paquete(paquete, socket_cliente);
    eliminar_paquete(paquete);
}

void enviar_InterfazStdout(int socket, t_list *direcciones_fisicas, uint32_t pid, char *nombre_interfaz)
{
    t_interfaz_stdout *interfaz = malloc(sizeof(t_interfaz_stdout));
    if (interfaz == NULL)
    {
        // Manejo de error si la asignación de memoria falla
        return;
    }
    interfaz->direccionesFisicas = direcciones_fisicas;
    interfaz->pidPcb = pid;
    interfaz->nombre_interfaz = strdup(nombre_interfaz); // Copiar el nombre de interfaz

    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(PEDIDO_IO_STDOUT_WRITE);
    if (paquete == NULL)
    {
        // Manejo de error si la creación del paquete falla
        free(interfaz->nombre_interfaz); // Liberar memoria asignada
        free(interfaz);
        return;
    }

    serializarInterfazStdout_de_Kernale_a_Memoria(paquete, interfaz);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);

    // Liberar la memoria asignada para el nombre de la interfaz
    free(interfaz->nombre_interfaz);
    // Liberar la memoria asignada para la estructura de interfaz
    free(interfaz);
}

void serializarInterfazStdout_de_Kernale_a_Memoria(t_paquete *paquete, t_interfaz_stdout *interfaz)
{
    uint32_t tamanio_nombre_interfaz = strlen(interfaz->nombre_interfaz) + 1;
    uint32_t tamanioLista = list_size(interfaz->direccionesFisicas);

    paquete->buffer->size = sizeof(uint32_t) * 3 + tamanio_nombre_interfaz + tamanioLista * (2 * sizeof(uint32_t));
    paquete->buffer->stream = malloc(paquete->buffer->size);
    if (paquete->buffer->stream == NULL)
    {
        // Manejo de error si la asignación de memoria falla
        return;
    }

    int desplazamiento = 0;

    memcpy(paquete->buffer->stream + desplazamiento, &(interfaz->pidPcb), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, &(tamanio_nombre_interfaz), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, interfaz->nombre_interfaz, tamanio_nombre_interfaz);
    desplazamiento += tamanio_nombre_interfaz;

    memcpy(paquete->buffer->stream + desplazamiento, &tamanioLista, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    for (int i = 0; i < tamanioLista; i++)
    {
        t_direcciones_fisicas *dato = list_get(interfaz->direccionesFisicas, i);
        memcpy(paquete->buffer->stream + desplazamiento, &(dato->direccion_fisica), sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
        memcpy(paquete->buffer->stream + desplazamiento, &(dato->tamanio), sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
    }
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
    uint32_t tamanioListDirecciones;
    void *stream = buffer->stream;
    int desplazamiento = 0;

    memcpy(&(interfaz->pidPcb), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&(long_interfaz), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    interfaz->nombre_interfaz = malloc(long_interfaz);
    if (interfaz->nombre_interfaz == NULL)
    {
        free(interfaz);
        return NULL;
    }

    memcpy(interfaz->nombre_interfaz, stream + desplazamiento, long_interfaz);
    desplazamiento += long_interfaz;

    memcpy(&(tamanioListDirecciones), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    interfaz->direccionesFisicas = list_create();
    if (interfaz->direccionesFisicas == NULL)
    {
        free(interfaz->nombre_interfaz);
        free(interfaz);
        return NULL;
    }

    for (size_t i = 0; i < tamanioListDirecciones; i++)
    {
        t_direcciones_fisicas *dato = malloc(sizeof(t_direcciones_fisicas));
        if (dato == NULL)
        {
            list_clean_and_destroy_elements(interfaz->direccionesFisicas, free);
            free(interfaz->nombre_interfaz);
            free(interfaz);
            return NULL;
        }
        memcpy(&(dato->direccion_fisica), stream + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
        memcpy(&(dato->tamanio), stream + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        list_add(interfaz->direccionesFisicas, dato);
    }

    return interfaz;
}

/*
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

void enviar_InterfazStdoutConCodigoOPaKernel(int socket, t_list *direcciones_fisicas, uint32_t pid, char *nombre_interfaz)
{
    t_interfaz_stdin *interfaz = malloc(sizeof(t_interfaz_stdin));
    if (interfaz == NULL)
    {
        // Manejo de error si la asignación de memoria falla
        return;
    }
    interfaz->direccionesFisicas = direcciones_fisicas;
    interfaz->pidPcb = pid;
    interfaz->nombre_interfaz = strdup(nombre_interfaz); // Copiar el nombre de interfaz

    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(FINALIZACION_INTERFAZ_STDOUT);
    if (paquete == NULL)
    {
        // Manejo de error si la creación del paquete falla
        free(interfaz->nombre_interfaz); // Liberar memoria asignada
        free(interfaz);
        return;
    }

    serializarInterfazStdin_de_Kernale_a_Memoria(paquete, interfaz);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);

    // Liberar la memoria asignada para el nombre de la interfaz
    free(interfaz->nombre_interfaz);
    // Liberar la memoria asignada para la estructura de interfaz
    free(interfaz);
}

void enviar_direcciones_stdout(int socket, t_list *direcciones_fisicas, uint32_t pid)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(PEDIDO_A_LEER_DATO_STDOUT);
    serializar_direcciones_stdout(paquete, direcciones_fisicas, pid);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}

void serializar_direcciones_stdout(t_paquete *paquete, t_list *direcciones_fisicas, uint32_t pid)
{
    uint32_t tamanioLista = list_size(direcciones_fisicas);

    paquete->buffer->size = sizeof(uint32_t) + sizeof(uint32_t) + tamanioLista * (2 * sizeof(uint32_t));
    paquete->buffer->stream = malloc(paquete->buffer->size);
    if (paquete->buffer->stream == NULL)
    {
        // Manejo de error si la asignación de memoria falla
        return;
    }

    int desplazamiento = 0;

    memcpy(paquete->buffer->stream + desplazamiento, &tamanioLista, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    for (int i = 0; i < tamanioLista; i++)
    {
        t_direcciones_fisicas *dato = list_get(direcciones_fisicas, i);
        memcpy(paquete->buffer->stream + desplazamiento, &(dato->direccion_fisica), sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
        memcpy(paquete->buffer->stream + desplazamiento, &(dato->tamanio), sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
    }
    memcpy(paquete->buffer->stream + desplazamiento, &pid, sizeof(uint32_t));
}

void recibir_direcciones_de_stdout(int socket_cliente, t_list *lista_direcciones, uint32_t *pid)
{
    t_paquete *paquete = recibir_paquete(socket_cliente);
    deserializar_direcciones_de_stdout(paquete, lista_direcciones, pid);
    eliminar_paquete(paquete);
}

void deserializar_direcciones_de_stdout(t_paquete *paquete, t_list *lista_datos, uint32_t *pid)
{
    int desplazamiento = 0;
    uint32_t tamanioLista;

    // Leer el tamaño de la lista del stream
    memcpy(&tamanioLista, paquete->buffer->stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    // Iteramos sobre el buffer y deserializamos cada elemento
    for (uint32_t i = 0; i < tamanioLista; i++)
    {
        t_direcciones_fisicas *dato = malloc(sizeof(t_direcciones_fisicas));
        memcpy(&(dato->direccion_fisica), paquete->buffer->stream + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
        memcpy(&(dato->tamanio), paquete->buffer->stream + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        list_add(lista_datos, dato);
    }
    memcpy(pid, paquete->buffer->stream + desplazamiento, sizeof(uint32_t));
}

void enviar_dato_movIn(int socket, t_list *lista)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(RESPUESTA_DATO_MOVIN);
    // serializar_dato_leido(paquete, dato, tamanio);
    serializar_datos_leidos(paquete, lista);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}

void enviar_dato_leido(int socket, char *dato, int tamanio)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(RESPUESTA_DATO_STDOUT);
    serializar_dato_leido(paquete, dato, tamanio);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}

void serializar_dato_leido(t_paquete *paquete, char *dato, int tamanio)
{
    uint32_t tamanio_dato = tamanio + 1;

    paquete->buffer->size = sizeof(uint32_t) + tamanio_dato;
    paquete->buffer->stream = malloc(paquete->buffer->size);
    if (paquete->buffer->stream == NULL)
    {
        // Manejo de error si la asignación de memoria falla
        return;
    }

    int desplazamiento = 0;

    memcpy(paquete->buffer->stream + desplazamiento, &tamanio_dato, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, dato, tamanio_dato);
}

void serializar_datos_leidos(t_paquete *paquete, t_list *lista)
{
    uint32_t tamanioLista = list_size(lista);

    // Calcular el tamaño total necesario para el buffer
    uint32_t sizeTotal = sizeof(uint32_t); // Para almacenar el tamaño de la lista
    for (int i = 0; i < tamanioLista; i++)
    {
        char *dato = list_get(lista, i);
        uint32_t longitud = strlen(dato) + 1;     // +1 para incluir el carácter nulo
        sizeTotal += sizeof(uint32_t) + longitud; // Para longitud + datos
    }

    paquete->buffer->size = sizeTotal;
    paquete->buffer->stream = malloc(paquete->buffer->size);
    if (paquete->buffer->stream == NULL)
    {
        perror("Error al asignar memoria para paquete->buffer->stream");
        return;
    }

    int desplazamiento = 0;

    // Copiar el tamaño de la lista al inicio del stream
    memcpy(paquete->buffer->stream + desplazamiento, &tamanioLista, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    // Iterar sobre cada elemento de la lista
    for (int i = 0; i < tamanioLista; i++)
    {
        char *dato = list_get(lista, i);

        // Serializar cada dato en el stream
        uint32_t longitud = strlen(dato) + 1; // +1 para incluir el carácter nulo
        memcpy(paquete->buffer->stream + desplazamiento, &longitud, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        memcpy(paquete->buffer->stream + desplazamiento, dato, longitud);
        desplazamiento += longitud;
    }
}

char *recibir_dato_stdout(int socket_cliente)
{
    char *datoObtenido;
    t_paquete *paquete = recibir_paquete(socket_cliente);
    datoObtenido = deserializar_dato_interfaz_STDOUT(paquete);
    eliminar_paquete(paquete);
    return datoObtenido;
}

char *deserializar_dato_interfaz_STDOUT(t_paquete *paquete)
{
    int desplazamiento = 0;
    char *datoObtenido = NULL; // Inicializar el puntero

    // Deserializar el tamaño del dato
    uint32_t tamanio_Dato;
    memcpy(&tamanio_Dato, paquete->buffer->stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    // Asignar memoria para datoObtenido
    datoObtenido = malloc(tamanio_Dato);
    if (datoObtenido == NULL)
    {
        perror("Error al asignar memoria para datoObtenido");
        return NULL; // O manejar el error según tu lógica de aplicación
    }

    // Deserializar el dato
    memcpy(datoObtenido, paquete->buffer->stream + desplazamiento, tamanio_Dato);
    desplazamiento += tamanio_Dato;

    return datoObtenido;
}

char *recibir_dato(int socket, t_log *logger)
{
    op_cod cop;
    char *datoRecibido = NULL;

    recv(socket, &cop, sizeof(op_cod), 0);
    switch (cop)
    {
    case RESPUESTA_DATO_STDOUT:
        datoRecibido = recibir_dato_stdout(socket);
        break;

    default:
        log_error(logger, "No se pudo recibir el dato");
        break;
    }

    if (datoRecibido == NULL)
    {
        log_error(logger, "No se leyo nada de memoria");
        return NULL;
    }

    return datoRecibido;
}