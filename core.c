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
            entry->symbol = (uint8_t)i;
            entry->frequency = freq[i];
            entry->probability = (total_chars > 0)
                                     ? ((float)freq[i] / (float)total_chars)
                                     : 0.0f;
        }
    }

    qsort(out_table->entries, out_table->count, sizeof(SymbolFreq),
          compare_freq);
}
