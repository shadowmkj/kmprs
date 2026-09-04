#include "shannon.h"
#include "core.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static void print_shannon_tree_recursive(const ShannonNode *node,
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
        printf("Leaf: %-12s [code: %-8s] (freq: %llu, prob: %.4f)\n", sym_buf,
               (depth > 0) ? code : "0", (unsigned long long)node->frequency,
               node->probability);
    } else {
        printf("Internal (freq: %llu, prob: %.4f)\n",
               (unsigned long long)node->frequency, node->probability);
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
            print_shannon_tree_recursive(node->left, new_prefix,
                                         node->right != NULL, "├── [0] ", code,
                                         depth + 1);
        }
        if (node->right) {
            code[depth] = '1';
            print_shannon_tree_recursive(node->right, new_prefix, 0, "└── [1] ",
                                         code, depth + 1);
        }
    }
}

static ShannonNode *build_shannon_tree_recursive(const SymbolTable *table,
                                                 size_t start, size_t end) {
    if (start > end)
        return NULL;

    ShannonNode *node = malloc(sizeof(ShannonNode));
    if (!node)
        return NULL;

    if (start == end) {
        node->is_leaf = 1;
        node->symbol = table->entries[start].symbol;
        node->frequency = table->entries[start].frequency;
        node->probability = table->entries[start].probability;
        node->left = NULL;
        node->right = NULL;
        return node;
    }

    uint64_t total = 0;
    for (size_t i = start; i <= end; i++) {
        total += table->entries[i].frequency;
    }

    uint64_t minimum = UINT64_MAX;
    size_t split = start;
    uint64_t left_sum = 0;

    for (size_t i = start; i < end; i++) {
        left_sum += table->entries[i].frequency;
        uint64_t right_sum = total - left_sum;
        uint64_t diff = (left_sum > right_sum) ? (left_sum - right_sum)
                                               : (right_sum - left_sum);
        if (diff < minimum) {
            minimum = diff;
            split = i;
        } else {
            break;
        }
    }

    node->is_leaf = 0;
    node->symbol = 0;
    node->left = build_shannon_tree_recursive(table, start, split);
    node->right = build_shannon_tree_recursive(table, split + 1, end);
    node->frequency = (node->left ? node->left->frequency : 0) +
                      (node->right ? node->right->frequency : 0);
    node->probability = (node->left ? node->left->probability : 0) +
                        (node->right ? node->right->probability : 0);
    return node;
}

ShannonNode *build_shannon_tree(const SymbolTable *table) {
    if (!table || table->count == 0)
        return NULL;
    return build_shannon_tree_recursive(table, 0, table->count - 1);
}

void print_shannon_tree(const ShannonNode *root) {
    if (!root) {
        printf("(empty tree)\n");
        return;
    }
    char code[ALPHABET_SIZE] = {0};
    print_shannon_tree_recursive(root, "", 0, "[Root] ", code, 0);
}

void free_shannon_tree(ShannonNode *node) {
    if (!node)
        return;
    free_shannon_tree(node->left);
    free_shannon_tree(node->right);
    free(node);
}

static void extract_codes_recursive(const ShannonNode *node,
                                    uint32_t current_code, uint8_t depth,
                                    Codebook *out_book) {
    if (!node)
        return;

    if (node->is_leaf) {
        out_book->codes[node->symbol].bits = current_code;
        // NOTE: Edge case: single-symbol file receives a 1-bit code
        out_book->codes[node->symbol].len = (depth == 0) ? 1 : depth;
        return;
    }

    extract_codes_recursive(node->left, (current_code << 1U) | 0U, depth + 1,
                            out_book);
    extract_codes_recursive(node->right, (current_code << 1U) | 1U, depth + 1,
                            out_book);
}

void build_codebook(const ShannonNode *root, Codebook *out_book) {
    memset(out_book, 0, sizeof(Codebook));
    if (root) {
        extract_codes_recursive(root, 0, 0, out_book);
    }
}
