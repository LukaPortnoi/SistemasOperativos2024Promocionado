#include "../include/main.h"

char *PUERTO_ESCUCHA_MEMORIA;
char *IP_MEMORIA;
int TAM_MEMORIA;
int TAM_PAGINA;
char *PATH_INSTRUCCIONES;
int RETARDO_RESPUESTA;
int CLIENTE_CPU, CLIENTE_KERNEL, CLIENTE_IN_OU;

t_log *LOGGER_MEMORIA;
t_config *CONFIG_MEMORIA;

int fd_memoria;

pthread_mutex_t mutex_comunicacion_procesos;
pthread_mutex_t mutex_procesos;

void *memoriaUsuario;
uint32_t tamanioMemoria;

t_list *tablaPaginas;
t_list *marcosPaginas;
t_list *procesos_totales;

int main(void)
{
	inicializar_config();
	iniciar_semaforos();
	iniciar_memoria_usuario();

	fd_memoria = iniciar_servidor(LOGGER_MEMORIA, "MEMORIA", IP_MEMORIA, PUERTO_ESCUCHA_MEMORIA);

	while (server_escuchar(LOGGER_MEMORIA, "MEMORIA", fd_memoria));

	terminar_programa(fd_memoria, LOGGER_MEMORIA, CONFIG_MEMORIA);

	return 0;
}

void inicializar_config()
{
	LOGGER_MEMORIA = iniciar_logger("memoria.log", "Servidor Memoria");
	CONFIG_MEMORIA = iniciar_config("./memoria.config", "MEMORIA");
	PUERTO_ESCUCHA_MEMORIA = config_get_string_value(CONFIG_MEMORIA, "PUERTO_ESCUCHA");
	TAM_MEMORIA = config_get_int_value(CONFIG_MEMORIA, "TAM_MEMORIA");
	TAM_PAGINA = config_get_int_value(CONFIG_MEMORIA, "TAM_PAGINA");
	PATH_INSTRUCCIONES = config_get_string_value(CONFIG_MEMORIA, "PATH_INSTRUCCIONES");
	RETARDO_RESPUESTA = config_get_int_value(CONFIG_MEMORIA, "RETARDO_RESPUESTA");
	IP_MEMORIA = config_get_string_value(CONFIG_MEMORIA, "IP_MEMORIA");
}

void iniciar_memoria_usuario()
{
	memoriaUsuario = malloc(TAM_MEMORIA);
	memset(memoriaUsuario, 0, TAM_MEMORIA);
	tamanioMemoria = TAM_MEMORIA;
	if (memoriaUsuario == NULL)
	{
		log_error(LOGGER_MEMORIA, "Error al reservar memoria para el usuario");
		exit(EXIT_FAILURE);
	}

	log_debug(LOGGER_MEMORIA, "Memoria reservada para el usuario");
	
	tablaPaginas = list_create();
	marcosPaginas = list_create();
	procesos_totales = list_create();

	iniciar_marcos();
}

void iniciar_marcos()
{
	uint32_t cantidadMarcos = TAM_MEMORIA / TAM_PAGINA;
	for (int i = 0; i < cantidadMarcos; i++)
	{
		// Creo un marco de pagina y todas esas cosas
	}
}