#include "helper.h"
#include "core.h"
#include <stdio.h>

// prints the SymbolFreq
void print_symbol_freq(const SymbolFreq *freq, size_t size) {
    for (size_t i = 0; i < size; i++) {
        if (freq[i].frequency > 0)
            printf("SymbolFreq: %lu %c %llu %f\n", i, freq[i].symbol,
                   (long long unsigned int)freq[i].frequency,
                   freq[i].probability);
    }
}

// prints the frequencies
void print_freq(const uint64_t freq[256]) {
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0 && i != 10)
            printf("Freq: %d %c %llu\n", i, i, (unsigned long long)freq[i]);
    }
}
