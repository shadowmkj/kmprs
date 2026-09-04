#include "helper.h"
#include <stdio.h>

// prints the frequencies
void print_freq(const uint64_t freq[256]) {
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0 && i != 10)
            printf("Freq: %d %c %lld\n", i, i, freq[i]);
    }
}
