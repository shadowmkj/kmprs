#include "core.h"
#include "helper.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

int compare_freq(const void *p1, const void *p2) {
    const SymbolFreq *a = (const SymbolFreq *)p1;
    const SymbolFreq *b = (const SymbolFreq *)p2;
    if (a->frequency > b->frequency)
        return -1;
    if (a->frequency < b->frequency)
        return 1;
    if (a->symbol < b->symbol)
        return -1;
    if (a->symbol > b->symbol)
        return 1;
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Error: no filename specified\n");
        return 1;
    }

    FILE *input = fopen(argv[1], "rb");
    if (!input) {
        perror("Error: failed to read file");
        return 1;
    }

    unsigned char *buffer = malloc(BUFFER_SIZE);
    if (!buffer) {
        perror("Error: failed to allocate buffer");
        fclose(input);
        return 1;
    }

    uint64_t freq[256] = {0};
    size_t bytes_read;
    uint64_t total_chars = 0;

    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, input)) > 0) {
        for (size_t i = 0; i < bytes_read; i++) {
            total_chars++;
            freq[buffer[i]]++;
        }
    }

    // print_freq(freq);

    if (ferror(input)) {
        perror("Error: failed to read file");
        free(buffer);
        fclose(input);
        return 1;
    }

    int count = 0;
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            count++;
        }
    }

    if (count == 0) {
        fprintf(stderr, "Error: the file is empty\n");
        free(buffer);
        fclose(input);
        return 1;
    }

    SymbolFreq sorted_freq[count];
    int idx = 0;
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            sorted_freq[idx].symbol = (uint8_t)i;
            sorted_freq[idx].frequency = freq[i];
            sorted_freq[idx].probability =
                (float)sorted_freq[idx].frequency / (float)total_chars;
            idx++;
        }
    }
    qsort(sorted_freq, count, sizeof(SymbolFreq), compare_freq);
    print_symbol_freq(sorted_freq, count);

    free(buffer);
    fclose(input);
    return 0;
}
