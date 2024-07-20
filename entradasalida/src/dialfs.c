#include "../include/dialfs.h"
#include <commons/memory.h>

t_archivo *crear_archivo(char *nombre, uint32_t bloque_inicial, uint32_t tamanio)
{
    t_archivo *archivo = malloc(sizeof(t_archivo));
    archivo->nombre = strdup(nombre);
    archivo->bloque_inicial = bloque_inicial;
    archivo->tamanio = tamanio;
    return archivo;
}

void destruir_archivo(t_archivo *archivo)
{
    free(archivo->nombre);
    free(archivo);
}

void eliminar_archivo_por_nombre(char *nombre)
{
    bool _es_archivo_por_nombre(t_archivo * archivo)
    {
        return strcmp(archivo->nombre, nombre) == 0;
    }

    t_archivo *archivo = list_remove_by_condition(ARCHIVOS_EN_FS, (void *)_es_archivo_por_nombre);
    if (archivo != NULL)
    {
        destruir_archivo(archivo);
    }
}

void actualizar_archivo_en_lista(char *nombre, uint32_t bloque_inicial, uint32_t tamanio)
{
    bool _es_archivo_por_nombre(t_archivo * archivo)
    {
        return strcmp(archivo->nombre, nombre) == 0;
    }

    t_archivo *archivo = list_find(ARCHIVOS_EN_FS, (void *)_es_archivo_por_nombre);

    if (archivo != NULL)
    {
        archivo->bloque_inicial = bloque_inicial;
        archivo->tamanio = tamanio;
    }
}

// Fijarse si existen los archivos bloques.dat y bitmap.dat en el PATH_BASE_DIALFS. Si no existen, crearlos.
void manejar_archivos_fs()
{
    crear_directorio_si_no_existe(PATH_BASE_DIALFS);

    FILE *bloques_file = fopen(BLOQUES_PATH, "rb+");
    if (bloques_file == NULL)
    {
        // El archivo no existe, se crea
        bloques_file = fopen(BLOQUES_PATH, "wb+");
        if (bloques_file == NULL)
        {
            log_error(LOGGER_INPUT_OUTPUT, "Error al crear bloques.dat. No existe la carpeta %s", PATH_BASE_DIALFS);
            exit(EXIT_FAILURE);
        }

        if (ftruncate(fileno(bloques_file), BLOCK_SIZE * BLOCK_COUNT) == -1)
        {
            log_error(LOGGER_INPUT_OUTPUT, "Error al establecer el tamaño de bloques.dat");
            fclose(bloques_file);
            exit(EXIT_FAILURE);
        }
    }
    fclose(bloques_file);
    log_trace(LOGGER_INPUT_OUTPUT, "Archivo bloques.dat verificado/creado con éxito");

    FILE *bitmap_file = fopen(BITMAP_PATH, "rb+");
    if (bitmap_file == NULL)
    {
        // El archivo no existe, se crea
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

void crear_directorio_si_no_existe(const char *path)
{
    struct stat st = {0};
    if (stat(path, &st) == -1)
    {
        if (mkdir(path, 0700) != 0)
        {
            log_error(LOGGER_INPUT_OUTPUT, "Error al crear el directorio %s", path);
            exit(EXIT_FAILURE);
        }
    }
    log_trace(LOGGER_INPUT_OUTPUT, "Directorio %s verificado/creado con éxito", path);
}

void actualizar_lista_archivos_en_fs()
{
    DIR *dir = opendir(PATH_BASE_DIALFS);
    if (dir == NULL)
    {
        log_error(LOGGER_INPUT_OUTPUT, "Error al abrir el directorio %s", PATH_BASE_DIALFS);
        exit(EXIT_FAILURE);
    }

    list_clean_and_destroy_elements(ARCHIVOS_EN_FS, (void (*)(void *))destruir_archivo);

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG)
        {
            if (strcmp(entry->d_name, "bloques.dat") == 0 || strcmp(entry->d_name, "bitmap.dat") == 0)
            {
                continue;
            }

            char *nombre_archivo = strdup(entry->d_name);
            char metadata_path[256];
            obtener_metadata_path(nombre_archivo, metadata_path, sizeof(metadata_path));

            if (access(metadata_path, F_OK) != -1)
            {
                t_archivo *archivo_en_fs = crear_archivo(nombre_archivo, obtener_bloque_inicial(metadata_path), obtener_tamanio_archivo(metadata_path));
                list_add(ARCHIVOS_EN_FS, archivo_en_fs);
            }
        }
    }

    closedir(dir);
}

