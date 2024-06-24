#include "../include/dialfs.h"

// Fijarse si existen los archivos bloques.dat y bitmap.dat en el PATH_BASE_DIALFS. Si no existen, crearlos.
void manejar_archivos_fs()
{
    // Verificar y crear bloques.dat si no existe
    FILE *bloques_file = fopen(BLOQUES_PATH, "rb+");
    if (bloques_file == NULL)
    {
        // El archivo no existe, crearlo
        bloques_file = fopen(BLOQUES_PATH, "wb+");
        if (bloques_file == NULL)
        {
            log_error(LOGGER_INPUT_OUTPUT, "Error al crear bloques.dat. No existe la carpeta %s", PATH_BASE_DIALFS);
            exit(EXIT_FAILURE);
        }

        // Establecer el tamaño del archivo
        if (ftruncate(fileno(bloques_file), BLOCK_SIZE * BLOCK_COUNT) == -1)
        {
            log_error(LOGGER_INPUT_OUTPUT, "Error al establecer el tamaño de bloques.dat");
            fclose(bloques_file);
            exit(EXIT_FAILURE);
        }
    }
    fclose(bloques_file);
    log_trace(LOGGER_INPUT_OUTPUT, "Archivo bloques.dat verificado/creado con éxito");

    // Verificar y crear bitmap.dat si no existe
    FILE *bitmap_file = fopen(BITMAP_PATH, "rb+");
    if (bitmap_file == NULL)
    {
        // El archivo no existe, crearlo
        bitmap_file = fopen(BITMAP_PATH, "wb+");
        if (bitmap_file == NULL)
        {
            log_error(LOGGER_INPUT_OUTPUT, "Error al crear bitmap.dat. No existe la carpeta %s", PATH_BASE_DIALFS);
            exit(EXIT_FAILURE);
        }

        size_t bitmap_size = (BLOCK_COUNT + 7) / 8;
        char *bitmap_data = calloc(bitmap_size, sizeof(char));
        if (bitmap_data == NULL)
        {
            log_error(LOGGER_INPUT_OUTPUT, "Error al asignar memoria para el bitmap");
            fclose(bitmap_file);
            exit(EXIT_FAILURE);
        }

        // Inicializar el bitmap utilizando la librería
        t_bitarray *bitarray = bitarray_create_with_mode(bitmap_data, bitmap_size, LSB_FIRST);
        if (bitarray == NULL)
        {
            log_error(LOGGER_INPUT_OUTPUT, "Error al crear bitarray");
            free(bitmap_data);
            fclose(bitmap_file);
            exit(EXIT_FAILURE);
        }

        // Escribir el bitmap inicial (todo en 0, bloques libres)
        if (fwrite(bitarray->bitarray, 1, bitmap_size, bitmap_file) != bitmap_size)
        {
            log_error(LOGGER_INPUT_OUTPUT, "Error al escribir en bitmap.dat");
            bitarray_destroy(bitarray);
            free(bitmap_data);
            fclose(bitmap_file);
            exit(EXIT_FAILURE);
        }

        bitarray_destroy(bitarray);
        free(bitmap_data);
    }
    fclose(bitmap_file);
    log_trace(LOGGER_INPUT_OUTPUT, "Archivo bitmap.dat verificado/creado con éxito");
}

void actualizar_bloque_inicial(t_config *metadata_config, uint32_t bloque_inicial)
{
    char *bloque_inicial_str = string_itoa(bloque_inicial);
    config_set_value(metadata_config, "BLOQUE_INICIAL", bloque_inicial_str);
    free(bloque_inicial_str);
}

void actualizar_tamanio_archivo(t_config *metadata_config, uint32_t tamanio)
{
    char *tamanio_str = string_itoa(tamanio);
    config_set_value(metadata_config, "TAMANIO", tamanio_str);
    free(tamanio_str);
}

uint32_t obtener_bloque_inicial(char path[])
{
    t_config *metadata_config = iniciar_config(path, "METADATA");
    uint32_t bloque_inicial = config_get_int_value(metadata_config, "BLOQUE_INICIAL");
    config_destroy(metadata_config);
    return bloque_inicial;
}

uint32_t obtener_tamanio_archivo(char path[])
{
    t_config *metadata_config = iniciar_config(path, "METADATA");
    uint32_t tamanio = config_get_int_value(metadata_config, "TAMANIO");
    config_destroy(metadata_config);
    return tamanio;
}

char *obtener_metadata_path(t_interfaz_dialfs *interfazRecibida, char *metadata_path, size_t size)
{
    snprintf(metadata_path, size, "%s/%s", PATH_BASE_DIALFS, interfazRecibida->nombre_archivo);
    return metadata_path;
}

