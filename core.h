#pragma once

#include <stddef.h>
#include <stdint.h>

#define ALPHABET_SIZE 256

typedef struct {
    uint64_t frequency;
    float probability;
    uint8_t symbol;
} SymbolFreq;

typedef struct ShannonNode {
    uint8_t symbol;            // Byte value (valid only for leaf nodes)
    uint64_t frequency;        // Aggregate frequency of this branch
    float probability;         // Aggregate probability of this branch
    int is_leaf;               // 1 if leaf node, 0 if internal node
    struct ShannonNode *left;  // '0' branch
    struct ShannonNode *right; // '1' branch
} ShannonNode;

typedef struct {
    SymbolFreq entries[ALPHABET_SIZE];
    size_t count;         // Number of unique symbols actually present (0..256)
    uint64_t total_chars; // Total characters in source file
} SymbolTable;

// Populates and sorts the symbol table (Caller owns the storage)
void build_symbol_table(const uint64_t freq[ALPHABET_SIZE],
                        uint64_t total_chars, SymbolTable *out_table);