void actualizar_bloque_inicial(t_config *metadata_config, uint32_t bloque_inicial)
{
    char *bloque_inicial_str = string_itoa(bloque_inicial);
    log_trace(LOGGER_INPUT_OUTPUT, "Actualizando bloque inicial a %s", bloque_inicial_str);
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

uint32_t obtener_bloques_ocupados(char path[])
{
    t_config *metadata_config = iniciar_config(path, "METADATA");
    uint32_t tamanio = config_get_int_value(metadata_config, "TAMANIO");
    config_destroy(metadata_config);
    int bloques_ocupados = (tamanio == 0) ? 1 : (tamanio + BLOCK_SIZE - 1) / BLOCK_SIZE; 
    return bloques_ocupados;
}

void obtener_metadata_path(char *nombre_archivo, char *metadata_path, size_t size)
{
    snprintf(metadata_path, size, "%s/%s", PATH_BASE_DIALFS, nombre_archivo);
}

int encontrar_bloque_libre()
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

int encontrar_bloques_libres_contiguos(uint32_t bloque_inicial, uint32_t bloques_necesarios, char *bitmap)
{
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
                return i - bloques_necesarios + 1;
            }
        }
        else
        {
            contador = 0;
        }
    }

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

    FILE *bitmap_file = fopen(BITMAP_PATH, "r+");
    FILE *bloques_file = fopen(BLOQUES_PATH, "r+");

    size_t bitmap_size = BLOCK_COUNT / 8;
    char *bitmap = mmap(NULL, bitmap_size, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(bitmap_file), 0);
    char *bloques = mmap(NULL, BLOCK_SIZE * BLOCK_COUNT, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(bloques_file), 0);

    // Primero quiero saber el tamaño total que ocupan los archivos en bloques, para esto tengo que recorrer la lista de archivos y sumar sus tamaños
    uint32_t tamanio_total_archivos = 0;
    for (int i = 0; i < list_size(ARCHIVOS_EN_FS); i++)
    {
        t_archivo *archivo = list_get(ARCHIVOS_EN_FS, i);
        tamanio_total_archivos += archivo->tamanio;
    }

    // Ahora voy a ordenar la lista de archivos por bloque_inicial

    ordenar_lista_archivos_por_bloque_inicial();
    
    // Luego voy a actualizar la lista de archivos en fs con los nuevos bloques iniciales, el primero tendra el bloque 0, el segundo tendra tamanio del primero, y asi sucesivamente todo esto dividido por el tamanio de un bloque
    // Al mismo tiempo que voy obteniendo cada archivo y actualizandole su bloque inicial en la lista de archivos en fs, voy a buscar por su nombre a
    // char metadata_path[256] obtener_metadata_path(interfazRecibida->nombre_archivo, metadata_path, sizeof(metadata_path)); su metadata y actualizarle el bloque inicial con actualizar_bloque_inicial(metadata_config, bloque_inicial);
    actualizar_lista_archivos_compactados();

    // Ya puedo hacer la compactacion con la lista de archivos ordenada, obteniendo de a uno su bloque inicial y tamanio y moviendolo a la posicion 0 de memoria que se va a ir actualizando con cada archivo
    int espacio_a_mover = 0;
    for (int i = 0; i < list_size(ARCHIVOS_EN_FS); i++)
    {
        t_archivo *archivo = list_get(ARCHIVOS_EN_FS, i);
        int offset_inicial = archivo->tamanio;

        if (offset_inicial % BLOCK_SIZE != 0)
        {
            offset_inicial = ((offset_inicial / BLOCK_SIZE) + 1) * BLOCK_SIZE;
        }

        memmove(bloques + espacio_a_mover, bloques + archivo->bloque_inicial * BLOCK_SIZE, offset_inicial); // Tambien podria usar memcpy
        espacio_a_mover += offset_inicial;
    }
    msync(bloques, BLOCK_SIZE * BLOCK_COUNT, MS_SYNC);

    // Al finalizar la compactacion, tengo que actualizar el bitmap (ahora tendra todos 1 en tamanio_total_archivos/bloque_size bits)

    log_trace(LOGGER_INPUT_OUTPUT, "Limpiando bitmap");
    // Primero limpio el bitmap
    for (int i = 0; i < bitmap_size; i++)
    {
        bitmap[i] = 0;
    }

    // guardo los datos de mi bitmap mapeado a memoria en el archivo bitmap.dat
    msync(bitmap, bitmap_size, MS_SYNC);
    imprimir_bitmap();
    log_trace(LOGGER_INPUT_OUTPUT, "Bitmap limpiado");

    // Luego voy a marcar los bloques ocupados, estos van a ser tamanio_total_archivos / BLOCK_SIZE porque van a estar todos juntos
    log_trace(LOGGER_INPUT_OUTPUT, "Marcando bloques ocupados esta cantidad de veces %d:", (tamanio_total_archivos + BLOCK_SIZE - 1) / BLOCK_SIZE);
    for (int i = 0; i < (tamanio_total_archivos + BLOCK_SIZE - 1) / BLOCK_SIZE; i++)
    {
        int byte_index = i / 8;
        int bit_index = i % 8;
        bitmap[byte_index] |= (1 << bit_index);
    }

    // guardo los datos de mi bitmap mapeado a memoria en el archivo bitmap.dat
    msync(bitmap, bitmap_size, MS_SYNC);
    imprimir_bitmap();

    //mostrar lista de archivos en fs
    for (int i = 0; i < list_size(ARCHIVOS_EN_FS); i++)
    {
        t_archivo *archivo = list_get(ARCHIVOS_EN_FS, i);
        log_trace(LOGGER_INPUT_OUTPUT, "Archivo: %s - Bloque Inicial: %d - Tamanio: %d", archivo->nombre, archivo->bloque_inicial, archivo->tamanio);
    }

    munmap(bitmap, bitmap_size);
    munmap(bloques, BLOCK_SIZE * BLOCK_COUNT);
    fclose(bitmap_file);
    fclose(bloques_file);

    usleep(RETRASO_COMPACTACION * 1000);
    log_info(LOGGER_INPUT_OUTPUT, "PID: %d - Fin Compactación.", pid);
}

