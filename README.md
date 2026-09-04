# kmprs - A Shannon-Fano File Compression Tool

`kmprs` is an educational command-line file compression utility built to explore entropy encoding and prefix tree algorithms using Shannon-Fano coding.

> [!NOTE]
> `kmprs` is designed for learning and experimentation with statistical encoding algorithms.

---

## Installation

### Option 1: Quick Install (curl)

Run the automated installer to detect your platform, download the latest pre-compiled release, and install `kmprs`:

```bash
curl -fsSL https://raw.githubusercontent.com/shadowmkj/kmprs/main/install.sh | bash
```

### Option 2: Pre-built Binaries (Manual)

Download the latest pre-compiled archive for your platform (Linux x86_64 or macOS Apple Silicon) from the [GitHub Releases](https://github.com/shadowmkj/kmprs/releases) page:

```bash
# Extract the archive
tar -xzf kmprs-v*-*.tar.gz

# Move binary into your PATH
sudo mv kmprs /usr/local/bin/
```

### Option 3: Build from Source

#### Prerequisites
- A C11-compliant compiler (`gcc` or `clang`)
- [Just](https://github.com/casey/just) task runner (optional, but recommended)

#### Build Steps
```bash
# Clone the repository
git clone https://github.com/shadowmkj/kmprs.git
cd kmprs

# Build optimized release binary
just build-release

# (Alternative without Just)
gcc -Wall -Wextra -Werror -pedantic -std=c11 -O3 -DNDEBUG \
  main.c helper.c core.c shannon.c bit_io.c format.c codec.c -o kmprs

# Optional: Install binary to /usr/local/bin
sudo cp kmprs /usr/local/bin/
```

---

## Usage

### Compression

Compress any file into a `.shn` container:

```bash
# Default output creates <input_file>.shn
kmprs document.txt

# Specify custom output path
kmprs document.txt compressed_archive.shn
```

### Decompression

Decompress a `.shn` container using the `-d` flag:

```bash
# Automatically strips .shn extension (restores document.txt)
kmprs -d document.txt.shn

# Specify custom output destination
kmprs -d compressed_archive.shn restored_document.txt
```

---

## Testing & Verification

```bash
# Run unit tests and CLI roundtrip verification
just test

# Run tests under AddressSanitizer and UndefinedBehaviorSanitizer
just test-asan

# Run clang-tidy static analysis
just tidy
```

