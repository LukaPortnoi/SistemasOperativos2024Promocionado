#include "../include/instrucciones_io.h"

void procesar_sleep(int socket_cliente)
{
    t_interfaz_gen *interfazRecibida = recibir_InterfazGenerica(socket_cliente);
    log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Operacion: IO_GEN_SLEEP", interfazRecibida->pidPcb);

    log_trace(LOGGER_INPUT_OUTPUT, "Durmiendo: %d segundos", interfazRecibida->unidades_de_trabajo);
    sleep(interfazRecibida->unidades_de_trabajo);

    enviar_InterfazGenericaConCodigoOP(socket_cliente, interfazRecibida->unidades_de_trabajo, interfazRecibida->pidPcb, interfazRecibida->nombre_interfaz);

    free(interfazRecibida->nombre_interfaz);
    free(interfazRecibida);
}

void procesar_stdin(int socket_cliente)
{
    t_interfaz_stdin *interfazRecibida = recibir_InterfazStdin(socket_cliente);
    log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Operacion: IO_STDIN_READ", interfazRecibida->pidPcb);

    uint32_t tamanioTotal = 0;
    for (int i = 0; i < list_size(interfazRecibida->direccionesFisicas); i++)
    {
        t_direcciones_fisicas *direccionAmostrar = list_get(interfazRecibida->direccionesFisicas, i);
        tamanioTotal = tamanioTotal + direccionAmostrar->tamanio;
    }

    char *datoRecibido;
    datoRecibido = procesarIngresoUsuario(tamanioTotal);
    enviar_dato_stdin(fd_io_memoria, interfazRecibida->direccionesFisicas, datoRecibido, interfazRecibida->pidPcb);
    free(datoRecibido);

    enviar_InterfazStdinConCodigoOPaKernel(socket_cliente, interfazRecibida->direccionesFisicas, interfazRecibida->pidPcb, interfazRecibida->nombre_interfaz);
}

char *procesarIngresoUsuario(uint32_t tamanioMaximo)
{
    char *leido = readline("> ");
    int longitud_bytes = strlen(leido);

    while (longitud_bytes > tamanioMaximo)
    {
        log_error(LOGGER_INPUT_OUTPUT, "El dato ingresado supera el tamaño máximo permitido. Por favor, vuelva a ingresar el dato.");
        free(leido);
        leido = readline("> ");
        longitud_bytes = strlen(leido);
    }
    return leido;
}

void procesar_stdout(int socket_cliente)
{
    t_interfaz_stdout *interfazRecibida = recibir_InterfazStdout(socket_cliente);
    log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Operacion: IO_STDOUT_WRITE", interfazRecibida->pidPcb);

    enviar_direcciones_stdout(fd_io_memoria, interfazRecibida->direccionesFisicas, interfazRecibida->pidPcb);

    char *datoRecibido = recibir_dato(fd_io_memoria, LOGGER_INPUT_OUTPUT);
    log_info(LOGGER_INPUT_OUTPUT, "Información leída: %s", datoRecibido);

    enviar_InterfazStdoutConCodigoOPaKernel(socket_cliente, interfazRecibida->direccionesFisicas, interfazRecibida->pidPcb, interfazRecibida->nombre_interfaz);
}

void procesar_dialfs_create(int socket_cliente)
{
    t_interfaz_dialfs *interfazRecibida = crearInterfazDialfs();
    recibir_InterfazDialfs(socket_cliente, interfazRecibida, PEDIDO_IO_FS_CREATE);
    log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Operacion: IO_FS_CREATE", interfazRecibida->pidPcb);

    char metadata_path[256];
    snprintf(metadata_path, sizeof(metadata_path), "%s/%s", PATH_BASE_DIALFS, interfazRecibida->nombre_archivo);

    if (access(metadata_path, F_OK) == 0)
    {
        log_error(LOGGER_INPUT_OUTPUT, "PID: %d - Error: El archivo %s ya existe", interfazRecibida->pidPcb, interfazRecibida->nombre_archivo);
        enviar_create_delete_terminado(socket_cliente, interfazRecibida->pidPcb, interfazRecibida->nombre_interfaz);
        destroyInterfazDialfs(interfazRecibida);
        return;
    }

    FILE *file = fopen(metadata_path, "w");
    fclose(file);

    t_config *metadata_config = config_create(metadata_path);

    FILE *bitmap_file = fopen(BITMAP_PATH, "r+");
    FILE *bloques_file = fopen(BLOQUES_PATH, "r+");

    size_t bitmap_size = BLOCK_COUNT / 8;
    char *bitmap = mmap(NULL, bitmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(bitmap_file), 0);
    char *bloques = mmap(NULL, BLOCK_SIZE * BLOCK_COUNT, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(bloques_file), 0);

    uint32_t bloque_inicial = encontrar_bloque_libre();

    if (bloque_inicial == -1)
    {
        log_error(LOGGER_INPUT_OUTPUT, "No hay bloques libres para crear el archivo");
        // avisarle a kernel que no se pudo crear el archivo
    }
    else
    {
        log_debug(LOGGER_INPUT_OUTPUT, "Bloque inicial: %d", bloque_inicial);

        // Marcar el bloque como ocupado en el bitmap
        int byte_index = bloque_inicial / 8;
        int bit_index = bloque_inicial % 8;
        bitmap[byte_index] |= (1 << bit_index);

        actualizar_bloque_inicial(metadata_config, bloque_inicial);
        actualizar_tamanio_archivo(metadata_config, 0);

        config_save(metadata_config);

        log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Crear archivo: %s", interfazRecibida->pidPcb, interfazRecibida->nombre_archivo);
    }

    config_destroy(metadata_config);
    munmap(bitmap, bitmap_size);
    munmap(bloques, BLOCK_SIZE * BLOCK_COUNT);
    fclose(bitmap_file);
    fclose(bloques_file);

    log_trace(LOGGER_INPUT_OUTPUT, "Durmiendo: %f segundos", (float)TIEMPO_UNIDAD_TRABAJO / 1000);
    usleep(TIEMPO_UNIDAD_TRABAJO * 1000);

    enviar_create_delete_terminado(socket_cliente, interfazRecibida->pidPcb, interfazRecibida->nombre_interfaz);
    destroyInterfazDialfs(interfazRecibida);
}

