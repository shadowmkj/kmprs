CC := "gcc"
CFLAGS := "-Wall -Wextra -Werror -pedantic -std=c11"
SRCS := "main.c helper.c core.c shannon.c bit_io.c format.c codec.c"
TEST_SRCS := "test_header.c core.c format.c shannon.c bit_io.c codec.c"

CLANG_TIDY := if path_exists("/opt/homebrew/opt/llvm/bin/clang-tidy") == "true" {
    "/opt/homebrew/opt/llvm/bin/clang-tidy"
} else if path_exists("/usr/local/opt/llvm/bin/clang-tidy") == "true" {
    "/usr/local/opt/llvm/bin/clang-tidy"
} else {
    "clang-tidy"
}

build:
    {{CC}} {{CFLAGS}} {{SRCS}} -o kmprs

build-release:
    {{CC}} {{CFLAGS}} -O3 -DNDEBUG {{SRCS}} -o kmprs

build-asan:
    {{CC}} {{CFLAGS}} -fsanitize=address,undefined -g {{SRCS}} -o kmprs

build-tests:
    {{CC}} {{CFLAGS}} {{TEST_SRCS}} -o test_header

build-tests-asan:
    {{CC}} {{CFLAGS}} -fsanitize=address,undefined -g {{TEST_SRCS}} -o test_header

run file: build
    ./kmprs {{ file }}

test: build build-tests
    @mkdir -p tmp
    ./kmprs small.data tmp/small.shn
    ./kmprs -d tmp/small.shn tmp/small.decomp
    cmp small.data tmp/small.decomp
    ./test_header

test-asan: build-asan build-tests-asan
    @mkdir -p tmp
    ./kmprs small.data tmp/small.shn
    ./kmprs -d tmp/small.shn tmp/small.decomp
    cmp small.data tmp/small.decomp
    ./test_header

tidy:
    @command -v {{CLANG_TIDY}} >/dev/null 2>&1 || { echo "Error: '{{CLANG_TIDY}}' not found. Please install clang-tidy/llvm." >&2; exit 1; }
    {{CLANG_TIDY}} {{SRCS}} test_header.c



