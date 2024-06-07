#include "../include/instrucciones.h"
#include <ctype.h>

// (Registro, Valor): Asigna al registro el valor pasado como parámetro.
void _set(char *registro, char *valor)
{
    if (revisar_registro(registro))
    {
        *(get_registry8(registro)) = str_to_uint8(valor);
    }
    else
    {
        *(get_registry32(registro)) = str_to_uint32(valor);
    }
}

// (Registro Datos, Registro Dirección): Lee el valor de memoria correspondiente a
// la Dirección Lógica que se encuentra en el Registro Dirección y lo almacena en el Registro Datos.
void _mov_in(char *registro, char *direc_logica)
{
    uint32_t direccionLogica32;

    if (revisar_registro(direc_logica))
    {
        direccionLogica32 = *(get_registry8(direc_logica));
    }
    else
    {
        direccionLogica32 = *(get_registry32(direc_logica));
    }

    uint32_t direccionFisica = traducir_direccion(pcb_actual->pid, direccionLogica32, TAM_PAGINA);
    // char *valorObtenido = obtener_valor_direccion_fisica(direccionFisica);

    /*if (revisar_registro(registro))
    {
        *(get_registry8(registro)) = get_registry8(valorObtenido);
    }
    else
    {
        *(get_registry8(registro)) = get_registry32(valorObtenido);
    }*/
}

//(Registro Dirección, Registro Datos): Lee el valor del Registro Datos y lo escribe en
// la dirección física de memoria obtenida a partir de la Dirección Lógica almacenada en el Registro Dirección.
void _mov_out(char *direc_logica, char *registro)
{

    uint32_t direccionLogica32;

    if (revisar_registro(direc_logica))
    {
        direccionLogica32 = *(get_registry8(direc_logica));
    }
    else
    {
        direccionLogica32 = *(get_registry32(direc_logica));
    }

    uint32_t direccionFisica = traducir_direccion(pcb_actual->pid, direccionLogica32, TAM_PAGINA);
    // char *valorObtenido = obtener_valor_direccion_fisica(direccionFisica);

    /*if (revisar_registro(registro))
    {
        *(get_registry8(valorObtenido)) = get_registry8(registro);

    }
    else
    {
        *(get_registry32(valorObtenido)) = get_registry32(registro);
    }*/
}

// (Registro Destino, Registro Origen): Suma al Registro Destino el
// Registro Origen y deja el resultado en el Registro Destino.
void _sum(char *registro_destino, char *registro_origen)
{
    uint8_t *destino8 = NULL;
    uint32_t *destino32 = NULL;
    uint8_t *origen8 = NULL;
    uint32_t *origen32 = NULL;
    int is_destino_8bit = revisar_registro(registro_destino);
    int is_origen_8bit = revisar_registro(registro_origen);

    if (is_destino_8bit)
    {
        destino8 = get_registry8(registro_destino);
    }
    else
    {
        destino32 = get_registry32(registro_destino);
    }

    if (is_origen_8bit)
    {
        origen8 = get_registry8(registro_origen);
    }
    else
    {
        origen32 = get_registry32(registro_origen);
    }

    if (is_destino_8bit && is_origen_8bit)
    {
        *destino8 = *destino8 + *origen8;
    }
    else if (!is_destino_8bit && !is_origen_8bit)
    {
        *destino32 = *destino32 + *origen32;
    }
    else
    {
        // Manejo de error si los registros son de tamaños diferentes
        printf("Error: registros de diferentes tamaños no pueden ser sumados.\n");
    }
}

// (Registro Destino, Registro Origen): Resta al Registro Destino
// el Registro Origen y deja el resultado en el Registro Destino
void _sub(char *registro_destino, char *registro_origen)
{
    uint8_t *destino8 = NULL;
    uint32_t *destino32 = NULL;
    uint8_t *origen8 = NULL;
    uint32_t *origen32 = NULL;
    int is_destino_8bit = revisar_registro(registro_destino);
    int is_origen_8bit = revisar_registro(registro_origen);

    if (is_destino_8bit)
    {
        destino8 = get_registry8(registro_destino);
    }
    else
    {
        destino32 = get_registry32(registro_destino);
    }

    if (is_origen_8bit)
    {
        origen8 = get_registry8(registro_origen);
    }
    else
    {
        origen32 = get_registry32(registro_origen);
    }

    if (is_destino_8bit && is_origen_8bit)
    {
        *destino8 = *destino8 - *origen8;
    }
    else if (!is_destino_8bit && !is_origen_8bit)
    {
        *destino32 = *destino32 - *origen32;
    }
    else
    {
        // Manejo de error si los registros son de tamaños diferentes
        printf("Error: registros de diferentes tamaños no pueden ser restados.\n");
    }
}

