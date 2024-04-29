#include "../include/contexto.h"

void enviar_contexto(int socket, t_contexto_ejecucion *contexto_a_enviar)
{
	t_paquete *paquete = crear_paquete_con_codigo_de_operacion(CONTEXTO);
	serializar_contexto(paquete, contexto_a_enviar);
	enviar_paquete(paquete, socket);
	eliminar_paquete(paquete);
}

void serializar_contexto(t_paquete *paquete, t_contexto_ejecucion *ctx)
{

	paquete->buffer->size = sizeof(int) * 2 +
							sizeof(t_registros) +
							sizeof(nombre_instruccion) +
							sizeof(uint32_t) * 3 +
							ctx->instruccion_ejecutada->longitud_parametro1 +
							ctx->instruccion_ejecutada->longitud_parametro2;
							// sizeof(motivo_desalojo);

	// printf("Size del stream a serializar: %d \n", paquete->buffer->size); // TODO - BORRAR LOG
	paquete->buffer->stream = malloc(paquete->buffer->size);

	int desplazamiento = 0;

	memcpy(paquete->buffer->stream + desplazamiento, &(ctx->pid), sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(paquete->buffer->stream + desplazamiento, &(ctx->program_counter), sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(paquete->buffer->stream + desplazamiento, ctx->registros, sizeof(t_registros));
	desplazamiento += sizeof(t_registros);

	/* memcpy(paquete->buffer->stream + desplazamiento, &(ctx->numero_marco), sizeof(int));
	desplazamiento += sizeof(int);

	memcpy(paquete->buffer->stream + desplazamiento, &(ctx->nro_pf), sizeof(int));
	desplazamiento += sizeof(int); */

	memcpy(paquete->buffer->stream + desplazamiento, &(ctx->instruccion_ejecutada->codigo), sizeof(nombre_instruccion));
	desplazamiento += sizeof(nombre_instruccion);

	memcpy(paquete->buffer->stream + desplazamiento, &(ctx->instruccion_ejecutada->longitud_parametro1), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(paquete->buffer->stream + desplazamiento, &(ctx->instruccion_ejecutada->longitud_parametro2), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(paquete->buffer->stream + desplazamiento, &(ctx->instruccion_ejecutada->pid), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);

	memcpy(paquete->buffer->stream + desplazamiento, ctx->instruccion_ejecutada->parametro1, ctx->instruccion_ejecutada->longitud_parametro1);
	desplazamiento += ctx->instruccion_ejecutada->longitud_parametro1;

	memcpy(paquete->buffer->stream + desplazamiento, ctx->instruccion_ejecutada->parametro2, ctx->instruccion_ejecutada->longitud_parametro2);
	desplazamiento += ctx->instruccion_ejecutada->longitud_parametro2;

	/*memcpy(paquete->buffer->stream + desplazamiento, &(ctx->codigo_ultima_instru), sizeof(nombre_instruccion));
	desplazamiento += sizeof(nombre_instruccion);

	 memcpy(paquete->buffer->stream + desplazamiento, &(ctx->motivo_desalojado), sizeof(motivo_desalojo));
	desplazamiento += sizeof(motivo_desalojo);*/
}

t_contexto_ejecucion *recibir_contexto(int socket)
{
	int size;
	void *buffer;

	buffer = recibir_buffer(&size, socket);
	// printf("Size del stream a deserializar: %d \n", size);

	t_contexto_ejecucion *contexto_recibido = malloc(sizeof(t_contexto_ejecucion));

	int offset = 0;

	memcpy(&(contexto_recibido->pid), buffer + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(contexto_recibido->program_counter), buffer + offset, sizeof(int));
	offset += sizeof(int);

	contexto_recibido->registros = malloc(sizeof(t_registros));
	memcpy(contexto_recibido->registros, buffer + offset, sizeof(t_registros));
	offset += sizeof(t_registros);

	/* memcpy(&(contexto_recibido->numero_marco), buffer + offset, sizeof(int));
	offset += sizeof(int);
	memcpy(&(contexto_recibido->nro_pf), buffer + offset, sizeof(int));
	offset += sizeof(int); */

	contexto_recibido->instruccion_ejecutada = malloc(sizeof(t_instruccion));

	memcpy(&(contexto_recibido->instruccion_ejecutada->codigo), buffer + offset, sizeof(nombre_instruccion));
	offset += sizeof(nombre_instruccion);
	memcpy(&(contexto_recibido->instruccion_ejecutada->longitud_parametro1), buffer + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(contexto_recibido->instruccion_ejecutada->longitud_parametro2), buffer + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(&(contexto_recibido->instruccion_ejecutada->pid), buffer + offset, sizeof(uint32_t));
	offset += sizeof(uint32_t);

	contexto_recibido->instruccion_ejecutada->parametro1 = malloc(contexto_recibido->instruccion_ejecutada->longitud_parametro1);
	memcpy(contexto_recibido->instruccion_ejecutada->parametro1, buffer + offset, contexto_recibido->instruccion_ejecutada->longitud_parametro1);
	offset += contexto_recibido->instruccion_ejecutada->longitud_parametro1;

	contexto_recibido->instruccion_ejecutada->parametro2 = malloc(contexto_recibido->instruccion_ejecutada->longitud_parametro2);
	memcpy(contexto_recibido->instruccion_ejecutada->parametro2, buffer + offset, contexto_recibido->instruccion_ejecutada->longitud_parametro2);
	offset += contexto_recibido->instruccion_ejecutada->longitud_parametro2;

	memcpy(&(contexto_recibido->codigo_ultima_instru), buffer + offset, sizeof(nombre_instruccion));
	offset += sizeof(nombre_instruccion);

	/*memcpy(&(contexto_recibido->motivo_desalojado), buffer + offset, sizeof(motivo_desalojo));
	offset += sizeof(motivo_desalojo);*/

	free(buffer);

	return contexto_recibido;
}
