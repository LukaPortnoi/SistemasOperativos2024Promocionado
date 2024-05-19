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