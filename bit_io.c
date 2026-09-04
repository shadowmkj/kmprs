#include "bit_io.h"
#include <stdint.h>
#include <stdio.h>

void bit_writer_init(BitWriter *bw, FILE *out) {
    if (!bw) {
        return;
    }
    bw->out = out;
    bw->accumulator = 0;
    bw->bits_in_buffer = 0;
}

void bit_writer_write(BitWriter *bw, uint32_t code, uint8_t length) {
    bw->accumulator = (bw->accumulator << length) |
                      (code & (uint32_t)((1ULL << length) - 1ULL));
    bw->bits_in_buffer += length;

    while (bw->bits_in_buffer >= 8) {
        bw->bits_in_buffer -= 8;
        uint8_t byte =
            (uint8_t)((bw->accumulator >> bw->bits_in_buffer) & 0xFFU);
        fputc(byte, bw->out);
    }
}

void bit_writer_flush(BitWriter *bw) {
    if (bw->bits_in_buffer > 0) {
        uint8_t byte =
            (uint8_t)((bw->accumulator << (8U - bw->bits_in_buffer)) & 0xFFU);
        fputc(byte, bw->out);
        bw->accumulator = 0;
        bw->bits_in_buffer = 0;
    }
}

void bit_reader_init(BitReader *br, FILE *in) {
    if (!br) {
        return;
    }
    br->in = in;
    br->buffer_size = 0;
    br->buffer_pos = 0;
    br->bit_reservoir = 0;
    br->bits_in_reservoir = 0;
    br->is_eof = 0;
}

int bit_reader_refill(BitReader *br) {
    if (!br || !br->in) {
        return 0;
    }

    // Append full 8-bit bytes into the 64-bit integer reservoir as long as
    // there is sufficient capacity (at least 8 bits available: <= 56 bits).
    while (br->bits_in_reservoir <= 56U) {
        // If the 4 KiB block buffer is exhausted, fetch the next chunk from
        // disk
        if (br->buffer_pos >= br->buffer_size) {
            if (br->is_eof) {
                break;
            }
            br->buffer_size = fread(br->buffer, 1, sizeof(br->buffer), br->in);
            br->buffer_pos = 0;
            if (br->buffer_size == 0) {
                br->is_eof = 1;
                break;
            }
        }

        uint8_t byte = br->buffer[br->buffer_pos++];
        br->bit_reservoir = (br->bit_reservoir << 8U) | (uint64_t)byte;
        br->bits_in_reservoir += 8U;
    }

    return (br->bits_in_reservoir > 0) ? 1 : 0;
}
