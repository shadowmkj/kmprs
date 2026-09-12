#pragma once

#include "core.h"
#include <stdint.h>

/**
 * @file shannon.h
 * @brief Shannon-Fano tree construction, codebook generation, and tree
 * operations.
 */

/**
 * @brief Constructs a binary Shannon-Fano prefix tree from a sorted symbol
 * table.
 *
 * Recursively partitions the symbol distribution at points that minimize the
 * frequency difference between left and right subsets.
 *
 * @param table Pointer to the sorted SymbolTable containing active symbols.
 * @return Pointer to the root TreeNode of the constructed tree,
 *         or NULL if the table is empty or memory allocation fails.
 *
 * @note The caller is responsible for deallocating the tree using
 * free_shannon_tree().
 */
TreeNode *build_shannon_tree(const SymbolTable *table);

/**
 * @brief Pretty-prints the hierarchical structure of a Shannon-Fano tree to
 * stdout.
 *
 * Uses box-drawing characters (`├──`, `└──`, `│`) to display branch directions
 * (`[0]` for left, `[1]` for right), codewords, symbol formatting, frequencies,
 * and probabilities.
 *
 * @param root Pointer to the root of the Shannon-Fano tree to display.
 */
void print_shannon_tree(const TreeNode *root);

/**
 * @brief Recursively frees all dynamically allocated nodes in a Shannon-Fano
 * tree.
 *
 * Safely handles NULL pointers (no-op).
 *
 * @param node Pointer to the root of the tree/subtree to free.
 */
void free_shannon_tree(TreeNode *node);

/**
 * @brief Traverses a Shannon-Fano tree to generate the prefix codebook.
 *
 * Performs a depth-first traversal of the tree, recording the path of '0'
 * (left) and '1' (right) branches to construct the exact binary codeword for
 * each leaf symbol. Handles edge cases such as single-symbol inputs by
 * assigning a 1-bit codeword.
 *
 * @param root     Pointer to the root of the Shannon-Fano tree.
 * @param out_book Pointer to the caller-allocated Codebook to populate.
 */
void build_codebook(const TreeNode *root, Codebook *out_book);
