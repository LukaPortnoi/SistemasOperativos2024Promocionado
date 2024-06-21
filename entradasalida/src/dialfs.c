#include "../include/dialfs.h"

// Fijarse si existen los archivos bloques.dat y bitmap.dat en el PATH_BASE_DIALFS. Si no existen, crearlos.
void manejar_archivos_fs()
{
    char path_bloques[256];
    char path_bitmap[256];

    snprintf(path_bloques, sizeof(path_bloques), "%s/bloques.dat", PATH_BASE_DIALFS);
    snprintf(path_bitmap, sizeof(path_bitmap), "%s/bitmap.dat", PATH_BASE_DIALFS);

    // Verificar y crear bloques.dat si no existe
    FILE *bloques_file = fopen(path_bloques, "rb+");
    if (bloques_file == NULL)
    {
        // El archivo no existe, crearlo
        bloques_file = fopen(path_bloques, "wb+");
        if (bloques_file == NULL)
        {
            log_error(LOGGER_INPUT_OUTPUT, "Error al crear bloques.dat");
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
    FILE *bitmap_file = fopen(path_bitmap, "rb+");
    if (bitmap_file == NULL)
    {
        // El archivo no existe, crearlo
        bitmap_file = fopen(path_bitmap, "wb+");
        if (bitmap_file == NULL)
        {
            log_error(LOGGER_INPUT_OUTPUT, "Error al crear bitmap.dat");
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