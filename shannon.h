#pragma once

#include "core.h"
#include <stdint.h>

typedef struct ShannonNode {
    uint64_t frequency;        // Aggregate frequency of this branch
    float probability;         // Aggregate probability of this branch
    int is_leaf;               // 1 if leaf node, 0 if internal node
    uint8_t symbol;            // Byte value (valid only for leaf nodes)
    struct ShannonNode *left;  // '0' branch
    struct ShannonNode *right; // '1' branch
} ShannonNode;

ShannonNode *build_shannon_tree(const SymbolTable *table);
ShannonNode *build_shannon_tree_recursive(const SymbolTable *table,
                                          size_t start, size_t end);
void print_shannon_tree(const ShannonNode *root);
void free_shannon_tree(ShannonNode *node);
