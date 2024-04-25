#include "../include/consola.h"

void iniciar_consola_interactiva(t_log *logger)
{
    char *leido;
    leido = readline("> ");

    while (strcmp(leido, ""))
    {
        if (!validar_comando(leido))
        {
            log_error(logger, "Comando de CONSOLA no reconocido");
            free(leido);
            leido = readline("> ");
            continue;
        }

        ejecutar_comando(leido, logger);
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

void ejecutar_comando(char *comando, t_log *logger)
{
    char **comando_separado = string_split(comando, " ");

    if (strcmp(comando_separado[0], "EJECUTAR_SCRIPT") == 0)
    {
        log_info(logger, "Ejecutando script %s...", comando_separado[1]);
    }
    else if (strcmp(comando_separado[0], "INICIAR_PROCESO") == 0)
    {
        log_info(logger, "Iniciando proceso %s...", comando_separado[1]);
    }
    else if (strcmp(comando_separado[0], "FINALIZAR_PROCESO") == 0)
    {
        log_info(logger, "Finalizando proceso %s...", comando_separado[1]);
    }
    else if (strcmp(comando_separado[0], "DETENER_PLANIFICACION") == 0)
    {
        log_info(logger, "Deteniendo planificacion...");
    }
    else if (strcmp(comando_separado[0], "INICIAR_PLANIFICACION") == 0)
    {
        log_info(logger, "Iniciando planificacion...");
    }
    else if (strcmp(comando_separado[0], "MULTIPROGRAMACION") == 0)
    {
        log_info(logger, "Cambiando grado de multiprogramacion a %s...", comando_separado[1]);
    }
    else if (strcmp(comando_separado[0], "PROCESO_ESTADO") == 0)
    {
        log_info(logger, "Estado del proceso %s...", comando_separado[1]);
    }

    string_array_destroy(comando_separado);
}