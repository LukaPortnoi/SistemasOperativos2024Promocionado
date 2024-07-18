#ifndef DIAlFS_H_
#define DIAlFS_H_

#include "./gestor.h"

t_archivo *crear_archivo(char *nombre, uint32_t bloque_inicial, uint32_t tamanio);
void destruir_archivo(t_archivo *archivo);
void eliminar_archivo_por_nombre(char *nombre);
void actualizar_archivo_en_lista(char *nombre, uint32_t bloque_inicial, uint32_t tamanio);

void manejar_archivos_fs();
void crear_directorio_si_no_existe(const char *path);
void actualizar_lista_archivos_en_fs();

void actualizar_bloque_inicial(t_config *metadata_config, uint32_t bloque_inicial);
void actualizar_tamanio_archivo(t_config *metadata_config, uint32_t tamanio);

uint32_t obtener_bloque_inicial(char path[]);
uint32_t obtener_tamanio_archivo(char path[]);
uint32_t obtener_bloques_ocupados(char path[]);

void obtener_metadata_path(char *nombre_archivo, char *metadata_path, size_t size);

int encontrar_bloque_libre();
int encontrar_bloques_libres_contiguos(uint32_t bloque_inicial, uint32_t bloques_necesarios, char *bitmap);

uint32_t encontrar_bloques_libres_contiguos_top(uint32_t bloque_inicial, uint32_t bloques_necesarios, uint32_t bloques_ocupados, char *bitmap);
uint32_t contar_bloques_libres(char *bitmap);

void compactar_dialfs(uint32_t pid);
void ordenar_lista_archivos_por_bloque_inicial();
void actualizar_lista_archivos_compactados();
void escribir_dato_archivo(char *datoRecibido, char *puntero_archivo, char *bloques, uint32_t bloque_inicial);
char *leer_dato_archivo(uint32_t tamanio, char *puntero_archivo, char *bloques, uint32_t bloque_inicial);

void imprimir_bitmap();
uint32_t obtener_bloque_inicial_por_nombre(char *nombre);

#endif // DIAlFS_H_