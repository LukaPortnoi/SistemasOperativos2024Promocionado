#ifndef GESTOR_H_
#define GESTOR_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <readline/readline.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>
#include <dirent.h>
#include <sys/stat.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>

#include "../../utils/include/sockets_server.h"
#include "../../utils/include/sockets_client.h"
#include "../../utils/include/sockets_utils.h"
#include "../../utils/include/IO.h"

typedef struct
{
    char *nombre;
    uint32_t bloque_inicial;
    uint32_t tamanio;
} t_archivo;

extern char *TIPO_INTERFAZ;
extern int TIEMPO_UNIDAD_TRABAJO;
extern char *IP_KERNEL;
extern char *PUERTO_KERNEL;
extern char *IP_MEMORIA;
extern char *PUERTO_MEMORIA;
extern char *PATH_BASE_DIALFS;
extern int BLOCK_SIZE;
extern int BLOCK_COUNT;
extern int RETRASO_COMPACTACION;

extern int fd_io_memoria;
extern int fd_io_kernel;

extern t_log *LOGGER_INPUT_OUTPUT;
extern t_config *CONFIG_INPUT_OUTPUT;

extern char BITMAP_PATH[256];
extern char BLOQUES_PATH[256];
extern t_list *ARCHIVOS_EN_FS;

#endif /* GESTOR_H_ */