#include "../include/consola.h"

void iniciar_consola_interactiva()
{
    log_trace(LOGGER_KERNEL, "<CONSOLA INTERACTIVA>");
    char *leido;
    leido = readline("> ");

    while (1)
    {
        if (leido)
        {
            add_history(leido);
        }
        if (!strncmp(leido, "exit", 4))
        {
            free(leido);
            break;
        }
        if (!validar_comando(leido))
        {
            log_error(LOGGER_KERNEL, "Comando de CONSOLA no reconocido");
            free(leido);
            leido = readline("> ");
            continue;
        }
        ejecutar_comando(leido);
        free(leido);
        leido = readline("> ");
    }

    free(leido);
}

bool validar_comando(char *comando)
{
    bool es_comando_valido = false;

    char **comando_separado = string_split(comando, " ");

    if (strcmp(comando_separado[0], "EJECUTAR_SCRIPT") == 0 && comando_separado[1] != NULL)
    {
        es_comando_valido = true;
    }
    else if (strcmp(comando_separado[0], "INICIAR_PROCESO") == 0 && comando_separado[1] != NULL)
    {
        es_comando_valido = true;
    }
    else if (strcmp(comando_separado[0], "FINALIZAR_PROCESO") == 0 && comando_separado[1] != NULL)
    {
        es_comando_valido = true;
    }
    else if (strcmp(comando_separado[0], "DETENER_PLANIFICACION") == 0)
    {
        es_comando_valido = true;
    }
    else if (strcmp(comando_separado[0], "INICIAR_PLANIFICACION") == 0)
    {
        es_comando_valido = true;
    }
    else if (strcmp(comando_separado[0], "MULTIPROGRAMACION") == 0 && comando_separado[1] != NULL)
    {
        es_comando_valido = true;
    }
    else if (strcmp(comando_separado[0], "PROCESO_ESTADO") == 0)
    {
        es_comando_valido = true;
    }

    string_array_destroy(comando_separado);

    return es_comando_valido;
}

void ejecutar_comando(char *comando)
{
    char **comando_separado = string_split(comando, " ");

    if (strcmp(comando_separado[0], "EJECUTAR_SCRIPT") == 0)
    {
        log_info(LOGGER_KERNEL, "Ejecutando script desde %s...", comando_separado[1]);
        ejecutar_script(comando_separado[1]);
    }
    else if (strcmp(comando_separado[0], "INICIAR_PROCESO") == 0)
    {
        log_info(LOGGER_KERNEL, "Iniciando proceso desde %s...", comando_separado[1]);
        iniciar_proceso(comando_separado[1]);
    }
    else if (strcmp(comando_separado[0], "FINALIZAR_PROCESO") == 0)
    {
        log_info(LOGGER_KERNEL, "Finalizando proceso %s...", comando_separado[1]);
        finalizar_proceso(comando_separado[1]);
    }
    else if (strcmp(comando_separado[0], "DETENER_PLANIFICACION") == 0)
    {
        log_info(LOGGER_KERNEL, "Deteniendo planificacion...");
        detener_planificacion();
    }
    else if (strcmp(comando_separado[0], "INICIAR_PLANIFICACION") == 0)
    {
        log_info(LOGGER_KERNEL, "Iniciando planificacion...");
        iniciar_planificacion();
    }
    else if (strcmp(comando_separado[0], "MULTIPROGRAMACION") == 0)
    {
        log_info(LOGGER_KERNEL, "Cambiando grado de multiprogramacion a %s...", comando_separado[1]);
        cambiar_multiprogramacion(comando_separado[1]);
    }
    else if (strcmp(comando_separado[0], "PROCESO_ESTADO") == 0)
    {
        log_info(LOGGER_KERNEL, "Estado del proceso %s...", comando_separado[1]);
        mostrar_listado_estados_procesos();
    }

    string_array_destroy(comando_separado);
}

void ejecutar_script(char *path_script)
{
    FILE *script = fopen(path_script, "r");

    if (script == NULL)
    {
        log_error(LOGGER_KERNEL, "No se pudo abrir el archivo desde: %s", path_script);
        return;
    }

    char *linea = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&linea, &len, script)) != -1)
    {
        if (linea[read - 1] == '\n')
        {
            linea[read - 1] = '\0';
        }

        if (!validar_comando(linea))
        {
            log_error(LOGGER_KERNEL, "Comando de CONSOLA no reconocido");
            continue;
        }

        ejecutar_comando(linea);
    }

    free(linea);
    fclose(script);
}

void iniciar_proceso(char *path_proceso)
{
    t_pcb *pcb = crear_proceso();

    // chequear si hay multiprogramacion disponible
    // chequear_multiprogramacion();

    //enviar_pcb_a_cpu(pcb);

    // enviar proceso a memoria
    enviar_proceso_a_memoria(pcb->pid, path_proceso);
}

void finalizar_proceso(char *pid_string)
{
    //int pid = atoi(pid_string);
}

void iniciar_planificacion() {}
void detener_planificacion() {}

void cambiar_multiprogramacion(char *grado_multiprogramacion_string)
{
    //int numero = atoi(grado_multiprogramacion_string);
}

void mostrar_listado_estados_procesos()
{
    log_info(LOGGER_KERNEL, "Listado de procesos:");
    //mostrar_procesos_en_cola(colaNuevos, "NEW");
    //mostrar_procesos_en_cola(colaListos, "READY");
    //mostrar_procesos_en_cola(colaBloqueados, "BLOCKED");
    //mostrar_procesos_en_cola(colaTerminados, "FINISHED");
}

void mostrar_procesos_en_cola(t_queue *cola, const char *nombre_cola)
{
    if (queue_is_empty(cola))
    {
        log_info(LOGGER_KERNEL, "No hay procesos en la cola %s", nombre_cola);
    }
    else
    {
        log_info(LOGGER_KERNEL, "Procesos en estado %s:", nombre_cola);
        while (!queue_is_empty(cola))
        {
            t_pcb *proceso = queue_pop(cola);
            //log_info(LOGGER_KERNEL, "PID: %d - Estado: %s", proceso->pid, estado_to_string(proceso->estado));
            queue_push(cola, proceso);
        }
    }
}