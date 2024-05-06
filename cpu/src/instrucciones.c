#include "../include/instrucciones.h"

// Asigna al registro el valor pasado como parámetro.
void _set(char *registro, char *valor)
{
    *(get_registry(registro)) = str_to_uint32(valor);
}

// Suma al registro el valor pasado como parámetro.
void _sum(char *registro_destino, char *registro_origen)
{
    uint32_t *destino = get_registry(registro_destino);
    uint32_t *origen = get_registry(registro_origen);

    *(destino) = *(destino) + *(origen);
}

// Resta al registro el valor pasado como parámetro.
void _sub(char *registro_destino, char *registro_origen)
{
    uint32_t *destino = get_registry(registro_destino);
    uint32_t *origen = get_registry(registro_origen);

    *(destino) = *(destino) - *(origen);
}

// Si valor del registro != cero, actualiza el IP al número de instrucción pasada por parámetro.
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

// Syscall bloqueante. Devuelve el contexto_actual de Ejecución actualizado al Kernel
// junto a la cantidad de segundos que va a bloquearse el proceso.
void _sleep()
{

    pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_SYSCALL;
}

// Esta instrucción solicita al Kernel que se asigne una instancia del recurso indicado por parámetro.
void _wait()
{
    pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_SYSCALL;
}

//  Esta instrucción solicita al Kernel que se libere una instancia del recurso indicado por parámetro.
void _signal()
{
    pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_SYSCALL;
}

// Lee el valor de memoria correspondiente a la Dirección Lógica y lo almacena en el Registro.
void _mov_in(char *registro, char *direc_logica)
{
    uint32_t *regis = get_registry(registro);

    uint32_t valor = 0; // obtener_valor_dir(str_to_uint32(direc_logica)); NO EXISTEEEEEEEEE
    if (valor != -1)
    {
        *(regis) = valor;
    }
}

// Lee el valor del Registro y lo escribe en la dirección
// física de memoria obtenida a partir de la Dirección Lógica.
void _mov_out(char *direc_logica, char *registro)
{
    uint32_t *regis = get_registry(registro);

    // escribir_memoria(str_to_uint32(direc_logica), regis); ???????????????????

    // TODO: solo cambiar si no es page fault
    free(regis);
}

// Solicita al kernel que abra el archivo pasado por parámetro con el modo de apertura indicado.
void _f_open(char *nombre_archivo, char *modo_apertura)
{
    pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_SYSCALL;
}

// Solicita al kernel que cierre el archivo pasado por parámetro.
void _f_close(char *nombre_archivo)
{
    pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_SYSCALL;
}

// Solicita al kernel actualizar el puntero del archivo a la posición pasada por parámetro.
void _f_seek(char *nombre_archivo, char *posicion)
{
    pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_SYSCALL;
}

// Solicita al Kernel que se lea del archivo indicado y
// se escriba en la dirección física de Memoria la información leída
void _f_read(char *nombre_archivo, char *direc_logica)
{
    traducir_dl_fs(direc_logica);

    pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_SYSCALL;
}

// Solicita al Kernel que se escriba en el archivo indicado l
// la información que es obtenida a partir de la dirección física de Memoria.
void _f_write(char *nombre_archivo, char *direc_logica)
{
    traducir_dl_fs(direc_logica);

    pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_SYSCALL;
}

//  Solicita al Kernel que se modifique el tamaño del archivo al indicado por parámetro.
void _f_truncate(char *nombre_archivo, char *tamanio)
{
    pcb_actual->contexto_ejecucion->motivo_desalojo = INTERRUPCION_SYSCALL;
}

// representa la syscall de finalización del proceso.
// Se deberá devolver el contexto_actual de ejecución actualizado al kernel
void __exit()
{
    // exitea
}

void traducir_dl_fs(char *dl)
{
    int df = 0; // traducir_dl(str_to_uint32(dl)); // NO EXISTE
    if (df == -1)
    {
        log_error(LOGGER_CPU, "Page fault: %s", dl);
    }
    else
    {
        log_error(LOGGER_CPU, "Thomy puto");

        // char *df_string = atoi(df);
        //  NO GUARDAMOS EN EL PCB YA LAS INSTRUCCIONES, HABRIA QUE VER COMO HACER ESTO DESPUES XDDDDDD
        // contexto_actual->instruccion_ejecutada->longitud_parametro2 = strlen(df_string) + 1;
        // contexto_actual->instruccion_ejecutada->parametro2 = strdup(df_string);
    }
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