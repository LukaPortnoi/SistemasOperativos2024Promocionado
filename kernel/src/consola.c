#include "../include/consola.h"

void iniciar_consola_interactiva()
{
  log_trace(LOGGER_KERNEL, "<CONSOLA INTERACTIVA>");
  inicializar_readline(); // Autocompletado con readline
  char *leido = readline("> ");

  while (leido != NULL)
  {
    add_history(leido);

    if (!strncmp(leido, "exit", 4))
    {
      free(leido);
      break;
    }

    if (!validar_comando(leido))
    {
      log_error(LOGGER_KERNEL, "Comando de CONSOLA no reconocido");
    }
    else
    {
      ejecutar_comando(leido);
    }

    free(leido);
    leido = readline("> ");
  }
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
    ejecutar_script(comando_separado[1]);
  }
  else if (strcmp(comando_separado[0], "INICIAR_PROCESO") == 0)
  {
    iniciar_proceso(comando_separado[1]);
  }
  else if (strcmp(comando_separado[0], "FINALIZAR_PROCESO") == 0)
  {
    finalizar_proceso_consola(comando_separado[1]);
  }
  else if (strcmp(comando_separado[0], "DETENER_PLANIFICACION") == 0)
  {
    detener_planificacion();
  }
  else if (strcmp(comando_separado[0], "INICIAR_PLANIFICACION") == 0)
  {
    iniciar_planificacion();
  }
  else if (strcmp(comando_separado[0], "MULTIPROGRAMACION") == 0)
  {
    cambiar_multiprogramacion(comando_separado[1]);
  }
  else if (strcmp(comando_separado[0], "PROCESO_ESTADO") == 0)
  {
    mostrar_listado_estados_procesos();
  }

  string_array_destroy(comando_separado);
}

// AUTOCOMPLETADO CON READLINE
char *comandos[] = {
    "EJECUTAR_SCRIPT",
    "INICIAR_PROCESO",
    "FINALIZAR_PROCESO",
    "DETENER_PLANIFICACION",
    "INICIAR_PLANIFICACION",
    "MULTIPROGRAMACION",
    "PROCESO_ESTADO",
    NULL};

char *comando_autocompletar(const char *text, int state)
{
  static int list_index, len;
  char *name;

  if (!state)
  {
    list_index = 0;
    len = strlen(text);
  }

  while ((name = comandos[list_index++]))
  {
    if (strncmp(name, text, len) == 0)
    {
      return strdup(name);
    }
  }

  return NULL;
}

char **consola_completar(const char *text, int start, int end)
{
  char **matches = NULL;

  if (start == 0)
  {
    matches = rl_completion_matches(text, comando_autocompletar);
  }

  return matches;
}

void inicializar_readline()
{
  rl_attempted_completion_function = consola_completar;
}

//-----------------------------------------------------------

void ejecutar_script(char *path_script)
{
  // POR AHORA DEJAR ESTE PATH COMO BASE
  char *path_archivo = string_new();
  string_append(&path_archivo, "/home/utnso/tp-2024-1c-OALP/kernel/scripts/");
  string_append(&path_archivo, path_script);

  FILE *script = fopen(path_archivo, "r");

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
      log_error(LOGGER_KERNEL, "Comando de CONSOLA no reconocido en script: %s", linea);
      continue;
    }

    ejecutar_comando(linea);
  }

  free(linea);
  fclose(script);
}

void iniciar_proceso(char *path_proceso)
{
  crear_proceso(path_proceso);
}

