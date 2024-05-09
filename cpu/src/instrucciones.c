#include "../include/instrucciones.h"

// (Registro, Valor): Asigna al registro el valor pasado como parámetro.
void _set(char *registro, char *valor)
{
    *(get_registry(registro)) = str_to_uint32(valor);
}

// (Registro Datos, Registro Dirección): Lee el valor de memoria correspondiente a
// la Dirección Lógica que se encuentra en el Registro Dirección y lo almacena en el Registro Datos.
void _mov_in(char *registro, char *direc_logica)
{
    uint32_t *regis = get_registry(registro);

    uint32_t valor = 0; // obtener_valor_dir(str_to_uint32(direc_logica)); NO EXISTEEEEEEEEE
    if (valor != -1)
    {
        *(regis) = valor;
    }
}

//(Registro Dirección, Registro Datos): Lee el valor del Registro Datos y lo escribe en
// la dirección física de memoria obtenida a partir de la Dirección Lógica almacenada en el Registro Dirección.
void _mov_out(char *direc_logica, char *registro)
{
    uint32_t *regis = get_registry(registro);

    // escribir_memoria(str_to_uint32(direc_logica), regis); ???????????????????

    // TODO: solo cambiar si no es page fault
    free(regis);
}

// (Registro Destino, Registro Origen): Suma al Registro Destino el
// Registro Origen y deja el resultado en el Registro Destino.
void _sum(char *registro_destino, char *registro_origen)
{
    uint32_t *destino = get_registry(registro_destino);
    uint32_t *origen = get_registry(registro_origen);

    *(destino) = *(destino) + *(origen);
}

// (Registro Destino, Registro Origen): Resta al Registro Destino
// el Registro Origen y deja el resultado en el Registro Destino
void _sub(char *registro_destino, char *registro_origen)
{
    uint32_t *destino = get_registry(registro_destino);
    uint32_t *origen = get_registry(registro_origen);

    *(destino) = *(destino) - *(origen);
}

// (Registro, Instrucción): Si el valor del registro es distinto de cero,
// actualiza el program counter al número de instrucción pasada por parámetro.
void _jnz(char *registro, char *instruccion)
{
    uint32_t *regis = get_registry(registro);

    if (regis != 0)
    {
        pcb_actual->contexto_ejecucion->registros->program_counter = str_to_uint32(instruccion);
    }
    else
    {
        log_warning(LOGGER_CPU, "El registro %s es igual a cero, no se actualiza el IP", registro);
    }

    free(regis);
}

void _io_gen_sleep(char *interfaz, int unidades_de_trabajo)
{
    // sleep
}

uint32_t *get_registry(char *registro)
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