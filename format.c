#include "format.h"
#include "core.h"
#include <stdint.h>
#include <stdio.h>

int write_shn_header(FILE *out, uint64_t original_size,
                     const SymbolTable *table) {
    if (!out) {
        return SHN_ERR_IO;
    }
    if (!table || table->count < 1 || table->count > ALPHABET_SIZE) {
        return SHN_ERR_INVALID_COUNT;
    }

    uint8_t
        buffer[SHN_FIXED_HEADER_SIZE + (ALPHABET_SIZE * SHN_SYMBOL_ENTRY_SIZE)];
    size_t offset = 0;

    // Magic Signature: "SHN\x01"
    buffer[offset++] = SHN_MAGIC_0;
    buffer[offset++] = SHN_MAGIC_1;
    buffer[offset++] = SHN_MAGIC_2;
    buffer[offset++] = SHN_VERSION;

    // Original uncompressed file size (uint64_t, Little-Endian)
    for (int i = 0; i < 8; i++) {
        buffer[offset++] =
            (uint8_t)((original_size >> (8U * (uint32_t)i)) & 0xFFU);
    }

    // Active symbol count K (uint16_t, Little-Endian: supports K = 256)
    uint16_t count = (uint16_t)table->count;
    buffer[offset++] = (uint8_t)(count & 0xFFU);
    buffer[offset++] = (uint8_t)(((uint32_t)count >> 8U) & 0xFFU);

    // Frequency dictionary records (K * 5 bytes)
    for (size_t i = 0; i < table->count; i++) {
        buffer[offset++] = table->entries[i].symbol;

        uint32_t freq = (uint32_t)table->entries[i].frequency;
        for (int j = 0; j < 4; j++) {
            buffer[offset++] = (uint8_t)((freq >> (8U * (uint32_t)j)) & 0xFFU);
        }
    }

    size_t written = fwrite(buffer, 1, offset, out);
    if (written != offset) {
        return SHN_ERR_IO;
    }

    return SHN_OK;
}

int read_shn_header(FILE *in, uint64_t *out_original_size,
                    SymbolTable *out_table) {
    if (!in || !out_original_size || !out_table) {
        return SHN_ERR_IO;
    }

    uint8_t fixed_header[SHN_FIXED_HEADER_SIZE];
    size_t read_bytes = fread(fixed_header, 1, SHN_FIXED_HEADER_SIZE, in);
    if (read_bytes != SHN_FIXED_HEADER_SIZE) {
        return feof(in) ? SHN_ERR_TRUNCATED : SHN_ERR_IO;
    }

    // Verify Magic Signature: "SHN\x01"
    if (fixed_header[0] != SHN_MAGIC_0 || fixed_header[1] != SHN_MAGIC_1 ||
        fixed_header[2] != SHN_MAGIC_2 || fixed_header[3] != SHN_VERSION) {
        return SHN_ERR_INVALID_MAGIC;
    }

    // Decode original_size (uint64_t, Little-Endian)
    uint64_t original_size = 0;
    for (int i = 0; i < 8; i++) {
        original_size |= ((uint64_t)fixed_header[4 + i]) << (8U * (uint32_t)i);
    }
    *out_original_size = original_size;

    // Decode symbol_count K (uint16_t, Little-Endian)
    uint16_t symbol_count = (uint16_t)((uint32_t)fixed_header[12] |
                                       ((uint32_t)fixed_header[13] << 8U));

    if (symbol_count < 1 || symbol_count > ALPHABET_SIZE) {
        return SHN_ERR_INVALID_COUNT;
    }

    // Read Frequency Dictionary (K * 5 bytes)
    size_t dict_bytes = (size_t)symbol_count * SHN_SYMBOL_ENTRY_SIZE;
    uint8_t dict_buffer[ALPHABET_SIZE * SHN_SYMBOL_ENTRY_SIZE];
    if (fread(dict_buffer, 1, dict_bytes, in) != dict_bytes) {
        return feof(in) ? SHN_ERR_TRUNCATED : SHN_ERR_IO;
    }

    // Decode symbol-frequency pairs into raw frequency histogram
    uint64_t freq[ALPHABET_SIZE] = {0};
    for (size_t i = 0; i < symbol_count; i++) {
        size_t entry_offset = i * SHN_SYMBOL_ENTRY_SIZE;
        uint8_t symbol = dict_buffer[entry_offset];

        uint32_t symbol_freq = 0;
        for (int j = 0; j < 4; j++) {
            symbol_freq |= ((uint32_t)dict_buffer[entry_offset + 1 + j])
                           << (8U * (uint32_t)j);
        }

        freq[symbol] = symbol_freq;
    }

    // Reconstruct SymbolTable using deterministic sorting
    build_symbol_table(freq, original_size, out_table);

    return SHN_OK;
}

