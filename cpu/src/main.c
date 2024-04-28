#include "../include/main.h"

t_contexto_ejecucion *contexto_actual;

char *IP_MEMORIA;
char *IP_CPU;
char *PUERTO_MEMORIA;
char *PUERTO_ESCUCHA_DISPATCH;
char *PUERTO_ESCUCHA_INTERRUPT;
char *CANTIDAD_ENTRADAS_TLB;
char *ALGORITMO_TLB;

t_log *LOGGER_CPU;
t_config *CONFIG;

int fd_cpu_dispatch;
int fd_cpu_interrupt;
int fd_cpu_memoria;

pthread_t hilo_interrupt;

op_cod cod_op;

int main()
{
    inicializar_config();
    log_info(LOGGER_CPU, "Iniciando CPU...");

    iniciar_conexiones();

    while (server_escuchar(LOGGER_CPU, "CPU_DISPATCH", fd_cpu_dispatch));

    /*while (1)
    {
        cod_op = recibir_operacion(fd_cpu_dispatch);

        switch (cod_op)
        {
        case CONTEXTO:
            log_info(LOGGER_CPU, "Recibiendo PCB...");
            break;

        default:
            break;
        }
    }*/

    terminar_programa(fd_cpu_dispatch, LOGGER_CPU, CONFIG);
    // terminar_programa(fd_cpu_interrupt, LOGGER_CPU, CONFIG);
}

void inicializar_config()
{
    LOGGER_CPU = iniciar_logger("cpu.log", "CPU");
    CONFIG = iniciar_config("./cpu.config", "CPU");
    IP_MEMORIA = config_get_string_value(CONFIG, "IP_MEMORIA");
    IP_CPU = config_get_string_value(CONFIG, "IP_CPU");
    PUERTO_MEMORIA = config_get_string_value(CONFIG, "PUERTO_MEMORIA");
    PUERTO_ESCUCHA_DISPATCH = config_get_string_value(CONFIG, "PUERTO_ESCUCHA_DISPATCH");
    PUERTO_ESCUCHA_INTERRUPT = config_get_string_value(CONFIG, "PUERTO_ESCUCHA_INTERRUPT");
    CANTIDAD_ENTRADAS_TLB = config_get_string_value(CONFIG, "CANTIDAD_ENTRADAS_TLB");
    ALGORITMO_TLB = config_get_string_value(CONFIG, "ALGORITMO_TLB");
}

void iniciar_conexiones()
{
    // server CPU DISPATCH
    fd_cpu_dispatch = iniciar_servidor(LOGGER_CPU, "CPU_DISPATCH", IP_CPU, PUERTO_ESCUCHA_DISPATCH);
    log_info(LOGGER_CPU, "CPU listo para recibir cliente en DISPATCH");

    // server CPU INTERRUPT
    fd_cpu_interrupt = iniciar_servidor(LOGGER_CPU, "CPU_INTERRUPT", IP_CPU, PUERTO_ESCUCHA_INTERRUPT);
    log_info(LOGGER_CPU, "CPU listo para recibir cliente en INTERRUPT");

    // conexion como cliente a MEMORIA
    fd_cpu_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
    enviar_mensaje("Mensaje de CPU para memoria", fd_cpu_memoria);

    // hilo para escuchar interrupciones
    pthread_create(&hilo_interrupt, NULL, (void *)escuchar_interrupt, NULL);
    pthread_detach(hilo_interrupt);
}

void escuchar_interrupt()
{
    while (server_escuchar(LOGGER_CPU, "CPU_INTERRUPT", fd_cpu_interrupt))
        ;
}