void ordenar_lista_archivos_por_bloque_inicial()
{
    bool _ordenar_por_bloque_inicial(t_archivo * archivo1, t_archivo * archivo2)
    {
        return archivo1->bloque_inicial < archivo2->bloque_inicial;
    }

    list_sort(ARCHIVOS_EN_FS, (void *)_ordenar_por_bloque_inicial);
}

void actualizar_lista_archivos_compactados()
{
    int espacio_a_mover = 0;
    for (int i = 0; i < list_size(ARCHIVOS_EN_FS); i++)
    {
        t_archivo *archivo = list_get(ARCHIVOS_EN_FS, i);

        if (espacio_a_mover % BLOCK_SIZE != 0)
        {
            espacio_a_mover = ((espacio_a_mover / BLOCK_SIZE) + 1) * BLOCK_SIZE;
        }

        archivo->bloque_inicial = espacio_a_mover / BLOCK_SIZE;
        espacio_a_mover += archivo->tamanio;

        char metadata_path[256];
        obtener_metadata_path(archivo->nombre, metadata_path, sizeof(metadata_path));
        t_config *metadata_config = config_create(metadata_path);
        actualizar_bloque_inicial(metadata_config, archivo->bloque_inicial);
        config_save(metadata_config);
        config_destroy(metadata_config);
    }
}

