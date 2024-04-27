#include "../include/main.h"

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

op_cod cod_op;

int main()
{
    inicializar_config();
    log_info(LOGGER_CPU, "Iniciando CPU...");

    iniciar_conexiones();

    while (server_escuchar(LOGGER_CPU, "CPU_DISPATCH", fd_cpu_dispatch)){}
    while (server_escuchar(LOGGER_CPU, "CPU_INTERRUPT", fd_cpu_interrupt)){}
    

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
    //terminar_programa(fd_cpu_interrupt, LOGGER_CPU, CONFIG);
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
    fd_cpu_dispatch = iniciar_servidor(LOGGER_CPU, "CPU - DISPATCH", IP_CPU, PUERTO_ESCUCHA_DISPATCH);
    log_info(LOGGER_CPU, "CPU listo para recibir cliente en DISPATCH");

    // server CPU INTERRUPT
    fd_cpu_interrupt = iniciar_servidor(LOGGER_CPU, "CPU - INTERRUPT", IP_CPU, PUERTO_ESCUCHA_INTERRUPT);
    log_info(LOGGER_CPU, "CPU listo para recibir cliente en INTERRUPT");

    // conexion como cliente a MEMORIA
    fd_cpu_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
    enviar_mensaje("Mensaje de CPU para memoria", fd_cpu_memoria);
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
}*/