// (Registro, Instrucción): Si el valor del registro es distinto de cero,
// actualiza el program counter al número de instrucción pasada por parámetro.
void _jnz(char *registro, char *instruccion)
{
    uint8_t *regis8 = NULL;
    uint32_t *regis32 = NULL;
    int is_regis_8bit = revisar_registro(registro);

    if (is_regis_8bit)
    {
        regis8 = get_registry8(registro);
        if (*regis8 != 0)
        {
            pcb_actual->contexto_ejecucion->registros->program_counter = str_to_uint32(instruccion);
        }
        else
        {
            log_warning(LOGGER_CPU, "El registro %s es igual a cero, no se actualiza el IP", registro);
        }
    }
    else
    {
        regis32 = get_registry32(registro);
        if (*regis32 != 0)
        {
            pcb_actual->contexto_ejecucion->registros->program_counter = str_to_uint32(instruccion);
        }
        else
        {
            log_warning(LOGGER_CPU, "El registro %s es igual a cero, no se actualiza el IP", registro);
        }
    }
}

void _resize(char *tamanioAReasignar)
{
    uint32_t tamanioAReasignarNum = str_to_uint32(tamanioAReasignar);
    enviar_resize_a_memoria(pcb_actual, tamanioAReasignarNum);
}

void _copy_string(char *tamanio) {}

/*void _wait(char *recurso, int cliente_socket)
{
    enviar_recurso(pcb_actual, recurso, cliente_socket, PEDIDO_WAIT);
}

void _signal(char *recurso, int cliente_socket)
{
    enviar_recurso(pcb_actual, recurso, cliente_socket, PEDIDO_SIGNAL);
}*/

void _io_gen_sleep(char *interfaz, char *unidades_de_trabajo, int cliente_socket)
{
    int unidades_de_trabajoNum = atoi(unidades_de_trabajo);
    enviar_interfaz_IO(pcb_actual, interfaz, unidades_de_trabajoNum, cliente_socket, IO_GEN_SLEEP);
}

void _io_stdin_read(char *interfaz, char *direc_logica, char *tamanio, int cliente_socket)
{
    uint32_t direccionLogica32;

    if (revisar_registro(direc_logica))
    {
        direccionLogica32 = *(get_registry8(direc_logica));
    }
    else
    {
        direccionLogica32 = *(get_registry32(direc_logica));
    }

    uint32_t direccionFisica = traducir_direccion(pcb_actual->pid, direccionLogica32, TAM_PAGINA);
    uint32_t tamanioMaximoAingresar = str_to_uint32(tamanio);
    enviar_interfaz_IO_stdin(pcb_actual, interfaz, direccionFisica, tamanioMaximoAingresar, cliente_socket, IO_STDIN_READ);
}


// ENVIOS

void enviar_recurso(t_pcb *pcb, char *recurso, int cliente_socket, op_cod codigo_operacion)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(codigo_operacion);
    serializar_recurso(pcb, recurso, paquete);
    enviar_paquete(paquete, cliente_socket);
    eliminar_paquete(paquete);
}

