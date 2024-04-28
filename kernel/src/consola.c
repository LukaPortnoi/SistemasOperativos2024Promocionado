#include "../include/consola.h"

void iniciar_consola_interactiva()
{   
    log_trace(LOGGER_KERNEL, "<CONSOLA INTERACTIVA>");
    char *leido;
    leido = readline("> ");

    while (strcmp(leido, ""))
    {
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

    if (strcmp(comando_separado[0], "EJECUTAR_SCRIPT") == 0)
    {
        es_comando_valido = true;
    }
    else if (strcmp(comando_separado[0], "INICIAR_PROCESO") == 0)
    {
        es_comando_valido = true;
    }
    else if (strcmp(comando_separado[0], "FINALIZAR_PROCESO") == 0)
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
    else if (strcmp(comando_separado[0], "MULTIPROGRAMACION") == 0)
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
        // ejecutar_script(comando_separado[1]);
    }
    else if (strcmp(comando_separado[0], "INICIAR_PROCESO") == 0)
    {
        log_info(LOGGER_KERNEL, "Iniciando proceso desde %s...", comando_separado[1]);
        iniciar_proceso(comando_separado[1]);
    }
    else if (strcmp(comando_separado[0], "FINALIZAR_PROCESO") == 0)
    {
        log_info(LOGGER_KERNEL, "Finalizando proceso %s...", comando_separado[1]);
        // finalizar_proceso(comando_separado[1]);
    }
    else if (strcmp(comando_separado[0], "DETENER_PLANIFICACION") == 0)
    {
        log_info(LOGGER_KERNEL, "Deteniendo planificacion...");
        // detener_planificacion();
    }
    else if (strcmp(comando_separado[0], "INICIAR_PLANIFICACION") == 0)
    {
        log_info(LOGGER_KERNEL, "Iniciando planificacion...");
        // iniciar_planificacion();
    }
    else if (strcmp(comando_separado[0], "MULTIPROGRAMACION") == 0)
    {
        log_info(LOGGER_KERNEL, "Cambiando grado de multiprogramacion a %s...", comando_separado[1]);
        // cambiar_multiprogramacion(comando_separado[1]);
    }
    else if (strcmp(comando_separado[0], "PROCESO_ESTADO") == 0)
    {
        log_info(LOGGER_KERNEL, "Estado del proceso %s...", comando_separado[1]);
        // mostrar_listado_estados_procesos(comando_separado[1]);
    }

    string_array_destroy(comando_separado);
}

/*void ejecutar_script(char *path_script)
{
    FILE *script = fopen(path_script, "r");

    if (script == NULL)
    {
        log_error(LOGGER_KERNEL_KERNEL, "No se pudo abrir el archivo %s", path_script);
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
            log_error(LOGGER_KERNEL_KERNEL, "Comando de CONSOLA no reconocido");
            continue;
        }

        ejecutar_comando(linea, LOGGER_KERNEL_KERNEL);
    }

    free(linea);
    fclose(script);
}*/

void iniciar_proceso(char *path_proceso)
{   
    int pid_nuevo = asignar_pid();
    enviar_proceso_a_memoria(pid_nuevo, path_proceso);
    t_pcb *pcb = crear_pcb(pid_nuevo, NEW, QUANTUM);
}