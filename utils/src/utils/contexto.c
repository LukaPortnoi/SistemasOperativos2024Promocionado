#include "../include/contexto.h"

char *motivo_desalojo_to_string(t_motivo_desalojo motivo)
{
	switch (motivo)
	{
	case SIN_MOTIVO:
		return "SIN MOTIVO";
	case INTERRUPCION_FIN_QUANTUM:
		return "INTERRUPCION FIN QUANTUM";
	case INTERRUPCION_BLOQUEO:
		return "INTERRUPCION BLOQUEO";
	case INTERRUPCION_FINALIZACION:
		return "INTERRUPCION FINALIZACION";
	case INTERRUPCION_ERROR:
		return "INTERRUPCION ERROR";
	case INTERRUPCION_SYSCALL:
		return "INTERRUPCION SYSCALL";
	default:
		return "ERROR";
	}
}

char *motivo_finalizacion_to_string(t_motivo_finalizacion motivo)
{
	switch (motivo)
	{
	case SUCCESS:
		return "SUCCESS";
	case INVALID_RESOURCE:
		return "INVALID_RESOURCE";
	case INVALID_INTERFACE:
		return "INVALID_INTERFACE";
	case OUT_OF_MEMORY:
		return "OUT_OF_MEMORY";
	case INTERRUPTED_BY_USER:
		return "INTERRUPTED_BY_USER";
	default:
		return "ERROR";
	}
}