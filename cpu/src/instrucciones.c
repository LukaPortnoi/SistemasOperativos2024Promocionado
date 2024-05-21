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
    uint8_t *regis;

    if (revisar_registro(registro))
    {
        regis = get_registry8(registro);
    }
    else
    {
        uint32_t *regis = (uint32_t *)regis;
        regis = get_registry32(registro);
    }

    uint32_t valor = 0;
    if (valor != -1)
    {
        *(regis) = valor;
    }
}

//(Registro Dirección, Registro Datos): Lee el valor del Registro Datos y lo escribe en
// la dirección física de memoria obtenida a partir de la Dirección Lógica almacenada en el Registro Dirección.
void _mov_out(char *direc_logica, char *registro)
{
    uint8_t *regis;

    if (revisar_registro(registro))
    {
        regis = get_registry8(registro);
    }
    else
    {
        uint32_t *regis = (uint32_t *)regis;
        regis = get_registry32(registro);
    }

    uint32_t valor = 0;
    if (valor != -1)
    {
        *(regis) = valor;
    }
}

// (Registro Destino, Registro Origen): Suma al Registro Destino el
// Registro Origen y deja el resultado en el Registro Destino.
void _sum(char *registro_destino, char *registro_origen)
{
    uint8_t *destino;
    uint8_t *origen;

    if (revisar_registro(registro_destino))
    {
        destino = get_registry8(registro_destino);
    }
    else
    {
        uint32_t *destino = (uint32_t *)destino;
        destino = get_registry32(registro_destino);
    }

    if (revisar_registro(registro_origen))
    {
        origen = get_registry8(registro_origen);
    }
    else
    {
        uint32_t *origen = (uint32_t *)origen;
        origen = get_registry32(registro_origen);
    }

    *(destino) = *(destino) + *(origen);
}

// (Registro Destino, Registro Origen): Resta al Registro Destino
// el Registro Origen y deja el resultado en el Registro Destino
void _sub(char *registro_destino, char *registro_origen)
{
    uint8_t *destino;
    uint8_t *origen;

    if (revisar_registro(registro_destino))
    {
        destino = get_registry8(registro_destino);
    }
    else
    {
        uint32_t *destino = (uint32_t *)destino;
        destino = get_registry32(registro_destino);
    }

    if (revisar_registro(registro_origen))
    {
        origen = get_registry8(registro_origen);
    }
    else
    {
        uint32_t *origen = (uint32_t *)origen;
        origen = get_registry32(registro_origen);
    }

    *(destino) = *(destino) - *(origen);
}

// (Registro, Instrucción): Si el valor del registro es distinto de cero,
// actualiza el program counter al número de instrucción pasada por parámetro.
void _jnz(char *registro, char *instruccion)
{
    uint8_t *regis;

    if (revisar_registro(registro))
    {
        regis = get_registry8(registro);
    }
    else
    {
        uint32_t *regis = (uint32_t *)regis;
        regis = get_registry32(registro);
    }

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

void _io_gen_sleep(char *interfaz, char *unidades_de_trabajo, int cliente_socket)
{   
    uint32_t unidades_de_trabajoNum;
    if (isdigit(unidades_de_trabajo[0])) {
        // Convertir char a uint32_t
         unidades_de_trabajoNum = (uint32_t)(unidades_de_trabajo[0] - '0');

    } else {
        printf("El carácter %c no es un dígito.\n", unidades_de_trabajo[0]);
    }
       //uint32_t unidades_de_trabajoNum = atoi(&unidades_de_trabajo);

   // pcb_actual->contexto_ejecucion->motivo_desalojo = ENVIAR_INTERFAZ;
   enviar_interfaz_IO(pcb_actual, interfaz, unidades_de_trabajoNum, cliente_socket);
    pcb_actual = recibir_pcb(cliente_socket);
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