#include "helper.h"
#include <stdio.h>

// prints the SymbolFreq
void print_symbol_freq(const SymbolFreq *freq, size_t size) {
    for (size_t i = 0; i < size; i++) {
        if (freq[i].frequency > 0)
            printf("SymbolFreq: %zu %c %llu %f\n", i, freq[i].symbol,
                   (long long unsigned int)freq[i].frequency,
                   freq[i].probability);
    }
}
