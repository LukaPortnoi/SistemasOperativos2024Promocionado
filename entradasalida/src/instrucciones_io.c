#include "../include/instrucciones_io.h"

void procesar_sleep(int socket_cliente, t_log *logger)
{
    log_info(logger, "Procesando instrucción sleep");
    t_interfaz_gen *interfazRecibida = recibir_InterfazGenerica(socket_cliente);

    log_info(logger, "Durmiendo %d segundos", interfazRecibida->unidades_de_trabajo);
    sleep(interfazRecibida->unidades_de_trabajo);
    log_info(logger, "Instrucción sleep finalizada");

    // Le aviso al cliente que terminé
    enviar_InterfazGenericaConCodigoOP(socket_cliente, interfazRecibida->unidades_de_trabajo, interfazRecibida->pidPcb, interfazRecibida->nombre_interfaz);
}