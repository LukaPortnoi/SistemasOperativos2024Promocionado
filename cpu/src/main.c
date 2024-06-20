#include "../include/main.h"

char *IP_MEMORIA;
char *IP_CPU;
char *PUERTO_MEMORIA;
char *PUERTO_ESCUCHA_DISPATCH;
char *PUERTO_ESCUCHA_INTERRUPT;
int CANTIDAD_ENTRADAS_TLB;
char *ALGORITMO_TLB;

t_log *LOGGER_CPU;
t_config *CONFIG;
uint32_t TAM_PAGINA;

int fd_cpu_dispatch;
int fd_cpu_interrupt;
int fd_cpu_memoria;

t_pcb *pcb_actual;

pthread_t hilo_interrupt;
pthread_mutex_t mutex_pcb_actual;
pthread_mutex_t mutex_interrupt;

op_cod cod_op;

int main()
{
    signal(SIGINT, manejador_signals);
    signal(SIGSEGV, manejador_signals);
    inicializar_config();
    iniciar_semaforos_etc();
    inicializar_tlb();
    pcb_actual = inicializar_pcb();
    if (pcb_actual == NULL)
    {
        // manejar error de asignación de memoria
    }

    iniciar_conexiones();

    while (server_escuchar(LOGGER_CPU, "CPU_DISPATCH", fd_cpu_dispatch))
        ;

    finalizar_cpu();
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
    CANTIDAD_ENTRADAS_TLB = config_get_int_value(CONFIG, "CANTIDAD_ENTRADAS_TLB");
    ALGORITMO_TLB = config_get_string_value(CONFIG, "ALGORITMO_TLB");
    TAM_PAGINA = config_get_int_value(CONFIG, "TAM_PAGINA");
}

void iniciar_conexiones()
{
    // server CPU DISPATCH
    fd_cpu_dispatch = iniciar_servidor(LOGGER_CPU, "CPU_DISPATCH", IP_CPU, PUERTO_ESCUCHA_DISPATCH);
    // log_info(LOGGER_CPU, "CPU listo para recibir cliente en DISPATCH");

    // server CPU INTERRUPT
    fd_cpu_interrupt = iniciar_servidor(LOGGER_CPU, "CPU_INTERRUPT", IP_CPU, PUERTO_ESCUCHA_INTERRUPT);
    // log_info(LOGGER_CPU, "CPU listo para recibir cliente en INTERRUPT");

    // conexion como cliente a MEMORIA
    fd_cpu_memoria = crear_conexion(IP_MEMORIA, PUERTO_MEMORIA);
    // enviar_mensaje("Mensaje de CPU para memoria", fd_cpu_memoria);

    // hilo para escuchar interrupciones
    pthread_create(&hilo_interrupt, NULL, (void *)escuchar_interrupt, NULL);
    pthread_detach(hilo_interrupt);
}

void escuchar_interrupt()
{
    while (server_escuchar(LOGGER_CPU, "CPU_INTERRUPT", fd_cpu_interrupt))
        ;
}

void finalizar_cpu()
{
    if (pcb_actual != NULL)
    {
        destruir_pcb(pcb_actual);
    }
    log_destroy(LOGGER_CPU);
    config_destroy(CONFIG);
    liberar_conexion(fd_cpu_dispatch);
    liberar_conexion(fd_cpu_interrupt);
    liberar_conexion(fd_cpu_memoria);
}

void manejador_signals(int signum)
{
    switch (signum)
    {
    case SIGUSR1:
        log_trace(LOGGER_CPU, "Se recibio la señal SIGUSR1\n");
        break;

    case SIGUSR2:
        log_trace(LOGGER_CPU, "Se recibio la señal SIGUSR2\n");
        break;

    case SIGINT:
        log_trace(LOGGER_CPU, "Se recibio la señal SIGINT\n");
        log_trace(LOGGER_CPU, "Se recibió SIGINT, cerrando conexiones y liberando recursos...");
        finalizar_cpu();
        destruir_tlb();
        destruir_pcb(pcb_actual);
        pthread_mutex_destroy(&mutex_interrupt);
        exit(0);
        break;

    case SIGSEGV:
        log_error(LOGGER_CPU, "Se recibio la señal SIGSEGV\n");
        log_trace(LOGGER_CPU, "Se recibió SIGINT, cerrando conexiones y liberando recursos...");
        finalizar_cpu();
        destruir_tlb();
        pthread_mutex_destroy(&mutex_interrupt);
        exit(0);
        break;

    case SIGTERM:
        log_trace(LOGGER_CPU, "Se recibio la señal SIGTERM\n");
        break;

    default:
        log_trace(LOGGER_CPU, "Se recibio una señal no manejada\n");
        break;
    }
}

t_pcb *inicializar_pcb()
{
    t_pcb *pcb = malloc(sizeof(t_pcb));
    if (pcb == NULL)
    {
        // manejar error de asignación de memoria
        return NULL;
    }

    pcb->pid = 0;                            // o cualquier valor inicial apropiado
    pcb->estado = NUEVO;                     // o cualquier valor inicial apropiado
    pcb->quantum = 0;                        // o cualquier valor inicial apropiado
    pcb->tiempo_q = 0;                       // o cualquier valor inicial apropiado
    pcb->recursos_asignados = list_create(); // asumiendo que t_list es una lista enlazada
    if (pcb->recursos_asignados == NULL)
    {
        // manejar error de asignación de memoria
        free(pcb);
        return NULL;
    }

    pcb->contexto_ejecucion = malloc(sizeof(t_contexto_ejecucion));
    if (pcb->contexto_ejecucion == NULL)
    {
        // manejar error de asignación de memoria
        list_destroy(pcb->recursos_asignados);
        free(pcb);
        return NULL;
    }

    pcb->contexto_ejecucion->registros = malloc(sizeof(t_registros));
    if (pcb->contexto_ejecucion->registros == NULL)
    {
        free(pcb->contexto_ejecucion);
        list_destroy(pcb->recursos_asignados);
        free(pcb);
        return NULL;
    }
    pcb->contexto_ejecucion->motivo_desalojo = SIN_MOTIVO;                  // o cualquier valor inicial apropiado
    pcb->contexto_ejecucion->motivo_finalizacion = FINALIZACION_SIN_MOTIVO; // o cualquier valor inicial apropiado

    return pcb;
}
