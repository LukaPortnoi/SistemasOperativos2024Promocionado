#include "../include/consola.h"

void iniciar_consola_interactiva()
{
  log_trace(LOGGER_KERNEL, "<CONSOLA INTERACTIVA>");
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

  // enviar pcb a cpu
  enviar_pcb(pcb, fd_kernel_cpu_dispatch);

  // enviar proceso a memoria
  enviar_proceso_a_memoria(pcb->pid, path_proceso);
}

void finalizar_proceso(char *pid_string)
{
  int pid = atoi(pid_string);

  if (pcb_ejecutandose != NULL && pcb_ejecutandose->pid == pid)
  {
    // debemos mandar una interrupcion a la cpu
    t_interrupcion *interrupcion = malloc(sizeof(t_interrupcion));
    interrupcion->motivo_interrupcion = INTERRUPCION_FINALIZACION;
    interrupcion->pid = pid;
    enviar_interrupcion(fd_kernel_cpu_interrupt, interrupcion);
  }

  t_pcb *proceso = buscar_proceso_en_colas(pid);

  if (proceso->estado == NUEVO)
  {
    squeue_remove(squeue_new, proceso);
  }
  else if (proceso->estado == LISTO)
  {
    squeue_remove(squeue_ready, proceso);
  }
  else if (proceso->estado == EJECUTANDO)
  {
    squeue_remove(squeue_exec, proceso);
  }
  else if (proceso->estado == BLOQUEADO)
  {
    squeue_remove(squeue_blocked, proceso);
  }

  squeue_push(squeue_exit, proceso);
  cambiar_estado_pcb(proceso, FINALIZADO);

  sem_post(&semFinalizado);
}

t_pcb *buscar_proceso_en_colas(int pid) // TODO: revisar q no este haciendo cagadas
{
  t_pcb *proceso = NULL;

  if (!queue_is_empty(squeue_new->cola))
  {
    proceso = buscar_proceso_en_cola(squeue_new, pid);
  }

  if (proceso == NULL && !queue_is_empty(squeue_ready->cola))
  {
    proceso = buscar_proceso_en_cola(squeue_ready, pid);
  }

  if (proceso == NULL && !queue_is_empty(squeue_exec->cola))
  {
    proceso = buscar_proceso_en_cola(squeue_exec, pid);
  }

  if (proceso == NULL && !queue_is_empty(squeue_block->cola))
  {
    proceso = buscar_proceso_en_cola(squeue_block, pid);
  }

  return proceso;
}

t_pcb *buscar_proceso_en_cola(t_squeue *squeue, int pid)
{
  t_pcb *proceso = NULL;
  t_list *temp = list_create();

  while (!queue_is_empty(squeue->cola))
  {
    t_pcb *proceso_actual = squeue_pop(squeue);
    list_add(temp, proceso_actual);

    if (proceso_actual->pid == pid)
    {
      proceso = proceso_actual;
    }
  }

  for (int i = 0; i < list_size(temp); i++)
  {
    squeue_push(squeue, list_get(temp, i));
  }

  list_destroy(temp);
  return proceso;
}

void iniciar_planificacion() {}
void detener_planificacion() {}

void cambiar_multiprogramacion(char *grado_multiprogramacion_string)
{
  int numero = atoi(grado_multiprogramacion_string);
}

void mostrar_listado_estados_procesos()
{
  log_info(LOGGER_KERNEL, "Listado de procesos:");
  mostrar_procesos_en_cola(squeue_new, "NEW");
  mostrar_procesos_en_cola(squeue_ready, "READY");
  mostrar_procesos_en_cola(squeue_block, "BLOCKED");
  mostrar_procesos_en_cola(squeue_exit, "FINISHED");
}

void mostrar_procesos_en_cola(t_squeue *squeue, const char *nombre_cola)
// Yo creo que esta funcion no esta bien,
// cada vez que yo haga un pop de una cola, deberia pushearla de nuevo
// porque sino se pierden los procesos :)
{
  if (queue_is_empty(squeue->cola))
  {
    log_info(LOGGER_KERNEL, "No hay procesos en la cola %s", nombre_cola);
  }
  else
  {
    log_info(LOGGER_KERNEL, "Procesos en estado %s:", nombre_cola);
    while (!queue_is_empty(squeue->cola))
    {
      t_pcb *proceso = squeue_pop(squeue);
      log_info(LOGGER_KERNEL, "PID: %d - Estado: %s", proceso->pid, estado_to_string(proceso->estado));
      squeue_push(squeue, proceso);
    }
  }
}