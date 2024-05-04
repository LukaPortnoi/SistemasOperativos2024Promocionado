#include "../include/utils_memoria.h"

pthread_mutex_t mutex_procesos;
t_list *procesos_totales;
t_proceso_memoria *procesos_memoria;

// ESTA PARTE LA HIZO THOMI (NO LO DIGO DE MALO, SOLO COMENTO PARA SEPARAR LO QUE HICE)
// SE ESTARA USANDO ESTA FUNCION A PRIORI, SI HAY DRAMA, SE REEMPLAZARA
t_proceso_memoria *recibir_proceso_memoria(int socket_cliente)
{
    t_proceso_memoria *proceso = malloc(sizeof(t_proceso_memoria));
    int size;
    void *buffer = recibir_buffer(&size, socket_cliente);
    int offset = 0;

    memcpy(&(proceso->pid), buffer + offset, sizeof(int));
    offset += sizeof(int);

    proceso->path = malloc(size - offset);
    memcpy(proceso->path, buffer + offset, size - offset);
    free(buffer);
    return proceso;
}

void extraer_de_paquete(t_paquete *paquete, void *destino, int size) {}
/*{
    if (paquete->buffer->size < size)
    {
        // Manejar error
        log_error(logger, "No se puede extraer %d bytes de un paquete de %d bytes", size, paquete->buffer->size);
        return;
    }

    memcpy(destino, paquete->buffer->stream, size);

    paquete->buffer->size -= size;
    memmove(paquete->buffer->stream, paquete->buffer->stream + size, paquete->buffer->size);
}*/

// ENVIADO DE INSTRUCCIONES DE MEMORIA A CPU
void recibir_pedido_instruccion(uint32_t *pid, uint32_t *pc, int socket)
{
    int size;
    void *buffer = recibir_buffer(&size, socket);
    int offset = 0;
    memcpy(pid, buffer + offset, sizeof(int));
    offset += sizeof(int);
    memcpy(pc, buffer + offset, sizeof(int));
    free(buffer);
}

t_proceso_memoria *obtener_proceso_pid(uint32_t pid_pedido)
{
    t_proceso_memoria *proceso;

    bool id_process(void *elemento)
    {
        return ((t_proceso_memoria *)elemento)->pid == pid_pedido;
    }

    pthread_mutex_lock(&mutex_procesos);
    proceso = list_find(procesos_totales, id_process);
    pthread_mutex_unlock(&mutex_procesos);
    return proceso;
}

t_instruccion *obtener_instruccion_del_proceso_pc(t_proceso_memoria *proceso, uint32_t pc)
{
    usleep(RETARDO_RESPUESTA * 1000); // 1000 * 1000 = 1 segundo
    return list_get(proceso->instrucciones, pc);
}

char *obtener_nombre_instruccion(nombre_instruccion instruccion)
{
    switch (instruccion)
    {
    case SET:
        return "SET";
    case SUM:
        return "SUM";
    case SUB:
        return "SUB";
    case JNZ:
        return "SET";
    case IO_GEN_SLEEP:
        return "IO_GEN_SLEEP";
    }
}

void enviar_instruccion(int socket, t_instruccion *instruccion)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(INSTRUCCION);
    serializar_instruccion(paquete, instruccion);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}

