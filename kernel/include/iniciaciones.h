#ifndef INICIACIONES_H
#define INICIACIONES_H

t_log *iniciar_logger(void);
t_config *iniciar_config(void);
void leer_consola(t_log *logger);
void paquete(int conexion, t_log *logger);

#endif