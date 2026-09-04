#include "bit_io.h"
#include <stdint.h>
#include <stdio.h>

void bit_writer_init(BitWriter *bw, FILE *out);
void bit_writer_write(BitWriter *bw, uint32_t code, uint8_t length);
