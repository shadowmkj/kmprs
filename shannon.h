#pragma once

#include "core.h"
#include <stdint.h>

/**
 * @file shannon.h
 * @brief Shannon-Fano tree construction, codebook generation, and tree operations.
 */

/**
 * @brief Binary tree node representing a Shannon-Fano code tree or subtree.
 *
 * Internal nodes represent binary prefix splits, while leaf nodes represent
 * individual 8-bit symbols with their associated statistical frequency.
 */
typedef struct ShannonNode {
    uint64_t frequency;        /**< Cumulative frequency of all symbols in this subtree */
    float probability;         /**< Cumulative probability of all symbols in this subtree */
    int is_leaf;               /**< Flag indicating if node is a leaf (1) or internal (0) */
    uint8_t symbol;            /**< Byte value represented (valid only if is_leaf == 1) */
    struct ShannonNode *left;  /**< Left child representing bit '0' */
    struct ShannonNode *right; /**< Right child representing bit '1' */
} ShannonNode;

/**
 * @brief Variable-length prefix codeword for a single symbol.
 *
 * Encapsulates the bit pattern (up to 32 bits) and its bit length. Bits are
 * ordered MSB to LSB.
 */
typedef struct ShannonCode {
    uint32_t bits; /**< Codeword bit pattern packed from MSB to LSB */
    uint8_t len;   /**< Bit length of the codeword (1 to 32) */
} ShannonCode;

/**
 * @brief Complete mapping table from byte values to their variable-length codewords.
 *
 * Allows O(1) lookup of a symbol's codeword during compression by indexing
 * directly with the byte value (0 to 255).
 */
typedef struct Codebook {
    ShannonCode codes[ALPHABET_SIZE]; /**< Array mapping each byte value to its ShannonCode */
} Codebook;

/**
 * @brief Constructs a binary Shannon-Fano prefix tree from a sorted symbol table.
 *
 * Recursively partitions the symbol distribution at points that minimize the
 * frequency difference between left and right subsets.
 *
 * @param table Pointer to the sorted SymbolTable containing active symbols.
 * @return Pointer to the root ShannonNode of the constructed tree,
 *         or NULL if the table is empty or memory allocation fails.
 *
 * @note The caller is responsible for deallocating the tree using free_shannon_tree().
 */
ShannonNode *build_shannon_tree(const SymbolTable *table);

/**
 * @brief Pretty-prints the hierarchical structure of a Shannon-Fano tree to stdout.
 *
 * Uses box-drawing characters (`├──`, `└──`, `│`) to display branch directions
 * (`[0]` for left, `[1]` for right), codewords, symbol formatting, frequencies,
 * and probabilities.
 *
 * @param root Pointer to the root of the Shannon-Fano tree to display.
 */
void print_shannon_tree(const ShannonNode *root);

/**
 * @brief Recursively frees all dynamically allocated nodes in a Shannon-Fano tree.
 *
 * Safely handles NULL pointers (no-op).
 *
 * @param node Pointer to the root of the tree/subtree to free.
 */
void free_shannon_tree(ShannonNode *node);

/**
 * @brief Traverses a Shannon-Fano tree to generate the prefix codebook.
 *
 * Performs a depth-first traversal of the tree, recording the path of '0' (left)
 * and '1' (right) branches to construct the exact binary codeword for each leaf symbol.
 * Handles edge cases such as single-symbol inputs by assigning a 1-bit codeword.
 *
 * @param root     Pointer to the root of the Shannon-Fano tree.
 * @param out_book Pointer to the caller-allocated Codebook to populate.
 */
void build_codebook(const ShannonNode *root, Codebook *out_book);

