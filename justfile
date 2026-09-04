CC := "gcc"
CFLAGS := "-Wall -Wextra -Werror -pedantic -std=c11"

build:
    {{CC}} {{CFLAGS}} main.c -o kmprs

build-asan:
    {{CC}} {{CFLAGS}} -fsanitize=address,undefined -g main.c -o kmprs

run file: build
    ./kmprs {{ file }}

test: build
    ./kmprs small.data

test-asan: build-asan
    ./kmprs small.data
