#pragma once

#include <stdint.h>
#include <stdio.h>

/**
 * @file bit_io.h
 * @brief Bit-level I/O streams for variable-length bit packing and unpacking.
 */

/**
 * @def BIT_WRITER_BUFFER_SIZE
 * @brief Size of the block I/O buffer for BitWriter (4 KiB).
 */
#define BIT_WRITER_BUFFER_SIZE 4096

/**
 * @brief High-performance buffered bit-level output stream with a 64-bit accumulator.
 *
 * Packs variable-length binary codes into a 64-bit accumulator and buffers full
 * 8-bit bytes (MSB first) into an internal 4 KiB memory buffer, writing in bulk
 * to the destination file stream using fwrite() for maximum throughput.
 */
typedef struct BitWriter {
    FILE *out;                                /**< Destination file stream (open in "wb" mode) */
    uint8_t buffer[BIT_WRITER_BUFFER_SIZE];  /**< Block I/O buffer for batch fwrite() */
    size_t buffer_pos;                        /**< Current write cursor in the block buffer */
    uint64_t accumulator;                     /**< 64-bit temporary bit reservoir */
    uint8_t bits_in_buffer;                   /**< Unwritten bits count in accumulator (0 to 64) */
} BitWriter;

/**
 * Initializes a BitWriter instance bound to an open output file stream.
 *
 * @param bw   Pointer to the BitWriter struct to initialize.
 * @param out  Pointer to an opened writable FILE stream (e.g. fopen with "wb").
 */
void bit_writer_init(BitWriter *bw, FILE *out);

/**
 * Flushes the internal block buffer to the destination file stream.
 *
 * @param bw Pointer to the initialized BitWriter.
 * @return 1 on success, or 0 on I/O failure.
 */
int bit_writer_flush_buffer(BitWriter *bw);

/**
 * Appends `length` bits of `code` into the bitstream (MSB-first).
 *
 * Inlined directly into caller loops to eliminate function call overhead.
 * Emits full 8-bit bytes directly into the 4 KiB block buffer.
 *
 * @param bw      Pointer to the initialized BitWriter.
 * @param code    Bit pattern containing the codeword to write.
 * @param length  Number of valid bits in `code` to emit (1 to 32).
 */
static inline void bit_writer_write(BitWriter *bw, uint32_t code,
                                    uint8_t length) {
    uint64_t mask = (length == 32U) ? 0xFFFFFFFFULL : ((1ULL << length) - 1ULL);
    bw->accumulator = (bw->accumulator << length) | ((uint64_t)code & mask);
    bw->bits_in_buffer += length;

    while (bw->bits_in_buffer >= 8U) {
        bw->bits_in_buffer -= 8U;
        uint8_t byte =
            (uint8_t)((bw->accumulator >> bw->bits_in_buffer) & 0xFFU);
        bw->buffer[bw->buffer_pos++] = byte;
        if (bw->buffer_pos == BIT_WRITER_BUFFER_SIZE) {
            (void)bit_writer_flush_buffer(bw);
        }
    }
}

/**
 * Flushes any remaining fractional bits and the block buffer to disk.
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
 * @brief High-performance buffered bit-level input stream with a 64-bit
 * reservoir.
 *
 * Reads 4 KiB chunks from the input file stream using fread(), refilling an
 * internal 64-bit integer bit-reservoir. Individual bits are extracted
 * MSB-first in constant time O(1) without per-bit function call overhead.
 */
typedef struct BitReader {
    FILE *in; /**< Input file stream (open in "rb" mode) */
    uint8_t buffer[BIT_READER_BUFFER_SIZE]; /**< Block I/O buffer */
    size_t buffer_size;     /**< Count of valid bytes currently in buffer */
    size_t buffer_pos;      /**< Read cursor within block buffer */
    uint64_t bit_reservoir; /**< 64-bit window of upcoming bits */
    uint8_t
        bits_in_reservoir; /**< Count of unread bits in reservoir (0 to 64) */
    int is_eof;            /**< Flag indicating input stream reached EOF */
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
