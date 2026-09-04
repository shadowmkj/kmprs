#pragma once

#include <stddef.h>
#include <stdint.h>

/**
 * @file core.h
 * @brief Core data structures and functions for byte frequency tracking.
 */

/**
 * @def ALPHABET_SIZE
 * @brief Total number of distinct possible byte values (0 through 255).
 */
#define ALPHABET_SIZE 256

/**
 * @struct SymbolFreq
 * @brief Statistical frequency and probability for a single byte symbol.
 *
 * Fields are ordered to minimize struct padding (16 bytes total: 8 + 4 + 1 + 3
 * padding).
 */
typedef struct {
    uint64_t frequency; /**< Observed occurrence count of this symbol */
    float probability;  /**< Relative frequency (frequency / total_chars) */
    uint8_t symbol;     /**< The 8-bit byte value (0 to 255) */
} SymbolFreq;

/**
 * @struct SymbolTable
 * @brief Encapsulates the sorted active symbols and metadata for an input file.
 *
 * Fixed-capacity table allocated on the stack by the caller (4 KB total),
 * avoiding dynamic memory allocations during frequency analysis.
 */
typedef struct {
    SymbolFreq entries[ALPHABET_SIZE]; /**< Array of active symbol statistics,
                                          sorted descending by frequency */
    size_t count; /**< Number of distinct symbols actually present in the file
                     (0 to 256) */
    uint64_t total_chars; /**< Total number of bytes in the source file */
} SymbolTable;

/**
 * @brief Populates and sorts the active symbols from a raw frequency histogram.
 *
 * Filters out all unused symbols (frequency == 0), calculates individual
 * probabilities, and sorts active entries in descending order of frequency
 * (with byte symbol tie-breaking). Operates entirely on caller-allocated
 * storage.
 *
 * @param freq         Raw 256-element frequency histogram where index is byte
 * value.
 * @param total_chars  Total byte count of the input file.
 * @param out_table    Pointer to caller-owned SymbolTable struct to populate.
 */
void build_symbol_table(const uint64_t freq[ALPHABET_SIZE],
                        uint64_t total_chars, SymbolTable *out_table);
