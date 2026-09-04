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
    br->buffer = 0;
    br->bits_remaining = 0;
}

int bit_reader_read_bit(BitReader *br) {
    if (br->bits_remaining == 0) {
        int byte = fgetc(br->in);
        if (byte == EOF) {
            return -1;
        }
        br->buffer = (uint8_t)byte;
        br->bits_remaining = 8;
    }
    br->bits_remaining--;
    return (int)(((unsigned int)br->buffer >> br->bits_remaining) & 1U);
}
