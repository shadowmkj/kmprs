#pragma once

#include <stdio.h>

/**
 * @file codec.h
 * @brief High-level streaming compression and decompression routines.
 *
 * Provides functions to encode raw input byte streams into the .shn container
 * format and reconstruct the original byte stream from .shn archives using
 * Shannon-Fano prefix coding.
 */

/**
 * @brief Compresses an uncompressed input stream and writes a .shn container to
 * output.
 *
 * Reads all bytes from the input stream to compute symbol frequencies,
 * constructs the Shannon-Fano prefix tree and codebook, writes the .shn
 * container header, rewinds the input stream, and encodes the payload using a
 * BitWriter.
 *
 * @param in   Readable, rewindable input stream (opened in "rb" mode).
 * @param out  Writable destination stream (opened in "wb" mode).
 * @return SHN_OK on success, or a negative ShnStatus error code on failure.
 */
int compress_stream(FILE *in, FILE *out);

/**
 * @brief Decompresses a .shn container stream and writes recovered payload to
 * output.
 *
 * Reads the container header, validates format integrity, reconstructs the
 * Shannon-Fano tree, and decodes the variable-length bitstream using a
 * BitReader until exactly original_size bytes have been written.
 *
 * @param in   Readable .shn stream (opened in "rb" mode).
 * @param out  Writable destination stream (opened in "wb" mode).
 * @return SHN_OK on success, or a negative ShnStatus error code on failure.
 */
int decompress_stream(FILE *in, FILE *out);