// REVISAR
void serializar_recurso(t_pcb *pcb, char *recurso, t_paquete *paquete)
{
    paquete->buffer->size = sizeof(uint32_t) + strlen(recurso) + 1;
    paquete->buffer->stream = malloc(paquete->buffer->size);
    int offset = 0;
    memcpy(paquete->buffer->stream + offset, &(pcb->pid), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(paquete->buffer->stream + offset, recurso, strlen(recurso) + 1);
}

void enviar_resize_a_memoria(t_pcb *pcb, uint32_t tamanioAReasignar)
{
    t_paquete *paquete = crear_paquete_con_codigo_de_operacion(PEDIDO_RESIZE);
    serializar_resize(pcb, tamanioAReasignar, paquete);
    enviar_paquete(paquete, fd_cpu_memoria);
    eliminar_paquete(paquete);
}

void serializar_resize(t_pcb *pcb, uint32_t tamanioAReasignar, t_paquete *paquete)
{
    paquete->buffer->size = sizeof(uint32_t) + sizeof(uint32_t);
    paquete->buffer->stream = malloc(paquete->buffer->size);
    int offset = 0;
    memcpy(paquete->buffer->stream + offset, &(pcb->pid), sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(paquete->buffer->stream + offset, &tamanioAReasignar, sizeof(uint32_t));
}

uint32_t *get_registry32(char *registro)
{
    if (strcmp(registro, "EAX") == 0)
        return &(pcb_actual->contexto_ejecucion->registros->eax);
    else if (strcmp(registro, "EBX") == 0)
        return &(pcb_actual->contexto_ejecucion->registros->ebx);
    else if (strcmp(registro, "ECX") == 0)
        return &(pcb_actual->contexto_ejecucion->registros->ecx);
    else if (strcmp(registro, "EDX") == 0)
        return &(pcb_actual->contexto_ejecucion->registros->edx);
    else if (strcmp(registro, "DI") == 0)
        return &(pcb_actual->contexto_ejecucion->registros->di);
    else if (strcmp(registro, "SI") == 0)
        return &(pcb_actual->contexto_ejecucion->registros->si);
    else if (strcmp(registro, "PC") == 0)
        return &(pcb_actual->contexto_ejecucion->registros->program_counter);
    else
    {
        log_error(LOGGER_CPU, "No se reconoce el registro %s", registro);
        return NULL;
    }
}

uint8_t *get_registry8(char *registro)
{
    if (strcmp(registro, "AX") == 0)
        return &(pcb_actual->contexto_ejecucion->registros->ax);
    else if (strcmp(registro, "BX") == 0)
        return &(pcb_actual->contexto_ejecucion->registros->bx);
    else if (strcmp(registro, "CX") == 0)
        return &(pcb_actual->contexto_ejecucion->registros->cx);
    else if (strcmp(registro, "DX") == 0)
        return &(pcb_actual->contexto_ejecucion->registros->dx);
    else
    {
        log_error(LOGGER_CPU, "No se reconoce el registro %s", registro);
        return NULL;
    }
}

char *instruccion_to_string(nombre_instruccion nombre)
{
    switch (nombre)
    {
    case SET:
        return "SET";
    case SUM:
        return "SUM";
    case SUB:
        return "SUB";
    case JNZ:
        return "JNZ";
    case RESIZE:
        return "RESIZE";
    case COPY_STRING:
        return "COPY_STRING";
    case MOV_IN:
        return "MOV_IN";
    case MOV_OUT:
        return "MOV_OUT";
    case WAIT:
        return "WAIT";
    case SIGNAL:
        return "SIGNAL";
    case IO_GEN_SLEEP:
        return "IO_GEN_SLEEP";
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
        return "DESCONOCIDA";
    }
}

bool revisar_registro(char *registro)
{
    if (strcmp(registro, "AX") == 0)
        return true;
    else if (strcmp(registro, "BX") == 0)
        return true;
    else if (strcmp(registro, "CX") == 0)
        return true;
    else if (strcmp(registro, "DX") == 0)
        return true;
    else if (strcmp(registro, "EAX") == 0)
        return false;
    else if (strcmp(registro, "EBX") == 0)
        return false;
    else if (strcmp(registro, "ECX") == 0)
        return false;
    else if (strcmp(registro, "EDX") == 0)
        return false;
    else if (strcmp(registro, "DI") == 0)
        return false;
    else if (strcmp(registro, "SI") == 0)
        return false;
    else if (strcmp(registro, "PC") == 0)
        return false;
    else
    {
        log_error(LOGGER_CPU, "No se reconoce el registro %s", registro);
        return false;
    }
}