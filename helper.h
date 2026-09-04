#ifndef HELPER_H
#define HELPER_H

#include "core.h"
#include <stddef.h>
#include <stdint.h>

// freq is an array of size 256; representing each byte
void print_freq(const uint64_t freq[256]);

// Print SymbolFreq array
void print_symbol_freq(const SymbolFreq *freq, size_t size);

#endif // HELPER_H
