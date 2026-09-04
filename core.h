#pragma once

#include <stddef.h>
#include <stdint.h>

#define ALPHABET_SIZE 256

typedef struct {
    uint64_t frequency;
    float probability;
    uint8_t symbol;
} SymbolFreq;

typedef struct {
    SymbolFreq entries[ALPHABET_SIZE];
    size_t count;         // Number of unique symbols actually present (0..256)
    uint64_t total_chars; // Total characters in source file
} SymbolTable;

// Populates and sorts the symbol table (Caller owns the storage)
void build_symbol_table(const uint64_t freq[ALPHABET_SIZE],
                        uint64_t total_chars, SymbolTable *out_table);