while (1)
{

    codigo_operacion = recibir_operacion(fd_cpu_dispatch);

    switch (codigo_operacion)
    {
    case CONTEXTO:
        contexto_actual = recibir_contexto(fd_cpu_dispatch);
        contexto_actual->codigo_ultima_instru = -1;

        while (!es_syscall() && !hay_interrupciones() && !page_fault && contexto_actual != NULL)
        {
            ejecutar_ciclo_instruccion();
        }
        //obtener_motivo_desalojo();
        enviar_contexto(fd_cpu_dispatch, contexto_actual);

        contexto_actual = NULL;

       // pthread_mutex_lock(&mutex_interrupt);
        //limpiar_interrupciones();
       // pthread_mutex_unlock(&mutex_interrupt);
        // liberar_contexto(contexto_actual);

        break;
    default:
        log_error(cpu_logger_info, "El codigo que me llego es %d", codigo_operacion);
        log_warning(cpu_logger_info, "Operacion desconocida \n");
        finalizar_cpu();
        abort();
        break;
    }
}
/*void *recibir_interrupt(void *arg)
{
    while (1)
    {
        codigo_operacion = recibir_operacion(conexion_kernel_interrupt);
        if (codigo_operacion != INTERRUPCION)
        {
            finalizar_cpu();
            abort();
        }
        t_interrupcion *interrupcion = recibir_interrupcion(conexion_kernel_interrupt);
        if (contexto_actual != NULL)
        {
            switch (interrupcion->motivo_interrupcion)
            {
            case INTERRUPT_FIN_QUANTUM:
                interrupciones[INTERRUPT_FIN_QUANTUM] = 1;
                break;
            case INTERRUPT_FIN_PROCESO:
                if (!descartar_instruccion)
                {
                    interrupciones[INTERRUPT_FIN_PROCESO] = 1;
                }
                break;
            case INTERRUPT_NUEVO_PROCESO:
                interrupciones[INTERRUPT_NUEVO_PROCESO] = 1;
                break;
            default:
                finalizar_cpu();
                abort();
                break;
            }
        }
        free(interrupcion);
    }

    return NULL;
} */

void ejecutar_ciclo_instruccion()
{
    t_instruccion *instruccion = fetch(contexto_actual->pid, contexto_actual->program_counter);
    decode(instruccion);
    if (!page_fault)
        contexto_actual->program_counter++;
}

t_instruccion *fetch(int pid, int pc)
{
    // TODO -- chequear que en los casos de instruccion con memoria logica puede dar PAGE FAULT y no hay que aumentar el pc (restarlo dentro del decode en esos casos)
    ask_instruccion_pid_pc(pid, pc, socket_memoria);

    op_code codigo_op = recibir_operacion(socket_memoria);

    t_instruccion *instruccion;

    if (codigo_op == INSTRUCCION)
    {
        instruccion = deserializar_instruccion(socket_memoria);
        contexto_actual->instruccion_ejecutada = instruccion;
    }
    else
    {
        log_warning(cpu_logger_info, "Operación desconocida. No se pudo recibir la instruccion de memoria.");
        abort();
    }

    log_info(cpu_logger_info, "PID: %d - FETCH - Program Counter: %d", pid, pc);

    return instruccion;
}

void decode(t_instruccion *instruccion)
{
    char *param1 = string_new();
    char *param2 = string_new();
    if (instruccion->parametro1 != NULL)
    {
        strcpy(param1, instruccion->parametro1);
    }
    if (instruccion->parametro2 != NULL)
    {
        strcpy(param2, instruccion->parametro2);
    }
    log_info(cpu_logger_info, "PID: %d - Ejecutando: %s - Parametros: %s - %s", contexto_actual->pid, cod_inst_to_str(instruccion->codigo), param1, param2);

    switch (instruccion->codigo)
    {
    case SET:
        _set(instruccion->parametro1, instruccion->parametro2);
        break;
    case SUM:
        _sum(instruccion->parametro1, instruccion->parametro2);
        break;
    case SUB:
        _sub(instruccion->parametro1, instruccion->parametro2);
        break;
    case JNZ:
        _jnz(instruccion->parametro1, instruccion->parametro2);
        break;
    case SLEEP:
        _sleep();
        break;
    case WAIT:
        _wait();
        break;
    case SIGNAL:
        _signal();
        break;
    case MOV_IN:
        _mov_in(instruccion->parametro1, instruccion->parametro2);
        break;
    case MOV_OUT:
        _mov_out(instruccion->parametro1, instruccion->parametro2);
        break;
    case F_OPEN:
        _f_open(instruccion->parametro1, instruccion->parametro2);
        break;
    case F_CLOSE:
        _f_close(instruccion->parametro1);
        break;
    case F_SEEK:
        _f_seek(instruccion->parametro1, instruccion->parametro2);
        break;
    case F_READ:
        _f_read(instruccion->parametro1, instruccion->parametro2);
        break;
    case F_WRITE:
        _f_write(instruccion->parametro1, instruccion->parametro2);
        break;
    case F_TRUNCATE:
        _f_truncate(instruccion->parametro1, instruccion->parametro2);
        break;
    case EXIT:
        __exit(contexto_actual);
        break;
    default:
        log_error(cpu_logger_info, "Instruccion no reconocida");
        break;
    }

    // free(param1);
    // free(param2);
}