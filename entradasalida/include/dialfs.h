#ifndef DIAlFS_H_
#define DIAlFS_H_

#include "./gestor.h"

void manejar_archivos_fs();
void actualizar_bloque_inicial(t_config *metadata_config, uint32_t bloque_inicial);
void actualizar_tamanio_archivo(t_config *metadata_config, uint32_t tamanio);
uint32_t encontrar_bloque_libre();
uint32_t obtener_bloque_inicial(char path[]);

#endif // DIAlFS_H_