void finalizar_proceso_consola(char *pid_string)
{
  uint32_t pid = atoi(pid_string);

  // Si se esta ejecutando, enviar interrupcion a CPU
  if (pcb_ejecutandose != NULL && pcb_ejecutandose->pid == pid)
  {
    t_interrupcion *interrupcion = malloc(sizeof(t_interrupcion));
    interrupcion->motivo_interrupcion = INTERRUPCION_FINALIZACION;
    interrupcion->pid = pid;
    enviar_interrupcion(fd_kernel_cpu_interrupt, interrupcion);
    free(interrupcion);
  }
  else
  {
    t_pcb *proceso = buscar_proceso_en_colas(pid);

    if (proceso == NULL)
    {
      log_error(LOGGER_KERNEL, "No se encontro el proceso con PID: %d", pid);
      return;
    }

    if (proceso->estado == NUEVO)
    {
      squeue_remove_element(squeue_new, proceso);
    }
    else if (proceso->estado == LISTO)
    {
      squeue_remove_element(squeue_ready, proceso);
    }
    else if (proceso->estado == BLOQUEADO)
    {
      squeue_remove_element(squeue_blocked, proceso);
      pcb_a_finalizar = proceso;
      proceso->contexto_ejecucion->motivo_finalizacion = INTERRUPTED_BY_USER;
    }

    if (proceso->estado != FINALIZADO)
    {
      proceso->contexto_ejecucion->motivo_finalizacion = INTERRUPTED_BY_USER;
      if (proceso->estado == NUEVO)
      {
        // FINALIZAR_PROCESO sin post a multiprogramacion
        log_info(LOGGER_KERNEL, "Finaliza el proceso %d - Motivo: %s", proceso->pid, motivo_finalizacion_to_string(proceso->contexto_ejecucion->motivo_finalizacion));
        cambiar_estado_pcb(proceso, FINALIZADO);
        squeue_push(squeue_exit, proceso);
        if (list_size(proceso->recursos_asignados) > 0)
        {
          for (int i = 0; i < list_size(proceso->recursos_asignados); i++)
          {
            liberar_recurso(proceso, list_get(proceso->recursos_asignados, i));
          }
        }
        // liberar_estructuras_memoria(proceso->pid); // TODO: Liberar estructuras de memoria
      }
      else
      {
        finalizar_proceso(proceso);
      }
    }
    else
    {
      log_warning(LOGGER_KERNEL, "El proceso ya finalizo");
    }
  }
}

t_pcb *buscar_proceso_en_colas(uint32_t pid)
{
  t_pcb *proceso = NULL;

  proceso = buscar_proceso_en_cola(squeue_new, pid);
  if (proceso != NULL)
    return proceso;

  proceso = buscar_proceso_en_cola(squeue_ready, pid);
  if (proceso != NULL)
    return proceso;

  proceso = buscar_proceso_en_cola(squeue_blocked, pid);
  if (proceso != NULL)
    return proceso;

  proceso = buscar_proceso_en_cola(squeue_exit, pid);
  if (proceso != NULL)
    return proceso;

  return NULL;
}

t_pcb *buscar_proceso_en_cola(t_squeue *squeue, uint32_t pid)
{
  t_pcb *proceso = NULL;

  for (int i = 0; i < list_size(squeue->cola); i++)
  {
    t_pcb *proceso_actual = list_get(squeue->cola, i);

    if (proceso_actual->pid == pid)
    {
      proceso = proceso_actual;
      break;
    }
  }

  return proceso;
}

void detener_planificacion()
{
  detener_planificadores();
}
void iniciar_planificacion()
{
  iniciar_planificadores();
}

void cambiar_multiprogramacion(char *grado_multiprogramacion_string)
{
  int numero = atoi(grado_multiprogramacion_string);
  GRADO_MULTIPROGRAMACION = numero;
  log_debug(LOGGER_KERNEL, "Grado de multiprogramacion cambiado a: %d", GRADO_MULTIPROGRAMACION);
  sem_init(&semMultiprogramacion, 0, GRADO_MULTIPROGRAMACION);
}

void mostrar_listado_estados_procesos()
{
  log_info(LOGGER_KERNEL, "Listado de procesos:");
  mostrar_procesos_en_cola(squeue_new, "NEW");
  mostrar_procesos_en_cola(squeue_ready, "READY");
  mostrar_procesos_en_cola(squeue_exec, "EXECUTING");
  mostrar_procesos_en_cola(squeue_blocked, "BLOCKED");
  mostrar_procesos_en_cola(squeue_exit, "FINISHED");
}

void mostrar_procesos_en_cola(t_squeue *squeue, const char *nombre_cola)
{
  if (list_is_empty(squeue->cola))
  {
    log_info(LOGGER_KERNEL, "No hay procesos en la cola %s", nombre_cola);
  }
  else
  {
    log_info(LOGGER_KERNEL, "Procesos en la cola %s:", nombre_cola);
    mostrar_procesos_en_squeue(squeue, LOGGER_KERNEL);
  }
}