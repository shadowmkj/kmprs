#include "huffman.h"
#include "core.h"
#include <stdlib.h>

void heapify(HuffmanHeap *heap, size_t index) {
    size_t smallest = index;
    size_t left = (index * 2) + 1;
    size_t right = (index * 2) + 2;

    if (left < heap->count &&
        heap->entries[left].frequency < heap->entries[smallest].frequency) {
        smallest = left;
    }

    if (right < heap->count &&
        heap->entries[right].frequency < heap->entries[smallest].frequency) {
        smallest = right;
    }

    if (smallest != index) {
        SymbolFreq temp = heap->entries[smallest];
        heap->entries[smallest] = heap->entries[index];
        heap->entries[index] = temp;
        heapify(heap, smallest);
    }
}

void heap_push(HuffmanHeap *heap, SymbolFreq entry) {
    if (!heap || heap->count >= ALPHABET_SIZE) {
        return;
    }

    size_t index = heap->count;
    heap->entries[index] = entry;
    heap->count++;

    while (index > 0) {
        size_t parent = (index - 1) / 2;
        if (heap->entries[parent].frequency <= heap->entries[index].frequency) {
            break;
        }
        SymbolFreq temp = heap->entries[parent];
        heap->entries[parent] = heap->entries[index];
        heap->entries[index] = temp;
        index = parent;
    }
}

SymbolFreq heap_pop(HuffmanHeap *heap) {
    if (!heap || heap->count == 0) {
        SymbolFreq empty = {0};
        return empty;
    }

    SymbolFreq min_entry = heap->entries[0];
    heap->count--;

    if (heap->count > 0) {
        heap->entries[0] = heap->entries[heap->count];
        heapify(heap, 0);
    }

    return min_entry;
}

HuffmanHeap *build_heap(const SymbolTable *table) {
    if (!table) {
        return NULL;
    }
    HuffmanHeap *heap = malloc(sizeof(HuffmanHeap));
    if (!heap) {
        return NULL;
    }
    for (size_t i = 0; i < table->count; i++) {
        heap->entries[i] = table->entries[i];
    }

    heap->count = table->count;

    if (heap->count > 1) {
        for (size_t i = (heap->count / 2); i > 0; i--) {
            heapify(heap, i - 1);
        }
    }

    return heap;
}

TreeNode *build_huffman_tree(const SymbolTable *symtab) {
    HuffmanHeap *heap = build_heap(symtab);
    free(heap);
    TreeNode *root = malloc(sizeof(TreeNode));
    return root;
}
