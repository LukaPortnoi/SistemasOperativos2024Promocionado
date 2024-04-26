#include "../include/planificador.h"

void iniciar_planificador_largo_plazo()
{
    pthread_t hilo_planificador;
    pthread_create(&hilo_planificador, NULL, (void*)grado_multiprogamacion, NULL);
    pthread_detach(hilo_planificador);
}

void grado_multiprogamacion(){}
