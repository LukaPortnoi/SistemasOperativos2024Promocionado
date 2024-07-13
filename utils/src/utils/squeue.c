#include "../include/squeue.h"

t_squeue *squeue_create()
{
    t_squeue *squeue = malloc(sizeof(t_squeue));
    squeue->cola = list_create();
    squeue->mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(squeue->mutex, NULL);
    return squeue;
}

void squeue_destroy(t_squeue *squeue)
{
    list_destroy(squeue->cola);
    pthread_mutex_destroy(squeue->mutex);
    free(squeue->mutex);
    free(squeue);
}

void *squeue_pop(t_squeue *squeue)
{
    void *elemento;
    pthread_mutex_lock(squeue->mutex);
    elemento = list_remove(squeue->cola, 0);
    pthread_mutex_unlock(squeue->mutex);
    return elemento;
}

void squeue_push(t_squeue *squeue, void *elemento)
{
    pthread_mutex_lock(squeue->mutex);
    list_add(squeue->cola, elemento);
    pthread_mutex_unlock(squeue->mutex);
}

void *squeue_peek(t_squeue *squeue)
{
    void *elemento;
    pthread_mutex_lock(squeue->mutex);
    elemento = list_get(squeue->cola, 0);
    pthread_mutex_unlock(squeue->mutex);
    return elemento;
}

void *squeue_remove_by_condition(t_squeue *squeue, bool (*condition)(void *))
{
    void *elemento = NULL;
    pthread_mutex_lock(squeue->mutex);
    elemento = list_remove_by_condition(squeue->cola, condition);
    pthread_mutex_unlock(squeue->mutex);
    return elemento;
}

void squeue_remove_element(t_squeue *squeue, void *elemento)
{
    pthread_mutex_lock(squeue->mutex);
    list_remove_element(squeue->cola, elemento);
    pthread_mutex_unlock(squeue->mutex);
}

void mostrar_procesos_en_squeue(t_squeue *squeue, t_log *LOGGER)
{
    pthread_mutex_lock(squeue->mutex);
    for (int i = 0; i < list_size(squeue->cola); i++)
    {
        t_pcb *pcb = list_get(squeue->cola, i);
        log_info(LOGGER, "PID: %d", pcb->pid);
    }
    pthread_mutex_unlock(squeue->mutex);
}