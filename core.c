#include "core.h"
#include <stdlib.h>

static int compare_freq(const void *p1, const void *p2) {
    const SymbolFreq *a = p1;
    const SymbolFreq *b = p2;
    if (a->frequency > b->frequency)
        return -1;
    if (a->frequency < b->frequency)
        return 1;
    if (a->symbol < b->symbol)
        return -1;
    if (a->symbol > b->symbol)
        return 1;
    return 0;
}

void build_symbol_table(const uint64_t freq[ALPHABET_SIZE],
                        uint64_t total_chars, SymbolTable *out_table) {
    out_table->total_chars = total_chars;
    out_table->count = 0;

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (freq[i] > 0) {
            SymbolFreq *entry = &out_table->entries[out_table->count++];
            TreeNode *tree_node = malloc(sizeof(TreeNode));
            tree_node->frequency = freq[i];
            tree_node->symbol = (uint8_t)i;
            tree_node->is_leaf = 1;
            tree_node->left = NULL;
            tree_node->right = NULL;
            entry->symbol = (uint8_t)i;
            entry->frequency = freq[i];
            entry->probability = (total_chars > 0)
                                     ? ((float)freq[i] / (float)total_chars)
                                     : 0.0f;
            entry->tree_node = tree_node;
        }
    }

    // TODO: Move this function outside, huffman does not need sorting
    qsort(out_table->entries, out_table->count, sizeof(SymbolFreq),
          compare_freq);
}
