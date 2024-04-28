#include "../include/sockets_utils.h"

t_log *iniciar_logger(char *file_name, char *name)
{
	t_log *nuevo_logger;
	nuevo_logger = log_create(file_name, name, 1, LOG_LEVEL_TRACE);
	if (nuevo_logger == NULL)
	{
		printf("No se pudo crear el logger %s\n", name);
		exit(1);
	};
	return nuevo_logger;
}

t_config *iniciar_config(char *file_name, char *name)
{
	t_config *nuevo_config;
	nuevo_config = config_create(file_name);
	if (nuevo_config == NULL)
	{
		printf("No se pudo leer la config %s\n", name);
		exit(2);
	};
	return nuevo_config;
}

t_paquete *crear_paquete_con_codigo_de_operacion(op_cod codigo)
{
	t_paquete *paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = codigo;
	crear_buffer(paquete);
	return paquete;
}

void serializar_nuevo(t_paquete *paquete, int pid, int size, char *path)
{
    paquete->buffer->size += sizeof(uint32_t) * 3 +  strlen(path) + 1;

    paquete->buffer->stream = malloc(paquete->buffer->size);

    int desplazamiento = 0;

    memcpy(paquete->buffer->stream + desplazamiento, &(pid), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, &(size), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    uint32_t long_path = strlen(path) + 1;
    memcpy(paquete->buffer->stream + desplazamiento, &(long_path), sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(paquete->buffer->stream + desplazamiento, path, long_path);
}

void terminar_programa(int conexion, t_log *logger, t_config *config)
{
	liberar_conexion(conexion);
	log_destroy(logger);
	config_destroy(config);
}