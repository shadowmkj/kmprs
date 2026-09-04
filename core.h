#pragma once

#include <stdint.h>

typedef struct {
    uint8_t symbol;
    uint64_t frequency;
    float probability;
} SymbolFreq;