void serializar_instruccion(t_paquete *paquete, t_instruccion *instruccion)
{ // Revisar toda la funcion
    paquete->buffer->size = sizeof(nombre_instruccion) + 
                            sizeof(uint32_t) * 2 + 
                            instruccion->longitud_parametro1 + 
                            instruccion->longitud_parametro2;
                            
    paquete->buffer->stream = malloc(paquete->buffer->size);
    int desplazamiento = 0;

    memcpy(paquete->buffer->stream + desplazamiento, &(instruccion->nombre), sizeof(nombre_instruccion));
    desplazamiento += sizeof(nombre_instruccion);
    memcpy(paquete->buffer->stream + desplazamiento, &(instruccion->longitud_parametro1), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(paquete->buffer->stream + desplazamiento, &(instruccion->longitud_parametro2), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(paquete->buffer->stream + desplazamiento, instruccion->parametro1, instruccion->longitud_parametro1);
    desplazamiento += instruccion->longitud_parametro1;
    memcpy(paquete->buffer->stream + desplazamiento, instruccion->parametro2, instruccion->longitud_parametro2);
    desplazamiento += instruccion->longitud_parametro2;
}

// NO ME CIERRA LA FUNCION, SE REVISARA MAS TARDE (MAS QUE NADA AL DEVOLVER EL PROCESO_NUEVO, YA QUE SE INICIALIZA PREVIAMENTE Y NO SE DESARROLLO LA FUNCION POR EL TEMA DE LA ESTRUCTURA DE LOS PROCESOS EN MEMORIA)
t_proceso_memoria *iniciar_proceso_path(t_proceso_memoria *proceso_nuevo)
{
    proceso_nuevo->instrucciones = parsear_instrucciones(proceso_nuevo->path);
    log_info(LOGGER_MEMORIA, "Instrucciones bien parseadas para el proceso PID [%d]", proceso_nuevo->pid);
    pthread_mutex_lock(&mutex_procesos);
    list_add(procesos_totales, proceso_nuevo); // Se agrega el proceso a la lista de procesos totales
    pthread_mutex_unlock(&mutex_procesos);
    // inicializar_nuevo_proceso(proceso_nuevo); -> Se usa para inicializar las estructuras de memoria del proceso. Se agregara despues si es que no hay drama con la funcion actual. Si genera problema, se eliminara y buscara otra alternativa
    return proceso_nuevo;
}

// LEE EL ARCHIVO E INGRESA CADA INSTRUCCION EN LA LISTA DE INSTRUCCIONES DEL PROCESO
t_list *parsear_instrucciones(char *path)
{
    t_list *instrucciones = list_create();
    char *path_archivo = string_new();
    string_append(&path_archivo, "./cfg/");
    string_append(&path_archivo, path);
    char *codigo_leido = leer_archivo(path_archivo);
    char **split_instrucciones = string_split(codigo_leido, "\n");
    int indice_split = 0;
    while (split_instrucciones[indice_split] != NULL)
    {
        char **palabras = string_split(split_instrucciones[indice_split], " ");
        if (string_equals_ignore_case(palabras[0], "SET"))
        {
            list_add(instrucciones, armar_estructura_instruccion(SET, palabras[1], palabras[2]));
        }
        else if (string_equals_ignore_case(palabras[0], "SUM"))
        {
            list_add(instrucciones, armar_estructura_instruccion(SUM, palabras[1], palabras[2]));
        }
        else if (string_equals_ignore_case(palabras[0], "SUB"))
        {
            list_add(instrucciones, armar_estructura_instruccion(SUB, palabras[1], palabras[2]));
        }
        else if (string_equals_ignore_case(palabras[0], "JNZ"))
        {
            list_add(instrucciones, armar_estructura_instruccion(JNZ, palabras[1], palabras[2]));
        }
        else if (string_equals_ignore_case(palabras[0], "IO_GEN_SLEEP"))
        {
            list_add(instrucciones, armar_estructura_instruccion(IO_GEN_SLEEP, palabras[1], palabras[2]));
        }
        indice_split++;
        string_iterates_lines(palabras, (void (*)(char *))free);
        free(palabras);
    }
    free(codigo_leido);
    string_iterates_lines(split_instrucciones, (void (*)(char *))free);
    free(split_instrucciones);
    free(path_archivo);
    return instrucciones;
}

t_instruccion *armar_estructura_instruccion(nombre_instruccion instruccion, char *parametro1, char *parametro2)
{
    t_instruccion *estructura = (t_instruccion *)malloc(sizeof(t_instruccion));
    estructura->nombre = instruccion;
    estructura->parametro1 = (parametro1[0] != '\0') ? strdup(parametro1) : parametro1;
    estructura->parametro2 = (parametro2[0] != '\0') ? strdup(parametro2) : parametro2;
    estructura->longitud_parametro1 = strlen(estructura->parametro1) + 1;
    estructura->longitud_parametro2 = strlen(estructura->parametro2) + 1;
    printf("%s - %s - %s \n", obtener_nombre_instruccion(estructura->nombre), estructura->parametro1, estructura->parametro2); // printea instrucciones
    return estructura;
}

char *leer_archivo(char *path)
{
    char instrucciones[100];
    strcpy(instrucciones, path);
    FILE *archivo = fopen(instrucciones, "r");
    if (archivo == NULL)
    {
        perror("Error al abrir el archivo");
    }
    fseek(archivo, 0, SEEK_END);
    int cant_elementos = ftell(archivo);
    rewind(archivo);
    char *cadena = calloc(cant_elementos + 1, sizeof(char));
    if (cadena == NULL)
    {
        perror("Error en la reserva de memoria \n");
        fclose(archivo);
        return NULL;
    }
    int cant_elementos_leidos = fread(cadena, sizeof(char), cant_elementos, archivo);
    if (cant_elementos != cant_elementos)
    {
        perror("Error leyendo el archivo \n");
        fclose(archivo);
        free(cadena);
        return NULL;
    }
    fclose(archivo);
    return cadena;
}