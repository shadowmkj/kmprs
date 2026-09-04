#include "codec.h"
#include "core.h"
#include "format.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void test_header_roundtrip(void) {
    const char *test_path = "tmp/test_roundtrip.shn";

    // Prepare known frequency distribution
    uint64_t freq[ALPHABET_SIZE] = {0};
    freq['a'] = 100;
    freq['b'] = 250;
    freq['c'] = 50;
    freq['d'] = 15;
    uint64_t original_size = 415;

    SymbolTable expected_table;
    build_symbol_table(freq, original_size, &expected_table);

    FILE *out = fopen(test_path, "wb");
    assert(out != NULL && "Failed to open test file for writing");
    int write_status = write_shn_header(out, original_size, &expected_table);
    assert(write_status == SHN_OK && "write_shn_header failed");
    (void)fclose(out);

    FILE *in = fopen(test_path, "rb");
    assert(in != NULL && "Failed to open test file for reading");
    uint64_t decoded_size = 0;
    SymbolTable decoded_table;
    int read_status = read_shn_header(in, &decoded_size, &decoded_table);
    assert(read_status == SHN_OK && "read_shn_header failed");
    (void)fclose(in);

    // Verify all decoded attributes match bit-for-bit
    assert(decoded_size == original_size && "Decoded size mismatch");
    assert(decoded_table.count == expected_table.count &&
           "Symbol count mismatch");
    assert(decoded_table.total_chars == original_size &&
           "Total chars mismatch");

    for (size_t i = 0; i < expected_table.count; i++) {
        assert(decoded_table.entries[i].symbol ==
               expected_table.entries[i].symbol);
        assert(decoded_table.entries[i].frequency ==
               expected_table.entries[i].frequency);
    }

    printf("  [PASS] test_header_roundtrip (%zu symbols, %llu bytes)\n",
           decoded_table.count, (unsigned long long)decoded_size);
}

static void test_invalid_magic(void) {
    const char *bad_file = "tmp/test_bad_magic.shn";

    // Write corrupted magic header
    FILE *out = fopen(bad_file, "wb");
    assert(out != NULL);
    uint8_t corrupt_header[SHN_FIXED_HEADER_SIZE] = {'B', 'A', 'D', 0x01, 0};
    size_t written = fwrite(corrupt_header, 1, sizeof(corrupt_header), out);
    assert(written == sizeof(corrupt_header));
    (void)fclose(out);

    // Attempt to read corrupted header
    FILE *in = fopen(bad_file, "rb");
    assert(in != NULL);
    uint64_t size = 0;
    SymbolTable table;
    int status = read_shn_header(in, &size, &table);
    (void)fclose(in);

    assert(status == SHN_ERR_INVALID_MAGIC && "Expected SHN_ERR_INVALID_MAGIC");
    printf("  [PASS] test_invalid_magic\n");
}

static void test_truncated_file(void) {
    const char *trunc_file = "tmp/test_truncated.shn";

    // Write incomplete header (less than 14 bytes)
    FILE *out = fopen(trunc_file, "wb");
    assert(out != NULL);
    uint8_t partial[5] = {'S', 'H', 'N', 0x01, 0x10};
    size_t written = fwrite(partial, 1, sizeof(partial), out);
    assert(written == sizeof(partial));
    (void)fclose(out);

    // Attempt to read truncated file
    FILE *in = fopen(trunc_file, "rb");
    assert(in != NULL);
    uint64_t size = 0;
    SymbolTable table;
    int status = read_shn_header(in, &size, &table);
    (void)fclose(in);

    assert(status == SHN_ERR_TRUNCATED && "Expected SHN_ERR_TRUNCATED");
    printf("  [PASS] test_truncated_file\n");
}

static void assert_files_equal(const char *path1, const char *path2) {
    FILE *f1 = fopen(path1, "rb");
    assert(f1 != NULL && "Failed to open path1");
    FILE *f2 = fopen(path2, "rb");
    assert(f2 != NULL && "Failed to open path2");

    uint8_t b1[512];
    uint8_t b2[512];

    while (!feof(f1) && !ferror(f1) && !feof(f2) && !ferror(f2)) {
        size_t r1 = fread(b1, 1, sizeof(b1), f1);
        size_t r2 = fread(b2, 1, sizeof(b2), f2);
        assert(r1 == r2 && "File sizes differ");
        if (r1 == 0) {
            break;
        }
        assert(memcmp(b1, b2, r1) == 0 && "File contents differ");
    }

    (void)fclose(f1);
    (void)fclose(f2);
}

static void test_stream_roundtrip(const char *name, const uint8_t *data,
                                  size_t len) {
    const char *orig_path = "tmp/test_codec_orig.data";
    const char *comp_path = "tmp/test_codec.shn";
    const char *decomp_path = "tmp/test_codec_decomp.data";

    // Write original payload
    FILE *f_orig = fopen(orig_path, "wb");
    assert(f_orig != NULL);
    if (len > 0) {
        assert(fwrite(data, 1, len, f_orig) == len);
    }
    (void)fclose(f_orig);

    // Compress
    FILE *in_c = fopen(orig_path, "rb");
    assert(in_c != NULL);
    FILE *out_c = fopen(comp_path, "wb");
    assert(out_c != NULL);
    int c_status = compress_stream(in_c, out_c);
    assert(c_status == SHN_OK && "compress_stream failed");
    (void)fclose(in_c);
    (void)fclose(out_c);

    // Decompress
    FILE *in_d = fopen(comp_path, "rb");
    assert(in_d != NULL);
    FILE *out_d = fopen(decomp_path, "wb");
    assert(out_d != NULL);
    int d_status = decompress_stream(in_d, out_d);
    assert(d_status == SHN_OK && "decompress_stream failed");
    (void)fclose(in_d);
    (void)fclose(out_d);

    // Assert equality
    assert_files_equal(orig_path, decomp_path);
    printf("  [PASS] %s (%zu bytes)\n", name, len);
}

int main(void) {
    printf("Running container and codec unit tests...\n");
    test_header_roundtrip();
    test_invalid_magic();
    test_truncated_file();

    const char *text =
        "The quick brown fox jumps over the lazy dog! 1234567890 \n\t\r";
    test_stream_roundtrip("test_stream_roundtrip_text", (const uint8_t *)text,
                          strlen(text));

    uint8_t single[64];
    memset(single, 'A', sizeof(single));
    test_stream_roundtrip("test_stream_roundtrip_single_symbol", single,
                          sizeof(single));

    uint8_t binary[512];
    for (size_t i = 0; i < sizeof(binary); i++) {
        binary[i] = (uint8_t)(i % 256);
    }
    test_stream_roundtrip("test_stream_roundtrip_binary", binary,
                          sizeof(binary));

    printf("All container and codec tests passed!\n");
    return 0;
}
