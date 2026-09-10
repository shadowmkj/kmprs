#pragma once

#include "core.h"
#include <stdint.h>

typedef struct HuffmanHeap {
    size_t count;
    SymbolFreq entries[ALPHABET_SIZE];
} HuffmanHeap;

TreeNode *build_huffman_tree(const SymbolTable *symtab);
HuffmanHeap *build_heap(const SymbolTable *table);
void heap_push(HuffmanHeap *heap, SymbolFreq entry);
SymbolFreq heap_pop(HuffmanHeap *heap);
void heapify(HuffmanHeap *heap, size_t index);
