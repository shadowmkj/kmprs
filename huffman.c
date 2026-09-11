#include "huffman.h"
#include "core.h"
#include "shannon.h"
#include <stdlib.h>

void heapify(HuffmanHeap *heap, size_t index) {
    size_t smallest = index;
    size_t left = (index * 2) + 1;
    size_t right = (index * 2) + 2;

    if (left < heap->count &&
        heap->entries[left]->frequency < heap->entries[smallest]->frequency) {
        smallest = left;
    }

    if (right < heap->count &&
        heap->entries[right]->frequency < heap->entries[smallest]->frequency) {
        smallest = right;
    }

    if (smallest != index) {
        TreeNode *temp = heap->entries[smallest];
        heap->entries[smallest] = heap->entries[index];
        heap->entries[index] = temp;
        heapify(heap, smallest);
    }
}

void heap_push(HuffmanHeap *heap, TreeNode *entry) {
    if (!heap || heap->count >= ALPHABET_SIZE) {
        return;
    }

    size_t index = heap->count;
    heap->entries[index] = entry;
    heap->count++;

    while (index > 0) {
        size_t parent = (index - 1) / 2;
        if (heap->entries[parent]->frequency <=
            heap->entries[index]->frequency) {
            break;
        }
        TreeNode *temp = heap->entries[parent];
        heap->entries[parent] = heap->entries[index];
        heap->entries[index] = temp;
        index = parent;
    }
}

TreeNode *heap_pop(HuffmanHeap *heap) {
    // WARN: Assumes that heap is not empty

    TreeNode *min_entry = heap->entries[0];
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
        TreeNode *leaf_node = malloc(sizeof(TreeNode));
        if (!leaf_node) {
            for (size_t j = 0; j < i; j++) {
                free(heap->entries[j]);
            }
            free(heap);
            return NULL;
        }
        leaf_node->frequency = table->entries[i].frequency;
        leaf_node->symbol = table->entries[i].symbol;
        leaf_node->probability = table->entries[i].probability;
        leaf_node->is_leaf = 1;
        leaf_node->left = NULL;
        leaf_node->right = NULL;
        heap->entries[i] = leaf_node;
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
    if (!symtab || symtab->count == 0) {
        return NULL;
    }

    HuffmanHeap *heap = build_heap(symtab);
    if (!heap) {
        return NULL;
    }

    while (heap->count > 1) {
        TreeNode *min_entry = heap_pop(heap);
        TreeNode *next_entry = heap_pop(heap);

        TreeNode *parent = malloc(sizeof(TreeNode));
        if (!parent) {
            free_shannon_tree(min_entry);
            free_shannon_tree(next_entry);
            while (heap->count > 0) {
                free_shannon_tree(heap_pop(heap));
            }
            free(heap);
            return NULL;
        }
        parent->frequency = min_entry->frequency + next_entry->frequency;
        parent->symbol = 0;
        parent->probability = 0.0f;
        parent->is_leaf = 0;
        parent->left = next_entry;
        parent->right = min_entry;
        heap_push(heap, parent);
    }

    TreeNode *root = (heap->count == 1) ? heap->entries[0] : NULL;
    free(heap);
    return root;
}
