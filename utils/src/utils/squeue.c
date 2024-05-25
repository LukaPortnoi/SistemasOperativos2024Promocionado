#include "../include/squeue.h"

t_squeue *squeue_create()
{
    t_squeue *squeue = malloc(sizeof(t_squeue));
    squeue->cola = queue_create();
    squeue->mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(squeue->mutex, NULL);
    return squeue;
}

void squeue_destroy(t_squeue *squeue)
{
    queue_destroy(squeue->cola);
    pthread_mutex_destroy(squeue->mutex);
    free(squeue);
}

void *squeue_pop(t_squeue *squeue)
{
    void *elemento;
    pthread_mutex_lock(squeue->mutex);
    elemento = queue_pop(squeue->cola);
    pthread_mutex_unlock(squeue->mutex);
    return elemento;
}

void squeue_push(t_squeue *squeue, void *elemento)
{
    /* if (squeue == squeue_ready)
    {
        log_info(LOGGER_KERNEL, "Ingreso a Cola Ready:");
        mostrar_procesos_en_squeue(squeue);
    } */
    // COLA READY PLUS DEL VRR
    /* if (squeue == squeue_readyPlus) {
        log_info(LOGGER_KERNEL, "Ingreso a Cola Ready+:);
        mostrar_procesos_en_squeue(squeue);
    } */
    pthread_mutex_lock(squeue->mutex);
    queue_push(squeue->cola, elemento);
    pthread_mutex_unlock(squeue->mutex);
}

void *squeue_peek(t_squeue *squeue)
{
    void *elemento;
    pthread_mutex_lock(squeue->mutex);
    elemento = queue_peek(squeue->cola);
    pthread_mutex_unlock(squeue->mutex);
    return elemento;
}

void mostrar_procesos_en_squeue(t_squeue *squeue, t_log *LOGGER)
{
    t_list *temp_list = list_create();

    if (queue_is_empty(squeue->cola))
    {
        log_info(LOGGER, "No hay procesos en la cola");
    }
    else
    {
        while (!queue_is_empty(squeue->cola))
        {
            t_pcb *proceso = squeue_pop(squeue);
            list_add(temp_list, proceso);
        }

        for (int i = 0; i < list_size(temp_list); i++)
        {
            t_pcb *proceso = list_get(temp_list, i);
            log_info(LOGGER, "PID: %d", proceso->pid);
        }
    }
}