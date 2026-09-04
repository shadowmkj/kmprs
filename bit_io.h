#pragma once

#include <stdint.h>
#include <stdio.h>

/**
 * @file bit_io.h
 * @brief Bit-level I/O streams for variable-length bit packing and unpacking.
 */

/**
 * @brief Packs variable-length binary codes into 8-bit bytes.
 *
 * Computers read and write storage in 8-bit byte increments, but
 * Shannon/Huffman codes have variable bit lengths (e.g. 3, 5, or 11 bits). The
 * BitWriter buffers bits in a 64-bit accumulator, writing out full 8-bit bytes
 * (MSB first) to the destination file whenever 8 or more bits have accumulated.
 */
typedef struct BitWriter {
    FILE *out;            /**< Output file stream (must be open in binary mode) */
    uint64_t accumulator; /**< Temporary bit buffer */
    uint8_t bits_in_buffer; /**< Count of unwritten bits currently in accumulator */
} BitWriter;

/**
 * Initializes a BitWriter instance bound to an open output file stream.
 *
 * @param bw   Pointer to the BitWriter struct to initialize.
 * @param out  Pointer to an opened writable FILE stream (e.g. fopen with "wb").
 */
void bit_writer_init(BitWriter *bw, FILE *out);

/**
 * Appends `length` bits of `code` into the bitstream (MSB-first).
 *
 * Automatically flushes full 8-bit bytes to the output file stream whenever
 * 8 or more bits accumulate.
 *
 * @param bw      Pointer to the initialized BitWriter.
 * @param code    Bit pattern containing the codeword to write.
 * @param length  Number of valid bits in `code` to emit (1 to 32).
 */
void bit_writer_write(BitWriter *bw, uint32_t code, uint8_t length);

/**
 * Flushes any remaining fractional bits to the output file stream.
 *
 * If the total number of bits written is not an exact multiple of 8,
 * the remaining bits are shifted to the upper bits of the final byte
 * and padded with trailing zeros.
 *
 * @note This MUST be called once after all symbols have been written
 *       before closing the output file.
 *
 * @param bw  Pointer to the initialized BitWriter.
 */
void bit_writer_flush(BitWriter *bw);

/**
 * @brief Extracts individual bits sequentially from an 8-bit byte stream.
 *
 * Reads 1 byte at a time from the source file into an internal 8-bit buffer,
 * providing a bit-by-bit stream (MSB to LSB) to drive prefix-tree decoding.
 */
typedef struct BitReader {
    FILE *in;               /**< Input file stream (must be open in binary mode) */
    uint8_t buffer;         /**< Current 8-bit byte being consumed */
    uint8_t bits_remaining; /**< Number of unread bits left in the current byte */
} BitReader;

/**
 * Initializes a BitReader instance bound to an open input file stream.
 *
 * @param br  Pointer to the BitReader struct to initialize.
 * @param in  Pointer to an opened readable FILE stream (e.g. fopen with "rb").
 */
void bit_reader_init(BitReader *br, FILE *in);

/**
 * Reads the next single bit from the stream (MSB to LSB).
 *
 * When all 8 bits of the current byte have been consumed, automatically reads
 * the next byte from the input file stream.
 *
 * @param br  Pointer to the initialized BitReader.
 * @return    `0` or `1` if a bit was successfully read,
 *            or `-1` if the End-Of-File (EOF) has been reached.
 */
int bit_reader_read_bit(BitReader *br);
