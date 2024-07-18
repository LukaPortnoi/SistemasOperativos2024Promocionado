#include "../include/instrucciones_io.h"

void procesar_sleep(int socket_cliente)
{
    t_interfaz_gen *interfazRecibida = recibir_InterfazGenerica(socket_cliente);
    log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Operacion: IO_GEN_SLEEP", interfazRecibida->pidPcb);

    uint32_t tiempo = interfazRecibida->unidades_de_trabajo * TIEMPO_UNIDAD_TRABAJO;
    float tiempo_segundos = tiempo / 1000.0;

    log_trace(LOGGER_INPUT_OUTPUT, "Durmiendo: %.2f segundos", tiempo_segundos);
    usleep(tiempo);

    enviar_InterfazGenericaConCodigoOP(socket_cliente, interfazRecibida->unidades_de_trabajo, interfazRecibida->pidPcb, interfazRecibida->nombre_interfaz);

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
    destroyInterfazStdin(interfazRecibida);
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
    free(datoRecibido); // NUEVONUEVO

    enviar_InterfazStdoutConCodigoOPaKernel(socket_cliente, interfazRecibida->direccionesFisicas, interfazRecibida->pidPcb, interfazRecibida->nombre_interfaz);
    destroyInterfazStdout(interfazRecibida);
}

void procesar_dialfs_create(int socket_cliente)
{
    t_interfaz_dialfs *interfazRecibida = crearInterfazDialfs();
    recibir_InterfazDialfs(socket_cliente, interfazRecibida, PEDIDO_IO_FS_CREATE);
    log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Operacion: IO_FS_CREATE", interfazRecibida->pidPcb);

    char metadata_path[256];
    obtener_metadata_path(interfazRecibida->nombre_archivo, metadata_path, sizeof(metadata_path));

    if (access(metadata_path, F_OK) == 0)
    {
        log_error(LOGGER_INPUT_OUTPUT, "PID: %d - Error: El archivo %s ya existe", interfazRecibida->pidPcb, interfazRecibida->nombre_archivo);
        enviar_dialfs_terminado(socket_cliente, interfazRecibida->pidPcb, interfazRecibida->nombre_interfaz);
        destroyInterfazDialfs(interfazRecibida);
        return;
    }

    FILE *file = fopen(metadata_path, "w");
    fclose(file);

    t_config *metadata_config = config_create(metadata_path);

    FILE *bitmap_file = fopen(BITMAP_PATH, "r+");

    size_t bitmap_size = BLOCK_COUNT / 8;
    char *bitmap = mmap(NULL, bitmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(bitmap_file), 0);

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

        msync(bitmap, bitmap_size, MS_SYNC);

        actualizar_bloque_inicial(metadata_config, bloque_inicial);
        actualizar_tamanio_archivo(metadata_config, 0);

        config_save(metadata_config);

        list_add(ARCHIVOS_EN_FS, crear_archivo(interfazRecibida->nombre_archivo, bloque_inicial, 0));

        log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Crear archivo: %s", interfazRecibida->pidPcb, interfazRecibida->nombre_archivo);
    }

    config_destroy(metadata_config);
    munmap(bitmap, bitmap_size);
    fclose(bitmap_file);
    usleep(TIEMPO_UNIDAD_TRABAJO * 1000);
    enviar_dialfs_terminado(socket_cliente, interfazRecibida->pidPcb, interfazRecibida->nombre_interfaz);
    destroyInterfazDialfs(interfazRecibida);
}

