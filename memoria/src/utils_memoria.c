#include "../include/utils_memoria.h"

t_proceso_memoria *proceso_memoria;

t_proceso_memoria *recibir_proceso_memoria(int socket_cliente)
{
    t_paquete *paquete = recibir_paquete(socket_cliente);
    t_proceso_memoria *proceso = deserializar_proceso(paquete->buffer);
    eliminar_paquete(paquete);
    return proceso;
}

t_proceso_memoria *deserializar_proceso(t_buffer *buffer)
{
    t_proceso_memoria *proceso = malloc(sizeof(t_proceso_memoria));
    if (proceso == NULL)
    {
        return NULL;
    }

    uint32_t long_path;
    void *stream = buffer->stream;
    int desplazamiento = 0;

    memcpy(&(proceso->pid), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(&(long_path), stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    proceso->path = malloc(long_path);

    memcpy(proceso->path, stream + desplazamiento, long_path);

    return proceso;
}

// ENVIADO DE INSTRUCCIONES DE MEMORIA A CPU
void recibir_pedido_instruccion(uint32_t *pid, uint32_t *pc, int socket)
{
    t_paquete *paquete = recibir_paquete(socket);
    deserializar_pedido_instruccion(pid, pc, paquete->buffer);
    eliminar_paquete(paquete);
}

void deserializar_pedido_instruccion(uint32_t *pid, uint32_t *pc, t_buffer *buffer)
{
    int desplazamiento = 0;
    memcpy(pid, buffer->stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(pc, buffer->stream + desplazamiento, sizeof(uint32_t));
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

char *instruccion_to_string(nombre_instruccion instruccion)
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
        return "JNZ";
    case IO_GEN_SLEEP:
        return "IO_GEN_SLEEP";
    case MOV_IN:
        return "MOV_IN";
    case MOV_OUT:
        return "MOV_OUT";
    case RESIZE:
        return "RESIZE";
    case COPY_STRING:
        return "COPY_STRING";
    case WAIT:
        return "WAIT";
    case SIGNAL:
        return "SIGNAL";
    case IO_STDIN_READ:
        return "IO_STDIN_READ";
    case IO_STDOUT_WRITE:
        return "IO_STDOUT_WRITE";
    case IO_FS_CREATE:
        return "IO_FS_CREATE";
    case IO_FS_DELETE:
        return "IO_FS_DELETE";
    case IO_FS_TRUNCATE:
        return "IO_FS_TRUNCATE";
    case IO_FS_WRITE:
        return "IO_FS_WRITE";
    case IO_FS_READ:
        return "IO_FS_READ";
    case EXIT:
        return "EXIT";
    default:
        return "Instruccion no reconocida";
        break;
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
{
    paquete->buffer->size = sizeof(nombre_instruccion) +
                            sizeof(uint32_t) * 5 +
                            instruccion->longitud_parametro1 +
                            instruccion->longitud_parametro2 +
                            instruccion->longitud_parametro3 +
                            instruccion->longitud_parametro4 +
                            instruccion->longitud_parametro5;

    paquete->buffer->stream = malloc(paquete->buffer->size);

    int desplazamiento = 0;

    memcpy(paquete->buffer->stream + desplazamiento, &(instruccion->nombre), sizeof(nombre_instruccion));
    desplazamiento += sizeof(nombre_instruccion);

    uint32_t longitud_parametro1 = instruccion->longitud_parametro1;                          // Cambio aquí
    memcpy(paquete->buffer->stream + desplazamiento, &longitud_parametro1, sizeof(uint32_t)); // Cambio aquí
    desplazamiento += sizeof(uint32_t);

    uint32_t longitud_parametro2 = instruccion->longitud_parametro2;                          // Cambio aquí
    memcpy(paquete->buffer->stream + desplazamiento, &longitud_parametro2, sizeof(uint32_t)); // Cambio aquí
    desplazamiento += sizeof(uint32_t);

    uint32_t longitud_parametro3 = instruccion->longitud_parametro3;                          // Cambio aquí
    memcpy(paquete->buffer->stream + desplazamiento, &longitud_parametro3, sizeof(uint32_t)); // Cambio aquí
    desplazamiento += sizeof(uint32_t);

    uint32_t longitud_parametro4 = instruccion->longitud_parametro4;                          // Cambio aquí
    memcpy(paquete->buffer->stream + desplazamiento, &longitud_parametro4, sizeof(uint32_t)); // Cambio aquí
    desplazamiento += sizeof(uint32_t);

    uint32_t longitud_parametro5 = instruccion->longitud_parametro5;                          // Cambio aquí
    memcpy(paquete->buffer->stream + desplazamiento, &longitud_parametro5, sizeof(uint32_t)); // Cambio aquí
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, instruccion->parametro1, instruccion->longitud_parametro1);
    desplazamiento += instruccion->longitud_parametro1;

    memcpy(paquete->buffer->stream + desplazamiento, instruccion->parametro2, instruccion->longitud_parametro2);
    desplazamiento += instruccion->longitud_parametro2;

    memcpy(paquete->buffer->stream + desplazamiento, instruccion->parametro3, instruccion->longitud_parametro3);
    desplazamiento += instruccion->longitud_parametro3;

    memcpy(paquete->buffer->stream + desplazamiento, instruccion->parametro4, instruccion->longitud_parametro4);
    desplazamiento += instruccion->longitud_parametro4;

    memcpy(paquete->buffer->stream + desplazamiento, instruccion->parametro5, instruccion->longitud_parametro5);
}

t_proceso_memoria *iniciar_proceso_path(t_proceso_memoria *proceso_nuevo)
{
    pthread_mutex_lock(&mutex_procesos);
    proceso_nuevo->instrucciones = parsear_instrucciones(proceso_nuevo->path);
    log_debug(LOGGER_MEMORIA, "Instrucciones bien parseadas para el proceso PID [%d]", proceso_nuevo->pid);
    list_add(procesos_totales, proceso_nuevo);
    pthread_mutex_unlock(&mutex_procesos);
    iniciar_estructura_proceso_memoria(proceso_nuevo);
    return proceso_nuevo;
}

// LEE EL ARCHIVO E INGRESA CADA INSTRUCCION EN LA LISTA DE INSTRUCCIONES DEL PROCESO
t_list *parsear_instrucciones(char *path)
{
    t_list *instrucciones = list_create();
    char *path_archivo = string_new();
    string_append(&path_archivo, PATH_INSTRUCCIONES);
    string_append(&path_archivo, path);
    char *codigo_leido = leer_archivo(path_archivo);
    char **split_instrucciones = string_split(codigo_leido, "\n");
    int indice_split = 0;
    while (split_instrucciones[indice_split] != NULL)
    {
        char **palabras = string_split(split_instrucciones[indice_split], " ");
        if (string_equals_ignore_case(palabras[0], "SET"))
        {
            list_add(instrucciones, armar_estructura_instruccion(SET, palabras[1], palabras[2], "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "SUM"))
        {
            list_add(instrucciones, armar_estructura_instruccion(SUM, palabras[1], palabras[2], "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "SUB"))
        {
            list_add(instrucciones, armar_estructura_instruccion(SUB, palabras[1], palabras[2], "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "JNZ"))
        {
            list_add(instrucciones, armar_estructura_instruccion(JNZ, palabras[1], palabras[2], "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "WAIT"))
        {
            list_add(instrucciones, armar_estructura_instruccion(WAIT, palabras[1], "", "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "SIGNAL"))
        {
            list_add(instrucciones, armar_estructura_instruccion(SIGNAL, palabras[1], "", "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "RESIZE"))
        {
            list_add(instrucciones, armar_estructura_instruccion(RESIZE, palabras[1], "", "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "MOV_IN"))
        {
            list_add(instrucciones, armar_estructura_instruccion(MOV_IN, palabras[1], palabras[2], "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "MOV_OUT"))
        {
            list_add(instrucciones, armar_estructura_instruccion(MOV_OUT, palabras[1], palabras[2], "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "COPY_STRING"))
        {
            list_add(instrucciones, armar_estructura_instruccion(COPY_STRING, palabras[1], "", "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "IO_GEN_SLEEP"))
        {
            list_add(instrucciones, armar_estructura_instruccion(IO_GEN_SLEEP, palabras[1], palabras[2], "", "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "IO_STDIN_READ"))
        {
            list_add(instrucciones, armar_estructura_instruccion(IO_STDIN_READ, palabras[1], palabras[2], palabras[3], "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "IO_STDOUT_WRITE"))
        {
            list_add(instrucciones, armar_estructura_instruccion(IO_STDOUT_WRITE, palabras[1], palabras[2], palabras[3], "", ""));
        }
        else if (string_equals_ignore_case(palabras[0], "EXIT"))
        {
            list_add(instrucciones, armar_estructura_instruccion(EXIT, "", "", "", "", ""));
        }

        indice_split++;
        string_iterate_lines(palabras, (void (*)(char *))free);
        free(palabras);
    }
    free(codigo_leido);
    string_iterate_lines(split_instrucciones, (void (*)(char *))free);
    free(split_instrucciones);
    free(path_archivo);
    return instrucciones;
}

t_instruccion *armar_estructura_instruccion(nombre_instruccion instruccion, char *parametro1, char *parametro2, char *parametro3, char *parametro4, char *parametro5)
{
    t_instruccion *estructura = (t_instruccion *)malloc(sizeof(t_instruccion));

    estructura->nombre = instruccion;
    estructura->parametro1 = (parametro1[0] != '\0') ? strdup(parametro1) : parametro1; //(parametro1 && parametro1[0] != '\0') ? strdup(parametro1) : NULL;
    estructura->parametro2 = (parametro2[0] != '\0') ? strdup(parametro2) : parametro2; //(parametro2 && parametro2[0] != '\0') ? strdup(parametro2) : NULL;

    estructura->longitud_parametro1 = strlen(estructura->parametro1) + 1;
    estructura->longitud_parametro2 = strlen(estructura->parametro2) + 1;

    estructura->parametro3 = (parametro3[0] != '\0') ? strdup(parametro3) : parametro3;
    estructura->longitud_parametro3 = strlen(estructura->parametro3) + 1;

    estructura->parametro4 = (parametro4[0] != '\0') ? strdup(parametro4) : parametro4;
    estructura->longitud_parametro4 = strlen(estructura->parametro4) + 1;

    estructura->parametro5 = (parametro5[0] != '\0') ? strdup(parametro5) : parametro5;
    estructura->longitud_parametro5 = strlen(estructura->parametro5) + 1;

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
    if (cant_elementos != cant_elementos_leidos)
    {
        perror("Error leyendo el archivo \n");
        fclose(archivo);
        free(cadena);
        return NULL;
    }
    fclose(archivo);
    return cadena;
}

void recibir_pedido_resize(uint32_t *pid, uint32_t *nueva_cantidad_paginas, int socket)
{
    t_paquete *paquete = recibir_paquete(socket);
    deserializar_pedido_resize(pid, nueva_cantidad_paginas, paquete->buffer);
    eliminar_paquete(paquete);
}

void deserializar_pedido_resize(uint32_t *pid, uint32_t *tamanio, t_buffer *buffer)
{
    int desplazamiento = 0;
    memcpy(pid, buffer->stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(tamanio, buffer->stream + desplazamiento, sizeof(uint32_t));
}

void iniciar_semaforos()
{
    pthread_mutex_init(&mutex_procesos, NULL);
    pthread_mutex_init(&mutex_comunicacion_procesos, NULL);
}

void recibir_mov_out_cpu(t_list *lista_direcciones, uint32_t *valorObtenido, int cliente_socket)
{
    t_paquete *paquete = recibir_paquete(cliente_socket);
    deserializar_datos_mov_out(paquete, lista_direcciones, valorObtenido);
    // printf("Valor dir_fisica recv: %ls \n", direccion_fisica);
    // printf("Valor tamanio_registro recv: %ls \n", tamanio_registro);
    // printf("Valor valorObtenido recv: %ls \n", valorObtenido);
    eliminar_paquete(paquete);
}

void deserializar_datos_mov_out(t_paquete *paquete, t_list *lista_datos, uint32_t *valorObtenido)
{
    int desplazamiento = 0;
    size_t num_elementos = (paquete->buffer->size - sizeof(uint32_t)) / (2 * sizeof(uint32_t));

    // Iteramos sobre el buffer y deserializamos cada elemento
    for (size_t i = 0; i < num_elementos; i++)
    {
        t_direcciones_fisicas *dato = malloc(sizeof(t_direcciones_fisicas));
        memcpy(&(dato->direccion_fisica), paquete->buffer->stream + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
        memcpy(&(dato->tamanio), paquete->buffer->stream + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        list_add(lista_datos, dato);
    }

    memcpy(valorObtenido, paquete->buffer->stream + desplazamiento, sizeof(uint32_t));
}

void recibir_mov_in_cpu(int socket_cliente, t_list *lista_direcciones)
{
    t_paquete *paquete = recibir_paquete(socket_cliente);
    deserializar_datos_mov_in(paquete, lista_direcciones);
    eliminar_paquete(paquete);
}

void deserializar_datos_mov_in(t_paquete *paquete, t_list *lista_datos)
{
    int desplazamiento = 0;
    size_t num_elementos = paquete->buffer->size / (2 * sizeof(uint32_t));

    // Iteramos sobre el buffer y deserializamos cada elemento
    for (size_t i = 0; i < num_elementos; i++)
    {
        t_direcciones_fisicas *dato = malloc(sizeof(t_direcciones_fisicas));
        memcpy(&(dato->direccion_fisica), paquete->buffer->stream + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
        memcpy(&(dato->tamanio), paquete->buffer->stream + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        list_add(lista_datos, dato);
    }
}

void recibir_pedido_marco(uint32_t *pagina, uint32_t *pid_proceso, int socket)
{
    t_paquete *paquete = recibir_paquete(socket);
    deserializar_pedido_marco(pagina, pid_proceso, paquete->buffer);
    eliminar_paquete(paquete);
}

void deserializar_pedido_marco(uint32_t *pagina, uint32_t *pid_proceso, t_buffer *buffer)
{
    int desplazamiento = 0;
    memcpy(pid_proceso, buffer->stream + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    memcpy(pagina, buffer->stream + desplazamiento, sizeof(uint32_t));
}

void enviar_marco(int socket, uint32_t marco)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(ENVIAR_MARCO); // ??????
    serializar_marco(paquete, marco);
    enviar_paquete(paquete, socket);
    eliminar_paquete(paquete);
}

void serializar_marco(t_paquete *paquete, uint32_t marco)
{
    paquete->buffer->size = sizeof(uint32_t);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    memcpy(paquete->buffer->stream, &(marco), sizeof(uint32_t));
}

void enviar_valor_mov_in_memoria(char *valor, int socket)
{
    t_paquete *paquete_mov_in = crear_paquete_con_codigo_de_operacion(PEDIDO_MOV_IN);
    serializar_valor_leido_mov_in(paquete_mov_in, valor);
    enviar_paquete(paquete_mov_in, socket);
    eliminar_paquete(paquete_mov_in);
}

void serializar_valor_leido_mov_in(t_paquete *paquete, char *valor)
{
    uint32_t tamanio_valor = strlen(valor) + 1;
    paquete->buffer->size = sizeof(uint32_t);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    memcpy(paquete->buffer->stream, &tamanio_valor, sizeof(uint32_t));
}

void recibir_copystring(int socket_cliente, t_list *Lista_direccionesFisica_escritura, t_list *Lista_direccionesFisica_lectura, uint32_t *tamanio)
{
    t_paquete *paquete = recibir_paquete(socket_cliente);
    deserializar_datos_copystring(paquete, Lista_direccionesFisica_escritura, Lista_direccionesFisica_lectura, tamanio);
    eliminar_paquete(paquete);
}

void deserializar_datos_copystring(t_paquete *paquete, t_list *Lista_direccionesFisica_escritura, t_list *Lista_direccionesFisica_lectura, uint32_t *tamanio)
{
    int desplazamiento = 0;
    size_t num_elementos_escritura = (paquete->buffer->size - sizeof(uint32_t)) / (4 * sizeof(uint32_t));

    // Iteramos sobre el buffer y deserializamos cada elemento
    for (size_t i = 0; i < num_elementos_escritura; i++)
    {
        // Asignamos memoria para un t_direcciones_fisicas, que contiene dos uint32_t
        t_direcciones_fisicas *dato = malloc(sizeof(t_direcciones_fisicas));
        memcpy(&(dato->direccion_fisica), paquete->buffer->stream + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
        memcpy(&(dato->tamanio), paquete->buffer->stream + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        list_add(Lista_direccionesFisica_escritura, dato);
    }

    size_t num_elementos_lectura = (paquete->buffer->size - sizeof(uint32_t) - desplazamiento) / (2 * sizeof(uint32_t));

    for (size_t i = 0; i < num_elementos_lectura; i++)
    {
        // Asignamos memoria para un t_direcciones_fisicas, que contiene dos uint32_t
        t_direcciones_fisicas *dato = malloc(sizeof(t_direcciones_fisicas));
        memcpy(&(dato->direccion_fisica), paquete->buffer->stream + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);
        memcpy(&(dato->tamanio), paquete->buffer->stream + desplazamiento, sizeof(uint32_t));
        desplazamiento += sizeof(uint32_t);

        list_add(Lista_direccionesFisica_lectura, dato);
    }

    if (desplazamiento < paquete->buffer->size)
    {
        *tamanio = *(uint32_t *)(paquete->buffer->stream + desplazamiento);
    }
}

void escribir_memoria(uint32_t dir_fisica, uint32_t tamanio_registro, char *valorObtenido) //Revisar char* en valorObtenido
{
    pthread_mutex_lock(&mutex_memoria_usuario);
    /* for (uint32_t i = 0; i < tamanio_registro; i++)
    {
        if (valorObtenido[indiceAux])
        {
            printf("Cosa escrita en memoria: %c\n", valorObtenido[indiceAux]);
            memcpy(memoriaUsuario + dir_fisica + i, &valorObtenido[indiceAux], 1);
                    indiceAux++;

        }
    } */

    printf("Cosa a escribir en memoria: %s", valorObtenido);
    memcpy(memoriaUsuario + dir_fisica, valorObtenido, tamanio_registro);
    pthread_mutex_unlock(&mutex_memoria_usuario);
    log_info(LOGGER_MEMORIA, "PID: <%d> - Accion: <ESCRIBIR> - Direccion Fisica: <%d> - Tamaño <%d> \n", proceso_memoria->pid, dir_fisica, tamanio_registro);
    usleep(RETARDO_RESPUESTA * 1000);
}

void escribir_memoria_mov_out(uint32_t dir_fisica, uint32_t tamanio_registro,  char *valorObtenido)
{

    


    pthread_mutex_lock(&mutex_memoria_usuario);

    unsigned char valorConvertido = (unsigned char)atoi(valorObtenido);
    
    printf("Cosa a escribir en memoria: %u\n", valorConvertido);
    *((unsigned char*)memoriaUsuario + dir_fisica) = valorConvertido;

    // Verificar que el valor se escribió correctamente
    unsigned char valorLeido = *((unsigned char*)memoriaUsuario + dir_fisica);
    if (valorLeido == valorConvertido) {
        printf("Valor escrito correctamente en la posición %u: %u\n", dir_fisica, valorLeido);
    } else {
        printf("Error al escribir el valor en la posición %u. Valor esperado: %u, valor leído: %u\n", dir_fisica, valorConvertido, valorLeido);
    }

    pthread_mutex_unlock(&mutex_memoria_usuario);
    log_info(LOGGER_MEMORIA, "PID: <%d> - Accion: <ESCRIBIR> - Direccion Fisica: <%d> - Tamaño <%d> \n", 0, dir_fisica, tamanio_registro);
    usleep(RETARDO_RESPUESTA * 1000);
}

char *leer_memoria(uint32_t dir_fisica, uint32_t tamanio_registro)
{
    pthread_mutex_lock(&mutex_memoria_usuario);
    
    // Asignar memoria para un solo byte que contendrá el dato leído
    char *dato_leido = malloc(4); // Asignamos espacio para un solo byte
    if (dato_leido == NULL) {
        perror("Error al asignar memoria para dato_leido");
        pthread_mutex_unlock(&mutex_memoria_usuario);
        return NULL;
    }
    
    // Leer el dato de la memoria
    unsigned char valorLeido = *((unsigned char*)memoriaUsuario + dir_fisica);
    sprintf(dato_leido, "%u", valorLeido);
    // Registrar la acción en el log
    log_info("LOGGER_MEMORIA", "PID: <%d> - Accion: <LEER> - Direccion Fisica: <%d> - Tamaño <%d> \n", 0, dir_fisica, tamanio_registro);
    
    // Mostrar el dato leído en la consola
    printf("Dato leido: %s\n", dato_leido);
    
    pthread_mutex_unlock(&mutex_memoria_usuario);
    usleep(RETARDO_RESPUESTA * 1000);
    
    return dato_leido;
}

char *leer_memoria_IO(uint32_t dir_fisica, uint32_t tamanio_registro)
{
    /* char valor_leido; // Cambiar a int
    int valorTotalaDeLeer = 0; */
    pthread_mutex_lock(&mutex_memoria_usuario);
    char *cadena = malloc(tamanio_registro + 1); // Allocate memory dynamically
    memset(cadena, 0, tamanio_registro + 1);     // Initialize all elements to 0
    /* for (uint32_t i = 0; i < tamanio_registro; i++)
    {
        if (&memoriaUsuario[dir_fisica + i] != 0)
        {
            memcpy(&valor_leido, &memoriaUsuario[dir_fisica + i], 1);
            cadena[i] = valor_leido;
            printf("Valor a agregar a cadena: %c \n", valor_leido);
            valorTotalaDeLeer++;
            // Assign the character directly
        }
    } */
    memcpy(cadena, memoriaUsuario + dir_fisica, tamanio_registro);
    cadena[tamanio_registro] = '\0';
    pthread_mutex_unlock(&mutex_memoria_usuario);
    log_info(LOGGER_MEMORIA, "PID: <%d> - Accion: <LEER> - Direccion Fisica: <%d> - Tamaño <%d> \n", proceso_memoria->pid, dir_fisica, tamanio_registro);
    printf("Cadena leida: %s \n", cadena);
    usleep(RETARDO_RESPUESTA * 1000);
    return cadena;
}

char *int_to_char(int num)
{
    if (num == 0) // Añade este caso especial para cuando num es 0
    {
        char *s = (char *)calloc(2, sizeof(char)); // Asigna espacio para '0' y '\0'
        s[0] = '0';
        s[1] = '\0';
        return s;
    }

    int i = log10(num) + 1;
    char *s = (char *)calloc(i + 1, sizeof(char)); // Añadir espacio para el carácter nulo

    for (i--; num != 0; i--)
    {
        s[i] = (num % 10) + '0';
        num /= 10;
    }

    return s;

    /*if (num == 0)
    {
        char *s = (char *)calloc(2, sizeof(char)); // Si num es 0, devolver "0"
        if (s == NULL)
        {
            fprintf(stderr, "Error al asignar memoria en int_to_char\n");
            exit(EXIT_FAILURE); // En caso de fallo de asignación de memoria
        }
        s[0] = '0';
        s[1] = '\0'; // Terminador de cadena
        return s;
    }

    // Calcular el tamaño necesario para la cadena
    int i = log10(abs(num)) + 1;
    char *s = (char *)calloc(i + 1, sizeof(char)); // +1 para el terminador nulo

    if (s == NULL)
    {
        fprintf(stderr, "Error al asignar memoria en int_to_char\n");
        exit(EXIT_FAILURE); // En caso de fallo de asignación de memoria
    }

    int negativo = 0; // Variable para manejar números negativos
    if (num < 0)
    {
        negativo = 1;
        num = -num;
    }

    // Llenar el arreglo con los digitos de num
    for (i--; num != 0; i--)
    {
        s[i] = (num % 10) + '0';
        num /= 10;
    }

    if (negativo)
    {
        s[i] = '-';
    }

    return s;
    */
}

char *concatenar_lista_de_cadenas(t_list *lista , int tamanio)
{
    // Calcular el tamaño total necesario
    size_t tam_total = tamanio + 1; // Inicia en 1 para el carácter nulo

    // Asignar memoria para la cadena concatenada
    char *cadena_concatenada = (char *)calloc(tam_total, sizeof(char));
    if (cadena_concatenada == NULL)
    {
        return NULL; // Manejo de error si la asignación de memoria falla
    }

    // Inicializar la cadena concatenada como una cadena vacía
    cadena_concatenada[0] = '\0';

    // Copiar cada cadena de la lista en la cadena concatenada
    for (int i = 0; i < list_size(lista); i++)
    {
        strncat(cadena_concatenada, list_get(lista, i), tam_total - strlen(cadena_concatenada) - 1);
    }

    // Asegurarse de que la cadena esté terminada con un carácter nulo
    cadena_concatenada[tam_total - 1] = '\0';

    return cadena_concatenada;
}

char* decimal_a_binario(int numero) 
{
    // Array temporal para almacenar los dígitos binarios (máximo 32 bits para un entero)
    char temp[33];
    int index = 0;

    // Manejo del caso cuando el número es 0
    if (numero == 0) {
        temp[index++] = '0';
    } else {
        // Convertir el número decimal a binario
        while (numero > 0) {
            temp[index++] = (numero % 2) + '0';
            numero = numero / 2;
        }
    }

    // Añadir el carácter nulo al final de la cadena
    temp[index] = '\0';

    // Asignar memoria para la cadena final
    char* binaryNum = (char*)malloc((index + 1) * sizeof(char*));

    // Copiar los elementos en orden inverso al array final
    for (int i = 0; i < index; i++) {
        binaryNum[i] = temp[index - 1 - i];
    }

    // Añadir el carácter nulo al final de la cadena
    binaryNum[index] = '\0';

    return binaryNum;
}


int binario_a_decimal(int binario) 
{
    int decimal = 0;
    int posicion = 0;

    while (binario > 0) {
        int digito = binario % 10; // Obtiene el último dígito del número binario
        if (digito == 1) {
            decimal += pow(2, posicion);
        }
        binario /= 10; // Elimina el último dígito del número binario
        posicion++;
    }

    return decimal;
}