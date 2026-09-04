CC := "gcc"
CFLAGS := "-Wall -Wextra -Werror -pedantic -std=c11"
SRCS := "main.c helper.c core.c shannon.c"

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