void escribir_dato_archivo(char *datoRecibido, char *puntero_archivo, char *bloques, uint32_t bloque_inicial)
{
    uint32_t offset_inicial = atoi(puntero_archivo);
    uint32_t posicion_actual = bloque_inicial * BLOCK_SIZE + offset_inicial;
    size_t len_dato = strlen(datoRecibido);

    while (len_dato > 0)
    {
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

void imprimir_bitmap()
{
    FILE *bitmap_file = fopen(BITMAP_PATH, "r");
    size_t bitmap_size = BLOCK_COUNT / 8;
    char *bitmap = mmap(NULL, bitmap_size, PROT_READ, MAP_SHARED, fileno(bitmap_file), 0);

    // Crear el bitarray usando bitarray_create_with_mode
    t_bitarray *bitarray = bitarray_create_with_mode(bitmap, bitmap_size, LSB_FIRST);

    char datos_a_imprimir[BLOCK_COUNT + 500]; // Para el formato "n: b - " (+1 para '\0')
    int offset = 0;

    for (int i = 0; i < BLOCK_COUNT; i++)
    {
        offset += snprintf(datos_a_imprimir + offset, sizeof(datos_a_imprimir) - offset, "%d: %d - ", i, bitarray_test_bit(bitarray, i));
    }

    // Quitar el último " - " y añadir el carácter nulo
    if (offset >= 3)
    {
        datos_a_imprimir[offset - 3] = '\0';
    }

    log_trace(LOGGER_INPUT_OUTPUT, "%s", datos_a_imprimir);

    bitarray_destroy(bitarray);
    fclose(bitmap_file);
    munmap(bitmap, bitmap_size);
}

uint32_t encontrar_bloques_libres_contiguos_top(uint32_t bloque_inicial, uint32_t bloques_necesarios, uint32_t bloques_ocupados, char *bitmap)
{
    // hay 2 formas de encontrar bloques libres contiguos
    // 1. Hay bloques bloques_necesarios - bloques_ocupados libres contiguos inmediatamente despues de los bloques ocupados
    // 2. Hay bloques_necesarios contiguos en todo el bitmap

    // 1.
    uint32_t contador = 0;
    uint32_t bloque_inicial_aux = bloque_inicial + bloques_ocupados;
    // Ponele que mi bloque inicial es 0, tengo 2 bloques ocupados y necesito 3 bloques
    // Si ocupados tengo 2 y necesarios 3, tengo que buscar (3-2) = 1 bloque libre contiguo
    uint32_t bloques_libres_necesarios = bloques_necesarios - bloques_ocupados;

    for (uint32_t i = (bloque_inicial_aux + 1); i <= (bloque_inicial_aux + bloques_libres_necesarios); i++)
    {
        int byte_index = i / 8;
        int bit_index = i % 8;
        if (!(bitmap[byte_index] & (1 << bit_index)))
        {
            contador++;
            if (contador == bloques_libres_necesarios)
            {
                log_trace(LOGGER_INPUT_OUTPUT, "Bloques libres contiguos inmediatamente despues de los bloques ocupados: %d", bloque_inicial);
                return bloque_inicial;
            }
        }
        else
        {
            break;
        }
    }

    // 2.
    contador = 0;
    // primero me voy a armar un bitmap auxiliar
    char *bitmap_aux = malloc(BLOCK_COUNT / 8);
    memcpy(bitmap_aux, bitmap, (BLOCK_COUNT / 8));
    // ahora voy a marcar los bloques ocupados de mi archivo en el bitmap auxiliar como libres
    for (uint32_t i = bloque_inicial; i < (bloque_inicial + bloques_ocupados); i++)
    {
        int byte_index = i / 8;
        int bit_index = i % 8;
        bitmap_aux[byte_index] &= ~(1 << bit_index);
    }
    // ahora voy a buscar bloques libres contiguos en el bitmap auxiliar
    for (uint32_t i = 0; i < BLOCK_COUNT; i++)
    {
        int byte_index = i / 8;
        int bit_index = i % 8;
        if (!(bitmap_aux[byte_index] & (1 << bit_index)))
        {
            contador++;
            if (contador == bloques_necesarios)
            {
                free(bitmap_aux);
                log_trace(LOGGER_INPUT_OUTPUT, "Nueva posicion inicial: %d, ocupando hasta %d", i - bloques_necesarios + 1, i);
                return i - bloques_necesarios + 1;
            }
        }
        else
        {
            contador = 0;
        }
    }
    free(bitmap_aux);
    return -1;
}

//obtener bloque inicial en la lista de archivos en fs
uint32_t obtener_bloque_inicial_por_nombre(char *nombre)
{
    bool _es_archivo_por_nombre(t_archivo * archivo)
    {
        return strcmp(archivo->nombre, nombre) == 0;
    }

    t_archivo *archivo = list_find(ARCHIVOS_EN_FS, (void *)_es_archivo_por_nombre);

    if (archivo != NULL)
    {
        return archivo->bloque_inicial;
    }
    return -1;
}