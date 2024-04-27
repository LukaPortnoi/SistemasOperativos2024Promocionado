#ifndef COMUNICACIONES_H_
#define COMUNICACIONES_H_

#include <stdlib.h>
#include <stdio.h>

#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <inttypes.h>
#include <readline/readline.h>

#include "../../utils/include/hello.h"
#include "../../utils/include/sockets_client.h"
#include "../../utils/include/sockets_server.h"

int server_escuchar(t_log *logger, char *server_name, int server_socket);

#endif