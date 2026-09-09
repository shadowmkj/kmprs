#pragma once

#include "core.h"
#include <stddef.h>
#include <stdint.h>

void build_heap(SymbolTable *symtab);
void heapify(SymbolTable *symtab, size_t index);
void heap_push(SymbolTable *symtab, SymbolFreq symfreq);
SymbolFreq heap_pop(SymbolTable *symtab);
void print_heap(const SymbolTable *symtab);

TreeNode *build_huffman_tree(SymbolTable *symtab);
void print_huffman_tree(const TreeNode *root);
