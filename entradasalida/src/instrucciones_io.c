#include "../include/instrucciones_io.h"

void procesar_sleep(int socket_cliente, t_log *logger)
{
    t_interfaz_gen *interfazRecibida = recibir_InterfazGenerica(socket_cliente);
    log_info(logger, "PID: %d - Operacion: IO_GEN_SLEEP", interfazRecibida->pidPcb);

    log_trace(logger, "Durmiendo: %d segundos", interfazRecibida->unidades_de_trabajo);
    sleep(interfazRecibida->unidades_de_trabajo);

    enviar_InterfazGenericaConCodigoOP(socket_cliente, interfazRecibida->unidades_de_trabajo, interfazRecibida->pidPcb, interfazRecibida->nombre_interfaz);
}