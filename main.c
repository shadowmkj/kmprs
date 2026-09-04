#include "codec.h"
#include "format.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s [-d] <input_file> [output_file]\n", prog_name);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -d    Decompress .shn archive\n");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    bool decompress = false;
    const char *input_path = NULL;
    const char *output_path = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            decompress = true;
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "Error: unrecognized option '%s'\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        } else if (!input_path) {
            input_path = argv[i];
        } else if (!output_path) {
            output_path = argv[i];
        } else {
            fprintf(stderr, "Error: unexpected argument '%s'\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }

    if (!input_path) {
        fprintf(stderr, "Error: no input file specified\n");
        print_usage(argv[0]);
        return 1;
    }

    char resolved_output[1024];
    if (!output_path) {
        if (decompress) {
            size_t in_len = strlen(input_path);
            if (in_len > 4 && strcmp(input_path + in_len - 4, ".shn") == 0) {
                size_t base_len = in_len - 4;
                if (base_len >= sizeof(resolved_output)) {
                    fprintf(stderr, "Error: input path too long\n");
                    return 1;
                }
                memcpy(resolved_output, input_path, base_len);
                resolved_output[base_len] = '\0';
            } else {
                int written = snprintf(resolved_output, sizeof(resolved_output),
                                       "%s.orig", input_path);
                if (written < 0 || (size_t)written >= sizeof(resolved_output)) {
                    fprintf(stderr, "Error: output path too long\n");
                    return 1;
                }
            }
        } else {
            int written = snprintf(resolved_output, sizeof(resolved_output),
                                   "%s.shn", input_path);
            if (written < 0 || (size_t)written >= sizeof(resolved_output)) {
                fprintf(stderr, "Error: output path too long\n");
                return 1;
            }
        }
        output_path = resolved_output;
    }

    if (strcmp(input_path, output_path) == 0) {
        fprintf(stderr, "Error: input and output paths must differ ('%s')\n",
                input_path);
        return 1;
    }

    FILE *in = fopen(input_path, "rb");
    if (!in) {
        perror("Error: failed to open input file");
        return 1;
    }

    FILE *out = fopen(output_path, "wb");
    if (!out) {
        perror("Error: failed to open output file");
        fclose(in);
        return 1;
    }

    int status =
        (int)decompress ? decompress_stream(in, out) : compress_stream(in, out);

    fclose(in);
    fclose(out);

    if (status != SHN_OK) {
        switch (status) {
        case SHN_ERR_INVALID_MAGIC:
            fprintf(stderr, "Error: '%s' is not a valid .shn archive\n",
                    input_path);
            break;
        case SHN_ERR_INVALID_COUNT:
            fprintf(stderr, "Error: invalid symbol count in '%s'\n",
                    input_path);
            break;
        case SHN_ERR_TRUNCATED:
            fprintf(stderr, "Error: unexpected end of file reading '%s'\n",
                    input_path);
            break;
        case SHN_ERR_CORRUPT_STREAM:
            fprintf(stderr, "Error: corrupted compressed bitstream in '%s'\n",
                    input_path);
            break;
        case SHN_ERR_EMPTY_FILE:
            fprintf(stderr, "Error: input file '%s' is empty\n", input_path);
            break;
        case SHN_ERR_IO:
        default:
            fprintf(stderr, "Error: I/O failure processing '%s'\n", input_path);
            break;
        }
        remove(output_path);
        return 1;
    }

    return 0;
}
