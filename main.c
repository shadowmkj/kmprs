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

    char *buffer = malloc(BUFFER_SIZE);
    if (!buffer) {
        perror("Failed to allocate buffer\n");
        fclose(input);
        return 1;
    }

    int64_t freq[256] = {0};

    ssize_t bytes_read;

    while ((bytes_read = read(fileno(input), buffer, BUFFER_SIZE)) != 0) {
        buffer[bytes_read] = '\0';
        for (int i = 0; i < bytes_read; i++) {
            freq[buffer[i]]++;
        }
    }

    return 0;
}