void procesar_dialfs_delete(int socket_cliente)
{
    t_interfaz_dialfs *interfazRecibida = crearInterfazDialfs();
    recibir_InterfazDialfs(socket_cliente, interfazRecibida, PEDIDO_IO_FS_DELETE);
    log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Operacion: IO_FS_DELETE", interfazRecibida->pidPcb);

    FILE *bitmap_file = fopen(BITMAP_PATH, "r+");
    FILE *bloques_file = fopen(BLOQUES_PATH, "r+");

    size_t bitmap_size = BLOCK_COUNT / 8;
    char *bitmap = mmap(NULL, bitmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(bitmap_file), 0);

    char metadata_path[256];
    snprintf(metadata_path, sizeof(metadata_path), "%s/%s", PATH_BASE_DIALFS, interfazRecibida->nombre_archivo);

    if (access(metadata_path, F_OK) != 0)
    {
        log_error(LOGGER_INPUT_OUTPUT, "PID: %d - Error: El archivo a eliminar %s no existe", interfazRecibida->pidPcb, interfazRecibida->nombre_archivo);
        enviar_create_delete_terminado(socket_cliente, interfazRecibida->pidPcb, interfazRecibida->nombre_interfaz);
        destroyInterfazDialfs(interfazRecibida);
        return;
    }

    uint32_t bloque_inicial = obtener_bloque_inicial(metadata_path);

    log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Eliminar archivo: %s", interfazRecibida->pidPcb, interfazRecibida->nombre_archivo);

    // Marcar el bloque como libre en el bitmap
    int byte_index = bloque_inicial / 8;
    int bit_index = bloque_inicial % 8;
    bitmap[byte_index] &= ~(1 << bit_index);

    remove(metadata_path);

    munmap(bitmap, bitmap_size);
    fclose(bitmap_file);
    fclose(bloques_file);

    log_trace(LOGGER_INPUT_OUTPUT, "Durmiendo: %f segundos", (float)TIEMPO_UNIDAD_TRABAJO / 1000);
    usleep(TIEMPO_UNIDAD_TRABAJO * 1000);

    enviar_create_delete_terminado(socket_cliente, interfazRecibida->pidPcb, interfazRecibida->nombre_interfaz);
    destroyInterfazDialfs(interfazRecibida);
}

void procesar_dialfs_truncate(int socket_cliente)
{
    /* t_interfaz_dialfs *interfazRecibida = recibir_InterfazDialfs(socket_cliente);
    log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Operacion: IO_FS_TRUNCATE", interfazRecibida->pidPcb);
    log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Truncar archivo: %s - Tamaño: %d", interfazRecibida->pidPcb, interfazRecibida->nombre_archivo, interfazRecibida->tamanio);
 */
}

void procesar_dialfs_write(int socket_cliente)
{
    /* t_interfaz_dialfs *interfazRecibida = recibir_InterfazDialfs(socket_cliente);
    log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Operacion: IO_FS_WRITE", interfazRecibida->pidPcb);
    log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Escribir archivo: %s - Tamaño a Escribir: %d - Puntero Archivo: %d", interfazRecibida->pidPcb, interfazRecibida->nombre_archivo, interfazRecibida->tamanio, interfazRecibida->puntero_archivo);
 */
}

void procesar_dialfs_read(int socket_cliente)
{
    /* t_interfaz_dialfs *interfazRecibida = recibir_InterfazDialfs(socket_cliente);
    log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Operacion: IO_FS_READ", interfazRecibida->pidPcb);
    log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Leer archivo: %s - Tamaño a Leer: %d - Puntero Archivo: %d", interfazRecibida->pidPcb, interfazRecibida->nombre_archivo, interfazRecibida->tamanio, interfazRecibida->puntero_archivo);
 */
}