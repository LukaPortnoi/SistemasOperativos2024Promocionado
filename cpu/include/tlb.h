#ifndef TLB_H_
#define TLB_H_

#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <commons/log.h>
#include <commons/collections/list.h>

typedef enum {
    LRU,
    FIFO
} algoritmos_tlb;

#endif // TLB_H_