CC := "gcc"
CFLAGS := "-Wall -Wextra -Werror -pedantic -std=c11"
SRCS := "main.c helper.c core.c shannon.c bit_io.c"

CLANG_TIDY := if path_exists("/opt/homebrew/opt/llvm/bin/clang-tidy") == "true" {
    "/opt/homebrew/opt/llvm/bin/clang-tidy"
} else if path_exists("/usr/local/opt/llvm/bin/clang-tidy") == "true" {
    "/usr/local/opt/llvm/bin/clang-tidy"
} else {
    "clang-tidy"
}

build:
    {{CC}} {{CFLAGS}} {{SRCS}} -o kmprs

build-asan:
    {{CC}} {{CFLAGS}} -fsanitize=address,undefined -g {{SRCS}} -o kmprs

run file: build
    ./kmprs {{ file }}

test: build
    ./kmprs small.data

test-asan: build-asan
    ./kmprs small.data

tidy:
    @command -v {{CLANG_TIDY}} >/dev/null 2>&1 || { echo "Error: '{{CLANG_TIDY}}' not found. Please install clang-tidy/llvm." >&2; exit 1; }
    {{CLANG_TIDY}} {{SRCS}}


