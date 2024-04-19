#ifndef SOCKETS_COMMON_H_
#define SOCKETS_COMMON_H_
 typedef enum 
{
    HANDSHAKE_consola,
    HANDSHAKE_kernel,
    HANDSHAKE_memoria,
    HANDSHAKE_cpu,
    HANDSHAKE_interrupt,
    HANDSHAKE_dispatch,
    HANDSHAKE_in_out,
    HANDSHAKE_ok_continue,
    MENSAJE,
    PAQUETE
} t_handshake;

#endif // SOCKETS_COMMON_H_