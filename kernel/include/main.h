#ifndef MAIN_H_
#define MAIN_H_

#include "./gestor.h"
#include "./comunicaciones.h"
#include "./consola.h"
#include "./planificador.h"

void inicializar_config();
void iniciar_conexiones();
void escuchar_kernel();

#endif // MAIN_H_