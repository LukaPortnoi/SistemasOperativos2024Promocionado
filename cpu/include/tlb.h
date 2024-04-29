#ifndef TLB_H
#define TLB_H

#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <commons/log.h>
#include <commons/collections/list.h>

typedef enum{
    LRU,
    FIFO
}algoritmos_tlb;