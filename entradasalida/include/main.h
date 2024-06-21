#ifndef MAINH
#define MAINH

#include "./gestor.h"
#include "./comunicaciones.h"

void inicializar_config(char *config_path);
void iniciar_conexiones();
void manejador_signals(int signum);
void iniciar_io(char *arg);
void finalizar_io();
void eliminar_extension(char *nombre_archivo);
char *extraer_nombre_interfaz(char *path);
char *agregar_prefijo_y_extension(const char *nombre_interfaz);
void asignar_tipo_interfaz(const char *t_interfaz);

#endif // MAIN_H