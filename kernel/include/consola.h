#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <stdlib.h>
#include <stdio.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "../../utils/include/sockets_server.h"
#include "../../utils/include/sockets_client.h"
#include "../../utils/include/sockets_utils.h"
#include "../../utils/include/pcb.h"

#include "./comunicaciones.h"
#include "./gestor.h"
#include "./planificador.h"

void iniciar_consola_interactiva();

bool validar_comando(char *comando);
void ejecutar_comando(char *comando);
void ejecutar_script(char *path);

void iniciar_proceso(char *path_proceso);
void finalizar_proceso(char *pid_string);

void iniciar_planificacion(void);
void detener_planificacion(void);

void cambiar_multiprogramacion(char *grado_multiprogramacion_string);
void mostrar_listado_estado_procesos();
void mostrar_procesos_en_cola(t_queue *cola, const char *nombre_cola);

#endif /* CONSOLA_H_ */