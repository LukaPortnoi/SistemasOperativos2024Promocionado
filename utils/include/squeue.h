#ifndef SQUEUE_H_
#define SQUEUE_H_

#include "./gestor.h"
#include "./pcb.h"

typedef struct
{
    t_list *cola;
    pthread_mutex_t *mutex;
} t_squeue;

t_squeue *squeue_create();
void squeue_destroy(t_squeue *squeue);
void *squeue_pop(t_squeue *squeue);
void squeue_push(t_squeue *squeue, void *elemento);
void *squeue_peek(t_squeue *squeue);
void *squeue_remove_by_condition(t_squeue *squeue, bool (*condition)(void *));
void squeue_remove_element(t_squeue *squeue, void *elemento);

void mostrar_procesos_en_squeue(t_squeue *squeue, t_log *LOGGER);

#endif // SQUEUE_H_