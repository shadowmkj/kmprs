#include "core.h"
#include "format.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

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

int main(void) {
    printf("Running header serialization/deserialization unit tests...\n");
    test_header_roundtrip();
    test_invalid_magic();
    test_truncated_file();
    printf("All header tests passed!\n");
    return 0;
}
