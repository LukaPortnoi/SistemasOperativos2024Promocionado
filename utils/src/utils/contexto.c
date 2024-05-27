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
	case FINALIZACION:
		return "FINALIZACION";
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

char *nombre_instruccion_to_string(nombre_instruccion instruccion)
{
	switch (instruccion)
	{
	case IO_GEN_SLEEP:
		return "IO_GEN_SLEEP";
	case IO_STDIN_READ:
		return "IO_STDIN_READ";
	case IO_STDOUT_WRITE:
		return "IO_STDOUT_WRITE";
	case IO_FS_CREATE:
		return "IO_FS_CREATE";
	case IO_FS_DELETE:
		return "IO_FS_DELETE";
	case IO_FS_TRUNCATE:
		return "IO_FS_TRUNCATE";
	case IO_FS_WRITE:
		return "IO_FS_WRITE";
	case IO_FS_READ:
		return "IO_FS_READ";
	case EXIT:
		return "EXIT";
	default:
		return "ERROR";
	}
}