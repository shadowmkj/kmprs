#pragma once

#include "core.h"
#include <stddef.h>
#include <stdint.h>

/**
 * @file helper.h
 * @brief Diagnostic and visualization utilities for symbol tables and frequencies.
 */

/**
 * @brief Prints non-zero entries of a raw frequency histogram to stdout.
 *
 * Formats byte values, character representations, and total counts.
 *
 * @param freq Raw 256-element array containing byte occurrence counts.
 */
void print_freq(const uint64_t freq[ALPHABET_SIZE]);

/**
 * @brief Prints a formatted table of active symbol frequencies and probabilities to stdout.
 *
 * Iterates through an array of SymbolFreq structures and displays the symbol index,
 * character representation, occurrence frequency, and calculated probability.
 *
 * @param freq Pointer to an array of SymbolFreq entries.
 * @param size Number of elements in the `freq` array.
 */
void print_symbol_freq(const SymbolFreq *freq, size_t size);

