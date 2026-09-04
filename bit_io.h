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
    FILE *out; /**< Output file stream (must be open in binary mode) */
    uint64_t accumulator; /**< Temporary bit buffer */
    uint8_t
        bits_in_buffer; /**< Count of unwritten bits currently in accumulator */
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
 * @def BIT_READER_BUFFER_SIZE
 * @brief Size of the block I/O buffer for BitReader (4 KiB).
 */
#define BIT_READER_BUFFER_SIZE 4096

/**
 * @brief High-performance buffered bit-level input stream with a 64-bit reservoir.
 *
 * Reads 4 KiB chunks from the input file stream using fread(), refilling an
 * internal 64-bit integer bit-reservoir. Individual bits are extracted MSB-first
 * in constant time O(1) without per-bit function call overhead.
 */
typedef struct BitReader {
    FILE *in;                                /**< Input file stream (open in "rb" mode) */
    uint8_t buffer[BIT_READER_BUFFER_SIZE];  /**< Block I/O buffer */
    size_t buffer_size;                      /**< Count of valid bytes currently in buffer */
    size_t buffer_pos;                       /**< Read cursor within block buffer */
    uint64_t bit_reservoir;                  /**< 64-bit window of upcoming bits */
    uint8_t bits_in_reservoir;               /**< Count of unread bits in reservoir (0 to 64) */
    int is_eof;                              /**< Flag indicating input stream reached EOF */
} BitReader;

/**
 * Initializes a BitReader instance bound to an open input file stream.
 *
 * @param br  Pointer to the BitReader struct to initialize.
 * @param in  Pointer to an opened readable FILE stream (e.g. fopen with "rb").
 */
void bit_reader_init(BitReader *br, FILE *in);

/**
 * Refills the 64-bit bit reservoir from the block buffer.
 *
 * Internal helper function called when the bit reservoir is depleted.
 *
 * @param br Pointer to the initialized BitReader.
 * @return 1 if at least one bit is available, or 0 if EOF is reached.
 */
int bit_reader_refill(BitReader *br);

/**
 * Reads the next single bit from the stream (MSB to LSB).
 *
 * Inlined directly into caller loops to eliminate function call overhead.
 *
 * @param br  Pointer to the initialized BitReader.
 * @return    `0` or `1` if a bit was successfully read,
 *            or `-1` if the End-Of-File (EOF) has been reached.
 */
static inline int bit_reader_read_bit(BitReader *br) {
    if (br->bits_in_reservoir == 0 && !bit_reader_refill(br)) {
        return -1;
    }
    br->bits_in_reservoir--;
    return (int)((br->bit_reservoir >> br->bits_in_reservoir) & 1U);
}
