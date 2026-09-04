#include "codec.h"
#include "bit_io.h"
#include "core.h"
#include "format.h"
#include "shannon.h"
#include <stdint.h>
#include <stdio.h>

int compress_stream(FILE *in, FILE *out) {
    if (!in || !out) {
        return SHN_ERR_IO;
    }

    // Count byte frequencies across the entire input file to establish symbol
    // probabilities for the Shannon-Fano code tree.
    uint8_t buffer[4096];
    uint64_t freq[ALPHABET_SIZE] = {0};
    uint64_t total_chars = 0;
    size_t bytes_read = 0;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        for (size_t i = 0; i < bytes_read; i++) {
            total_chars++;
            freq[buffer[i]]++;
        }
    }

    if (ferror(in)) {
        return SHN_ERR_IO;
    }

    if (total_chars == 0) {
        return SHN_ERR_EMPTY_FILE;
    }

    // Build the sorted symbol table, generate the Shannon-Fano prefix tree, and
    // extract prefix codes into an O(1) direct lookup table.
    SymbolTable table;
    build_symbol_table(freq, total_chars, &table);

    ShannonNode *tree = build_shannon_tree(&table);
    if (!tree) {
        return SHN_ERR_IO;
    }

    Codebook codebook;
    build_codebook(tree, &codebook);

    // Serialize magic bytes, uncompressed size, and frequency dictionary table.
    int status = write_shn_header(out, total_chars, &table);
    if (status != SHN_OK) {
        free_shannon_tree(tree);
        return status;
    }

    // Rewind input to the beginning, convert each byte into its prefix
    // codeword, and pack codewords bit-by-bit into the output stream.
    if (fseek(in, 0, SEEK_SET) != 0) {
        free_shannon_tree(tree);
        return SHN_ERR_IO;
    }

    BitWriter writer;
    bit_writer_init(&writer, out);

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        for (size_t i = 0; i < bytes_read; i++) {
            ShannonCode code = codebook.codes[buffer[i]];
            bit_writer_write(&writer, code.bits, code.len);
        }
    }

    if (ferror(in)) {
        free_shannon_tree(tree);
        return SHN_ERR_IO;
    }

    // Flush any pending unaligned fractional bits with trailing zero padding.
    bit_writer_flush(&writer);
    free_shannon_tree(tree);

    return SHN_OK;
}

int decompress_stream(FILE *in, FILE *out) {
    if (!in || !out) {
        return SHN_ERR_IO;
    }

    // Validate container magic, extract original uncompressed file size, and
    // reconstruct the frequency dictionary table.
    uint64_t original_size = 0;
    SymbolTable table;
    int status = read_shn_header(in, &original_size, &table);
    if (status != SHN_OK) {
        return status;
    }

    if (original_size == 0) {
        return SHN_OK;
    }

    // When all bytes in the original file are identical (K = 1), each symbol is
    // encoded with a 1-bit code. Read the bit to verify stream integrity and
    // emit the single symbol.
    if (table.count == 1) {
        uint8_t sym = table.entries[0].symbol;
        BitReader reader;
        bit_reader_init(&reader, in);
        for (uint64_t i = 0; i < original_size; i++) {
            int bit = bit_reader_read_bit(&reader);
            if (bit < 0) {
                return SHN_ERR_TRUNCATED;
            }
            if (fputc((int)sym, out) == EOF) {
                return SHN_ERR_IO;
            }
        }
        return SHN_OK;
    }

    // Reconstruct the exact same prefix tree topology from the sorted table.
    ShannonNode *tree = build_shannon_tree(&table);
    if (!tree) {
        return SHN_ERR_IO;
    }

    // Sequentially extract bits from the bitstream, walking the tree from root
    // to leaf for each symbol. Halt precisely when original_size bytes have
    // been emitted to avoid interpreting trailing padding bits as data.
    BitReader reader;
    bit_reader_init(&reader, in);

    for (uint64_t decoded = 0; decoded < original_size; decoded++) {
        const ShannonNode *curr = tree;
        while (!curr->is_leaf) {
            int bit = bit_reader_read_bit(&reader);
            if (bit < 0) {
                free_shannon_tree(tree);
                return SHN_ERR_TRUNCATED;
            }
            curr = (bit == 0) ? curr->left : curr->right;
            if (!curr) {
                free_shannon_tree(tree);
                return SHN_ERR_CORRUPT_STREAM;
            }
        }

        if (fputc((int)curr->symbol, out) == EOF) {
            free_shannon_tree(tree);
            return SHN_ERR_IO;
        }
    }

    free_shannon_tree(tree);
    return SHN_OK;
}
