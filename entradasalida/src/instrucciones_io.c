#include "../include/instrucciones_io.h"

void procesar_sleep(int socket_cliente, t_log *logger)
{
    t_interfaz_gen *interfazRecibida = recibir_InterfazGenerica(socket_cliente);
    log_info(logger, "PID: %d - Operacion: IO_GEN_SLEEP", interfazRecibida->pidPcb);

    log_trace(logger, "Durmiendo: %d segundos", interfazRecibida->unidades_de_trabajo);
    sleep(interfazRecibida->unidades_de_trabajo);

    enviar_InterfazGenericaConCodigoOP(socket_cliente, interfazRecibida->unidades_de_trabajo, interfazRecibida->pidPcb, interfazRecibida->nombre_interfaz);
    free(interfazRecibida->nombre_interfaz);
    free(interfazRecibida);
}

void procesar_stdin(int socket_cliente, t_log *logger)
{
    t_interfaz_stdin *interfazRecibida = recibir_InterfazStdin(socket_cliente);
    log_info(logger, "PID: %d - Operacion: IO_STDIN_READ", interfazRecibida->pidPcb);

    uint32_t tamanioTotal = 0;
    for (int i = 0; i < list_size(interfazRecibida->direccionesFisicas); i++)
    {
        t_direcciones_fisicas *direccionAmostrar = list_get(interfazRecibida->direccionesFisicas, i);
        tamanioTotal = tamanioTotal + direccionAmostrar->tamanio;
    }

    char *datoRecibido;
    datoRecibido = procesarIngresoUsuario(tamanioTotal);
    enviar_dato_stdin(fd_io_memoria, interfazRecibida->direccionesFisicas, datoRecibido, interfazRecibida->pidPcb );
    free(datoRecibido);

    enviar_InterfazStdinConCodigoOPaKernel(socket_cliente, interfazRecibida->direccionesFisicas, interfazRecibida->pidPcb, interfazRecibida->nombre_interfaz);
}

char *procesarIngresoUsuario(uint32_t tamanioMaximo)
{
    char *leido = readline("> ");
    int longitud_bytes = strlen(leido);

    while (longitud_bytes > tamanioMaximo)
    {
        log_error(LOGGER_INPUT_OUTPUT, "El dato ingresado supera el tamaño máximo permitido. Por favor, vuelva a ingresar el dato.");
        free(leido);
        leido = readline("> ");
        longitud_bytes = strlen(leido);
    }
    return leido;
}

void procesar_stdout(int socket_cliente, t_log *logger)
{
    t_interfaz_stdout *interfazRecibida = recibir_InterfazStdout(socket_cliente);
    log_info(logger, "PID: %d - Operacion: IO_STDOUT_WRITE", interfazRecibida->pidPcb);

    enviar_direcciones_stdout(fd_io_memoria, interfazRecibida->direccionesFisicas, interfazRecibida->pidPcb);
    char *datoRecibido = recibir_dato(fd_io_memoria, logger);
    imprimir_dato_recibido_de_memoria(datoRecibido);

    enviar_InterfazStdoutConCodigoOPaKernel(socket_cliente, interfazRecibida->direccionesFisicas, interfazRecibida->pidPcb, interfazRecibida->nombre_interfaz);
}

void imprimir_dato_recibido_de_memoria(char *dato)
{
    log_info(LOGGER_INPUT_OUTPUT, "Dato encontrado: %s", dato);
}