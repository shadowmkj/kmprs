#include "bit_io.h"
#include "core.h"
#include "format.h"
#include "helper.h"
#include "shannon.h"
#include <stdint.h>
#include <stdio.h>

#define BUFFER_SIZE 1024

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

    unsigned char buffer[BUFFER_SIZE];
    uint64_t freq[ALPHABET_SIZE] = {0};
    size_t bytes_read;
    uint64_t total_chars = 0;

    while (!feof(input) && !ferror(input)) {
        bytes_read = fread(buffer, 1, BUFFER_SIZE, input);
        if (bytes_read == 0) {
            break;
        }
        for (size_t i = 0; i < bytes_read; i++) {
            total_chars++;
            freq[buffer[i]]++;
        }
    }

    if (ferror(input)) {
        perror("Error: failed to read file");
        fclose(input);
        return 1;
    }

    if (total_chars == 0) {
        fprintf(stderr, "Error: the file is empty\n");
        fclose(input);
        return 1;
    }

    SymbolTable table;
    build_symbol_table(freq, total_chars, &table);
    print_symbol_freq(table.entries, table.count);
    ShannonNode *tree = build_shannon_tree(&table);
    print_shannon_tree(tree);

    Codebook codebook;
    build_codebook(tree, &codebook);

    FILE *output = fopen("output.shn", "wb");
    if (!output) {
        perror("Error: failed to create output file");
        free_shannon_tree(tree);
        fclose(input);
        return 1;
    }

    write_shn_header(output, table.total_chars, &table);

    fseek(input, 0, SEEK_SET);
    BitWriter writer;
    bit_writer_init(&writer, output);
    uint8_t code_byte;
    while (!feof(input) && !ferror(input)) {
        bytes_read = fread(&code_byte, 1, 1, input);
        if (bytes_read == 0) {
            break;
        }
        ShannonCode code = codebook.codes[code_byte];
        bit_writer_write(&writer, code.bits, code.len);
    }

    bit_writer_flush(&writer);

    fclose(output);

    free_shannon_tree(tree);
    fclose(input);
    return 0;
}
