#define _POSIX_C_SOURCE 200809L

#include "helper.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char **argv) {
    if (argc < 2) {
        perror("No filename specified\n");
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (!input) {
        perror("Failed to read file\n");
        return 1;
    }

    unsigned char *buffer = malloc(BUFFER_SIZE);
    if (!buffer) {
        perror("Failed to allocate buffer");
        fclose(input);
        return 1;
    }

    uint64_t freq[256] = {0};
    ssize_t bytes_read;

    while ((bytes_read = read(fileno(input), buffer, BUFFER_SIZE)) > 0) {
        for (ssize_t i = 0; i < bytes_read; i++) {
            freq[buffer[i]]++;
        }
    }

    print_freq(freq);

    if (bytes_read < 0) {
        perror("Error reading file");
        free(buffer);
        fclose(input);
        return 1;
    }

    free(buffer);
    fclose(input);
    return 0;
}
