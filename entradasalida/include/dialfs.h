#ifndef DIAlFS_H_
#define DIAlFS_H_

#include "./gestor.h"

void manejar_archivos_fs();
void actualizar_bloque_inicial(t_config *metadata_config, uint32_t bloque_inicial);
void actualizar_tamanio_archivo(t_config *metadata_config, uint32_t tamanio);
uint32_t obtener_bloque_inicial(char path[]);
uint32_t obtener_tamanio_archivo(char path[]);
char *obtener_metadata_path(t_interfaz_dialfs *interfazRecibida, char *metadata_path, size_t size);
uint32_t encontrar_bloque_libre();
uint32_t encontrar_bloques_libres_contiguos(uint32_t bloque_inicial, uint32_t bloques_necesarios);
uint32_t contar_bloques_libres(char *bitmap);
void compactar_dialfs(uint32_t pid);

#endif // DIAlFS_H_