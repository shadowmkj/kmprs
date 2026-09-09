#include "huffman.h"
#include "core.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void format_symbol(uint8_t sym, char *out, size_t out_len) {
    if (sym == '\n') {
        snprintf(out, out_len, "'\\n' (0x0A)");
    } else if (sym == '\r') {
        snprintf(out, out_len, "'\\r' (0x0D)");
    } else if (sym == '\t') {
        snprintf(out, out_len, "'\\t' (0x09)");
    } else if (sym == ' ') {
        snprintf(out, out_len, "' ' (0x20)");
    } else if (sym >= 32 && sym <= 126) {
        snprintf(out, out_len, "'%c' (0x%02X)", sym, sym);
    } else {
        snprintf(out, out_len, "0x%02X", sym);
    }
}

void heap_push(SymbolTable *symtab, SymbolFreq symfreq) {
    size_t i = symtab->count;
    symtab->entries[i] = symfreq;
    symtab->count++;

    while (i > 0) {
        size_t parent = (i - 1) / 2;

        if (symtab->entries[i].frequency >= symtab->entries[parent].frequency) {
            break;
        }

        SymbolFreq temp = symtab->entries[i];
        symtab->entries[i] = symtab->entries[parent];
        symtab->entries[parent] = temp;
        i = parent;
    }
}

SymbolFreq heap_pop(SymbolTable *symtab) {
    SymbolFreq value = symtab->entries[0];
    symtab->entries[0] = symtab->entries[symtab->count - 1];
    symtab->count--;
    heapify(symtab, 0);
    return value;
}

void build_heap(SymbolTable *symtab) {

    // Only go till 0 since size_t cannot handle negative
    for (size_t i = (symtab->count / 2) - 1; i > 0; i--) {
        heapify(symtab, i);
    }

    // Then explicitly call heapify at index 0
    heapify(symtab, 0);
}

void heapify(SymbolTable *symtab, size_t index) {
    size_t smallest = index;
    size_t left = (2 * index) + 1;
    size_t right = (2 * index) + 2;

    if (left < symtab->count &&
        symtab->entries[left].frequency < symtab->entries[smallest].frequency) {
        smallest = left;
    }

    if (right < symtab->count && symtab->entries[right].frequency <
                                     symtab->entries[smallest].frequency) {
        smallest = right;
    }

    if (smallest != index) {
        SymbolFreq temp = symtab->entries[smallest];
        symtab->entries[smallest] = symtab->entries[index];
        symtab->entries[index] = temp;
        heapify(symtab, smallest);
    }
}

void print_heap(const SymbolTable *symtab) {
    printf("Printing Heap\n");
    for (size_t i = 0; i < symtab->count; i++) {
        printf("%lld", symtab->entries[i].frequency);
        printf("\n");
    }
    printf("Heap Over\n");
}

TreeNode *build_huffman_tree(SymbolTable *symtab) {
    while (symtab->count > 1) {
        SymbolFreq a = heap_pop(symtab);
        SymbolFreq b = heap_pop(symtab);
        TreeNode *parent = malloc(sizeof(TreeNode));
        parent->frequency = a.frequency + b.frequency;
        parent->symbol = 0;
        parent->left = a.tree_node;
        parent->right = b.tree_node;

        uint64_t freq_sum = a.frequency + b.frequency;
        SymbolFreq new_freq = {.frequency = freq_sum,
                               .symbol = 0,
                               .probability = 0,
                               .tree_node = parent};

        heap_push(symtab, new_freq);
    }
    return symtab->entries[0].tree_node;
}

static void print_huffman_tree_recursive(const TreeNode *node,
                                         const char *prefix, int is_left,
                                         const char *branch_label, char *code,
                                         int depth) {
    if (!node)
        return;

    printf("%s%s", prefix, branch_label);

    if (node->is_leaf) {
        char sym_buf[32];
        format_symbol(node->symbol, sym_buf, sizeof(sym_buf));
        code[depth] = '\0';
        printf("Leaf: %-12s [code: %-8s] (freq: %llu)\n", sym_buf,
               (depth > 0) ? code : "0", (unsigned long long)node->frequency);
    } else {
        printf("Internal (freq: %llu)\n", (unsigned long long)node->frequency);
    }

    char new_prefix[512];
    if (depth == 0) {
        new_prefix[0] = '\0';
    } else {
        snprintf(new_prefix, sizeof(new_prefix), "%s%s", prefix,
                 is_left ? "│   " : "    ");
    }

    if (node->left || node->right) {
        if (node->left) {
            code[depth] = '0';
            print_huffman_tree_recursive(node->left, new_prefix,
                                         node->right != NULL, "├── [0] ", code,
                                         depth + 1);
        }
        if (node->right) {
            code[depth] = '1';
            print_huffman_tree_recursive(node->right, new_prefix, 0, "└── [1] ",
                                         code, depth + 1);
        }
    }
}

void print_huffman_tree(const TreeNode *root) {
    if (!root) {
        printf("(empty tree)\n");
        return;
    }
    char code[ALPHABET_SIZE] = {0};
    print_huffman_tree_recursive(root, "", 0, "[Root] ", code, 0);
}