void procesar_dialfs_delete(int socket_cliente)
{
    t_interfaz_dialfs *interfazRecibida = crearInterfazDialfs();
    recibir_InterfazDialfs(socket_cliente, interfazRecibida, PEDIDO_IO_FS_DELETE);
    log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Operacion: IO_FS_DELETE", interfazRecibida->pidPcb);

    FILE *bitmap_file = fopen(BITMAP_PATH, "r+");

    size_t bitmap_size = BLOCK_COUNT / 8;
    char *bitmap = mmap(NULL, bitmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(bitmap_file), 0);

    char metadata_path[256];
    obtener_metadata_path(interfazRecibida->nombre_archivo, metadata_path, sizeof(metadata_path));

    if (access(metadata_path, F_OK) != 0)
    {
        log_error(LOGGER_INPUT_OUTPUT, "PID: %d - Error: El archivo a eliminar %s no existe", interfazRecibida->pidPcb, interfazRecibida->nombre_archivo);
        enviar_dialfs_terminado(socket_cliente, interfazRecibida->pidPcb, interfazRecibida->nombre_interfaz);
        destroyInterfazDialfs(interfazRecibida);
        return;
    }

    uint32_t bloque_inicial = obtener_bloque_inicial(metadata_path);

    eliminar_archivo_por_nombre(interfazRecibida->nombre_archivo);

    log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Eliminar archivo: %s", interfazRecibida->pidPcb, interfazRecibida->nombre_archivo);

    // marcar todos los bloques del archivo como libres
    int tamanio_archivo = obtener_tamanio_archivo(metadata_path);
    int bloques_ocupados = (tamanio_archivo + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (bloques_ocupados == 0)
    {
        bloques_ocupados = 1;
    }

    for (uint32_t i = 0; i < bloques_ocupados; i++)
    {
        int bloque_a_liberar = bloque_inicial + i;
        int byte_index = bloque_a_liberar / 8;
        int bit_index = bloque_a_liberar % 8;
        bitmap[byte_index] &= ~(1 << bit_index);
    }

    msync(bitmap, bitmap_size, MS_SYNC);

    remove(metadata_path);

    munmap(bitmap, bitmap_size);
    fclose(bitmap_file);
    usleep(TIEMPO_UNIDAD_TRABAJO * 1000);
    enviar_dialfs_terminado(socket_cliente, interfazRecibida->pidPcb, interfazRecibida->nombre_interfaz);
    destroyInterfazDialfs(interfazRecibida);
}

/* Compruebo si se esta queriendo achicar el archivo, si es asi,
se achica marcando como libres los bloques que ya no son necesarios.
Despues, si se quiere agrandar, busco bloques libres contiguos en donde
entren los bloques necesarios. Si no se encuentra, se cuenta la cantidad
de bloques libres en el bitarray, y si hay disponibles pero no contiguos,
se compacta. Si no, se avisa que no se pudo truncar */
void procesar_dialfs_truncate(int socket_cliente)
{
    t_interfaz_dialfs *interfazRecibida = crearInterfazDialfs();
    recibir_InterfazDialfs(socket_cliente, interfazRecibida, PEDIDO_IO_FS_TRUNCATE);
    log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Operacion: IO_FS_TRUNCATE", interfazRecibida->pidPcb);

    log_debug(LOGGER_INPUT_OUTPUT, "Antes de truncar:");
    imprimir_bitmap();

    char metadata_path[256];
    obtener_metadata_path(interfazRecibida->nombre_archivo, metadata_path, sizeof(metadata_path));

    t_config *metadata_config = config_create(metadata_path);

    FILE *bitmap_file = fopen(BITMAP_PATH, "r+");
    FILE *bloques_file = fopen(BLOQUES_PATH, "r+");

    size_t bitmap_size = BLOCK_COUNT / 8;
    char *bitmap = mmap(NULL, bitmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(bitmap_file), 0);
    char *bloques = mmap(NULL, BLOCK_SIZE * BLOCK_COUNT, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(bloques_file), 0);

    uint32_t bloque_inicial = obtener_bloque_inicial_por_nombre(interfazRecibida->nombre_archivo);
    uint32_t nuevo_tamanio = interfazRecibida->tamanio;

    uint32_t bloques_necesarios = (nuevo_tamanio + BLOCK_SIZE - 1) / BLOCK_SIZE;
    uint32_t bloques_ocupados = (obtener_tamanio_archivo(metadata_path) + BLOCK_SIZE - 1) / BLOCK_SIZE;

    log_error(LOGGER_INPUT_OUTPUT, "Bloque inicial: %d", bloque_inicial);

    if (nuevo_tamanio < bloques_ocupados * BLOCK_SIZE)
    {
        // Achicar el archivo
        for (uint32_t i = bloques_necesarios; i < bloques_ocupados; i++)
        {
            int bloque_a_liberar = bloque_inicial + i;
            int byte_index = bloque_a_liberar / 8;
            int bit_index = bloque_a_liberar % 8;
            bitmap[byte_index] &= ~(1 << bit_index);
            memset(bloques + (bloque_a_liberar * BLOCK_SIZE), 0, BLOCK_SIZE);
        }
        actualizar_tamanio_archivo(metadata_config, nuevo_tamanio);
        config_save(metadata_config);
        actualizar_archivo_en_lista(interfazRecibida->nombre_archivo, bloque_inicial, nuevo_tamanio);
        log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Truncar archivo: %s - Tamaño: %d", interfazRecibida->pidPcb, interfazRecibida->nombre_archivo, nuevo_tamanio);
    }
    else if (nuevo_tamanio > bloques_ocupados * BLOCK_SIZE)
    {
        uint32_t primer_bloque_libre_de_los_contiguos = encontrar_bloques_libres_contiguos_top(bloque_inicial, bloques_necesarios, bloques_ocupados, bitmap);

        if (primer_bloque_libre_de_los_contiguos == -1)
        {
            uint32_t bloques_libres = contar_bloques_libres(bitmap);
            if (bloques_libres >= bloques_necesarios)
            {
                compactar_dialfs(interfazRecibida->pidPcb);
                bloque_inicial = obtener_bloque_inicial_por_nombre(interfazRecibida->nombre_archivo);
                primer_bloque_libre_de_los_contiguos = encontrar_bloques_libres_contiguos_top(0, bloques_necesarios, bloques_ocupados, bitmap); // Buscar desde el inicio después de compactar
            }
        }

        if (primer_bloque_libre_de_los_contiguos == -1)
        {
            log_error(LOGGER_INPUT_OUTPUT, "No hay bloques libres para truncar el archivo");
            config_destroy(metadata_config);
            munmap(bitmap, bitmap_size);
            munmap(bloques, BLOCK_SIZE * BLOCK_COUNT);
            fclose(bitmap_file);
            fclose(bloques_file);
            destroyInterfazDialfs(interfazRecibida);
            return;
        }
        else if (primer_bloque_libre_de_los_contiguos != bloque_inicial)
        {
            log_trace(LOGGER_INPUT_OUTPUT, "El archivo se movió de posición inicial a %d", primer_bloque_libre_de_los_contiguos);
            log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Inicio Compactación.", interfazRecibida->pidPcb);
            int bloques_ocupados_aux = 0;

            if (bloques_ocupados == 0)
            {
                bloques_ocupados_aux = 1;
            }
            else
            {
                bloques_ocupados_aux = bloques_ocupados;
            }
            log_warning(LOGGER_INPUT_OUTPUT, "Bloques ocupados: %d", bloques_ocupados_aux);
            // Marcar los bloques originales como libres si el archivo se movió
            for (uint32_t i = 0; i < bloques_ocupados_aux; i++)
            {   
                log_error(LOGGER_INPUT_OUTPUT, "Bloque inicial: %d", bloque_inicial);
                log_warning(LOGGER_INPUT_OUTPUT, "Liberando bloque %d", bloque_inicial + i);
                int bloque_a_liberar = bloque_inicial + i;
                int byte_index = bloque_a_liberar / 8;
                int bit_index = bloque_a_liberar % 8;
                bitmap[byte_index] &= ~(1 << bit_index);
            }
            log_warning(LOGGER_INPUT_OUTPUT, "Bloques necesarios: %d", bloques_necesarios);
            // Marcar los bloques nuevos como ocupados
            for (uint32_t i = 0; i < bloques_necesarios; i++)
            {
                int nuevo_bloque = primer_bloque_libre_de_los_contiguos + i;
                int byte_index = nuevo_bloque / 8;
                int bit_index = nuevo_bloque % 8;
                bitmap[byte_index] |= (1 << bit_index);
            }

            // Mover la información a los nuevos bloques a la nueva posicion con memmove
            memmove(bloques + (primer_bloque_libre_de_los_contiguos * BLOCK_SIZE), bloques + (bloque_inicial * BLOCK_SIZE), bloques_ocupados_aux * BLOCK_SIZE);

            usleep(RETRASO_COMPACTACION * 1000);
            log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Fin Compactación.", interfazRecibida->pidPcb);
        }
        else if (primer_bloque_libre_de_los_contiguos == bloque_inicial)
        {
            log_trace(LOGGER_INPUT_OUTPUT, "El archivo no se movió porque su posición inicial es la misma que la nueva");
            for (uint32_t i = 0; i < bloques_necesarios; i++)
            {
                int nuevo_bloque = primer_bloque_libre_de_los_contiguos + i;
                int byte_index = nuevo_bloque / 8;
                int bit_index = nuevo_bloque % 8;
                bitmap[byte_index] |= (1 << bit_index);
            }
        }

        log_warning(LOGGER_INPUT_OUTPUT, "Actualizando metadata, bloque inicial: %d, tamanio: %d", primer_bloque_libre_de_los_contiguos, nuevo_tamanio);
        actualizar_bloque_inicial(metadata_config, primer_bloque_libre_de_los_contiguos);
        actualizar_tamanio_archivo(metadata_config, nuevo_tamanio);
        config_save(metadata_config);
        actualizar_archivo_en_lista(interfazRecibida->nombre_archivo, primer_bloque_libre_de_los_contiguos, nuevo_tamanio);
        log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Truncar archivo: %s - Tamaño: %d", interfazRecibida->pidPcb, interfazRecibida->nombre_archivo, nuevo_tamanio);
    }

    msync(bitmap, bitmap_size, MS_SYNC);
    msync(bloques, BLOCK_SIZE * BLOCK_COUNT, MS_SYNC);
    log_debug(LOGGER_INPUT_OUTPUT, "Despues de truncar:");
    imprimir_bitmap();

    config_destroy(metadata_config);
    munmap(bitmap, bitmap_size);
    munmap(bloques, BLOCK_SIZE * BLOCK_COUNT);
    fclose(bitmap_file);
    fclose(bloques_file);

    usleep(TIEMPO_UNIDAD_TRABAJO * 1000);
    enviar_dialfs_terminado(socket_cliente, interfazRecibida->pidPcb, interfazRecibida->nombre_interfaz);
    destroyInterfazDialfs(interfazRecibida);
}

void procesar_dialfs_write(int socket_cliente)
{
    t_interfaz_dialfs *interfazRecibida = crearInterfazDialfs();
    recibir_InterfazDialfs(socket_cliente, interfazRecibida, PEDIDO_IO_FS_WRITE);
    log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Operacion: IO_FS_WRITE", interfazRecibida->pidPcb);

    char metadata_path[256];
    obtener_metadata_path(interfazRecibida->nombre_archivo, metadata_path, sizeof(metadata_path));

    FILE *bloques_file = fopen(BLOQUES_PATH, "r+");
    char *bloques = mmap(NULL, BLOCK_SIZE * BLOCK_COUNT, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(bloques_file), 0);
    //printeamos las listas que enviamos
    for (int i = 0; i < list_size(interfazRecibida->direcciones); i++)
    {
        t_direcciones_fisicas *direccionAmostrar = list_get(interfazRecibida->direcciones, i);
        log_debug(LOGGER_INPUT_OUTPUT, "Dirección: %d - Tamaño: %d", direccionAmostrar->direccion_fisica, direccionAmostrar->tamanio);
    }
    enviar_direcciones_stdout(fd_io_memoria, interfazRecibida->direcciones, interfazRecibida->pidPcb);
    char *datoRecibido = recibir_dato(fd_io_memoria, LOGGER_INPUT_OUTPUT);
    log_debug(LOGGER_INPUT_OUTPUT, "Información leída: %s", datoRecibido);
    escribir_dato_archivo(datoRecibido, interfazRecibida->puntero_archivo, bloques, obtener_bloque_inicial_por_nombre(interfazRecibida->nombre_archivo));

    log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Escribir archivo: %s - Tamaño a Escribir: %d - Puntero Archivo: %s", interfazRecibida->pidPcb, interfazRecibida->nombre_archivo, interfazRecibida->tamanio, interfazRecibida->puntero_archivo);

    msync(bloques, BLOCK_SIZE * BLOCK_COUNT, MS_SYNC);
    munmap(bloques, BLOCK_SIZE * BLOCK_COUNT);
    fclose(bloques_file);

    usleep(TIEMPO_UNIDAD_TRABAJO * 1000);
    enviar_dialfs_terminado(socket_cliente, interfazRecibida->pidPcb, interfazRecibida->nombre_interfaz);
    destroyInterfazDialfs(interfazRecibida);
}

void procesar_dialfs_read(int socket_cliente)
{
    t_interfaz_dialfs *interfazRecibida = crearInterfazDialfs();
    recibir_InterfazDialfs(socket_cliente, interfazRecibida, PEDIDO_IO_FS_READ);
    log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Operacion: IO_FS_READ", interfazRecibida->pidPcb);

    char metadata_path[256];
    obtener_metadata_path(interfazRecibida->nombre_archivo, metadata_path, sizeof(metadata_path));

    FILE *bloques_file = fopen(BLOQUES_PATH, "r+");
    char *bloques = mmap(NULL, BLOCK_SIZE * BLOCK_COUNT, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(bloques_file), 0);

    char *dato_a_enviar = leer_dato_archivo(interfazRecibida->tamanio, interfazRecibida->puntero_archivo, bloques, obtener_bloque_inicial(metadata_path));
    log_debug(LOGGER_INPUT_OUTPUT, "Información leída: %s", dato_a_enviar);
    enviar_dato_stdin(fd_io_memoria, interfazRecibida->direcciones, dato_a_enviar, interfazRecibida->pidPcb);

    log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Leer archivo: %s - Tamaño a Leer: %d - Puntero Archivo: %s", interfazRecibida->pidPcb, interfazRecibida->nombre_archivo, interfazRecibida->tamanio, interfazRecibida->puntero_archivo);

    msync(bloques, BLOCK_SIZE * BLOCK_COUNT, MS_SYNC);
    munmap(bloques, BLOCK_SIZE * BLOCK_COUNT);
    fclose(bloques_file);

    usleep(TIEMPO_UNIDAD_TRABAJO * 1000);
    enviar_dialfs_terminado(socket_cliente, interfazRecibida->pidPcb, interfazRecibida->nombre_interfaz);
    destroyInterfazDialfs(interfazRecibida);
}