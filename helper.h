#ifndef HELPER_H
#define HELPER_H

#include "core.h"
#include <stddef.h>
#include <stdint.h>

// freq is an array of size 256 (ALPHABET_SIZE); representing each byte
void print_freq(const uint64_t freq[ALPHABET_SIZE]);

// Print SymbolFreq array
void print_symbol_freq(const SymbolFreq *freq, size_t size);

#endif // HELPER_H