uint32_t encontrar_bloque_libre()
{
    FILE *bitmap_file = fopen(BITMAP_PATH, "r+");

    size_t bitmap_size = BLOCK_COUNT / 8;
    char *bitmap = mmap(NULL, bitmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(bitmap_file), 0);

    for (int i = 0; i < BLOCK_COUNT; i++)
    {
        int byte_index = i / 8;
        int bit_index = i % 8;
        if (!(bitmap[byte_index] & (1 << bit_index)))
        {
            fclose(bitmap_file);
            munmap(bitmap, bitmap_size);
            return i;
        }
    }

    fclose(bitmap_file);
    munmap(bitmap, bitmap_size);
    return -1;
}

uint32_t encontrar_bloques_libres_contiguos(uint32_t bloque_inicial, uint32_t bloques_necesarios)
{
    FILE *bitmap_file = fopen(BITMAP_PATH, "r+");
    size_t bitmap_size = BLOCK_COUNT / 8;
    char *bitmap = mmap(NULL, bitmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(bitmap_file), 0);

    uint32_t contador = 0;

    // Verificar desde bloque_inicial y hacia adelante
    for (uint32_t i = bloque_inicial; i < BLOCK_COUNT; i++)
    {
        int byte_index = i / 8;
        int bit_index = i % 8;
        if (!(bitmap[byte_index] & (1 << bit_index)))
        {
            contador++;
            if (contador == bloques_necesarios)
            {
                fclose(bitmap_file);
                munmap(bitmap, bitmap_size);
                return bloque_inicial; // Devolver el bloque_inicial si hay suficientes bloques libres contiguos
            }
        }
        else
        {
            contador = 0;
        }
    }

    // Si no encontró suficientes bloques libres contiguos desde bloque_inicial, buscar desde el inicio
    contador = 0;
    for (uint32_t i = 0; i < BLOCK_COUNT; i++)
    {
        int byte_index = i / 8;
        int bit_index = i % 8;
        if (!(bitmap[byte_index] & (1 << bit_index)))
        {
            contador++;
            if (contador == bloques_necesarios)
            {
                fclose(bitmap_file);
                munmap(bitmap, bitmap_size);
                return i - bloques_necesarios + 1;
            }
        }
        else
        {
            contador = 0;
        }
    }

    fclose(bitmap_file);
    munmap(bitmap, bitmap_size);
    return -1;
}

uint32_t contar_bloques_libres(char *bitmap)
{
    uint32_t contador = 0;
    for (uint32_t i = 0; i < BLOCK_COUNT; i++)
    {
        int byte_index = i / 8;
        int bit_index = i % 8;
        if (!(bitmap[byte_index] & (1 << bit_index)))
        {
            contador++;
        }
    }
    return contador;
}

void compactar_dialfs(uint32_t pid)
{
    log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Inicio Compactación.", pid);

    // Implementación pendiente

    usleep(RETRASO_COMPACTACION * 1000);
    log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Fin Compactación.", pid);
}

void escribir_dato_archivo(char *datoRecibido, char *puntero_archivo, char *bloques, uint32_t bloque_inicial)
{
    uint32_t offset_inicial = atoi(puntero_archivo);
    uint32_t posicion_actual = bloque_inicial * BLOCK_SIZE + offset_inicial;
    size_t len_dato = strlen(datoRecibido);

    while (len_dato > 0)
    {
        uint32_t bloque_actual = posicion_actual / BLOCK_SIZE;
        uint32_t offset_en_bloque = posicion_actual % BLOCK_SIZE;
        uint32_t espacio_disponible = BLOCK_SIZE - offset_en_bloque;

        size_t a_copiar = (len_dato < espacio_disponible) ? len_dato : espacio_disponible;

        memcpy(bloques + posicion_actual, datoRecibido, a_copiar);

        posicion_actual += a_copiar;
        datoRecibido += a_copiar;
        len_dato -= a_copiar;
    }
}

char *leer_dato_archivo(uint32_t tamanio, char *puntero_archivo, char *bloques, uint32_t bloque_inicial)
{
    uint32_t offset_inicial = atoi(puntero_archivo);
    uint32_t posicion_inicial = (BLOCK_SIZE * bloque_inicial) + offset_inicial;
    char *dato_leido = (char *)malloc(tamanio + 1);

    memcpy(dato_leido, bloques + posicion_inicial, tamanio);

    dato_leido[tamanio] = '\0';

    return dato_leido;
}