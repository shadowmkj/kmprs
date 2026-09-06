## [v0.1.1](https://github.com/shadowmkj/kmprs/releases/tag/v0.1.1) - 2026-09-06

### 🚀 Features

- *(install)* Add install script for kmprs - ([9fef021](https://github.com/shadowmkj/kmprs/commit/9fef021cabd53982b13da2dcc422a9e3e0f8579a)) (from [`@shadowmkj`](https://github.com/shadowmkj))

### ⚡️ Performance

- *(io)* Implement 4 KiB block buffering and inlining for BitWriter - ([2eb075d](https://github.com/shadowmkj/kmprs/commit/2eb075db980d0361075da521cc8223478ff87c53)) (from [`@shadowmkj`](https://github.com/shadowmkj))

### 👷 CI

- *(release)* Switch to orhun/git-cliff-action and add env vars - ([0aaf4fe](https://github.com/shadowmkj/kmprs/commit/0aaf4fe8183a6b6be60a7c9529856579d25be7a0)) (from [`@shadowmkj`](https://github.com/shadowmkj))

### ⚙️ Misc

- *(readme)* Expand installation, usage, and testing sections - ([c24c0db](https://github.com/shadowmkj/kmprs/commit/c24c0dbdc78ffeaa062da83af6eb307205c4e86b)) (from [`@shadowmkj`](https://github.com/shadowmkj))
- *(readme)* Update installation options and add curl installer - ([88c162b](https://github.com/shadowmkj/kmprs/commit/88c162b2e372bd2a03ea42ad388bc3100869536a)) (from [`@shadowmkj`](https://github.com/shadowmkj))
- Add performance benchmarks and benchmark chart to README - ([bfc5a2a](https://github.com/shadowmkj/kmprs/commit/bfc5a2a3a3fe0b6662b7ab9f0bd1cb3a9b562fd7)) (from [`@shadowmkj`](https://github.com/shadowmkj))
- Add BitWriter I/O buffering impact benchmark to README - ([35ce287](https://github.com/shadowmkj/kmprs/commit/35ce2871a38ba6ebf70fa4847e5219d1cde7b0be)) (from [`@shadowmkj`](https://github.com/shadowmkj))
- Add compression ratio and size comparison to README - ([5df272f](https://github.com/shadowmkj/kmprs/commit/5df272f39f7cd564b9f0bb77c086b528776b7557)) (from [`@shadowmkj`](https://github.com/shadowmkj))
## [v0.1.0](https://github.com/shadowmkj/kmprs/releases/tag/v0.1.0) - 2026-09-04

### 🚀 Features

- *(helper)* Add frequency printing helper module - ([cac430a](https://github.com/shadowmkj/kmprs/commit/cac430a58cc9d2a77a1052a44742495b89cd2219)) (from [`@shadowmkj`](https://github.com/shadowmkj))
- Add symbol frequency sorting and probability calculation - ([f9ed954](https://github.com/shadowmkj/kmprs/commit/f9ed954cba04a683ff43cbc7e293ed4c45ca7622)) (from [`@shadowmkj`](https://github.com/shadowmkj))
- *(shannon)* Add shannon tree construction and visualization - ([e6f5b30](https://github.com/shadowmkj/kmprs/commit/e6f5b30ccd6fef651aa3b0029cfd8ee62dc28f3a)) (from [`@shadowmkj`](https://github.com/shadowmkj))
- *(shannon)* Add codebook generation and related structs - ([546e56e](https://github.com/shadowmkj/kmprs/commit/546e56e907e9d8c13162c4f4dd48a406ecb26456)) (from [`@shadowmkj`](https://github.com/shadowmkj))
- *(bit_io)* Add bit-level I/O utilities - ([ef1100a](https://github.com/shadowmkj/kmprs/commit/ef1100a9de2e96e3ba21da9d28b30c47eeea17fe)) (from [`@shadowmkj`](https://github.com/shadowmkj))
- *(format)* Add .shn header serialization/deserialization and tests - ([1901bd0](https://github.com/shadowmkj/kmprs/commit/1901bd00da9f0e9139f9a57f31f406ed9f85e9b9)) (from [`@shadowmkj`](https://github.com/shadowmkj))
- *(main)* Add bit-level encoding with codebook generation - ([7c75d6e](https://github.com/shadowmkj/kmprs/commit/7c75d6e77772464e7be9f58df0c5272208f6ec64)) (from [`@shadowmkj`](https://github.com/shadowmkj))
- *(codec)* Add streaming codec module with compress and decompress functions - ([34a1de3](https://github.com/shadowmkj/kmprs/commit/34a1de3786e63cca6fa27edbea87dbf196d5c2c1)) (from [`@shadowmkj`](https://github.com/shadowmkj))

### 🐛 Bug Fixes

- Correct printf format specifier and add POSIX_C_SOURCE - ([1b4ecc5](https://github.com/shadowmkj/kmprs/commit/1b4ecc588de18924a99b8ad1ef5954cb7a6cfc0b)) (from [`@shadowmkj`](https://github.com/shadowmkj))
- *(helper)* Cast frequency to long long unsigned int - ([84acc0b](https://github.com/shadowmkj/kmprs/commit/84acc0b238b4e3707fb2110150772f2b425a178d)) (from [`@shadowmkj`](https://github.com/shadowmkj))

### ⚡️ Performance

- *(bit_io, codec)* Use block‑buffered bit reservoir and batch output - ([356a184](https://github.com/shadowmkj/kmprs/commit/356a1846e75e70d27dbf29045453e5c93dbfb27d)) (from [`@shadowmkj`](https://github.com/shadowmkj))

### 👷 CI

- Setup clang-tidy static analysis and Doxygen docs - ([d2aa429](https://github.com/shadowmkj/kmprs/commit/d2aa429ade9faaa47abfeb0a8fafba6f327bbac8)) (from [`@shadowmkj`](https://github.com/shadowmkj))
- *(release)* Add GitHub release workflow and build-release target - ([c875148](https://github.com/shadowmkj/kmprs/commit/c8751486e9dfbeb980ce130d63995a6ecd80ade3)) (from [`@shadowmkj`](https://github.com/shadowmkj))

### ⚙️ Misc

- Init project and implement frequency counting - ([a9e4963](https://github.com/shadowmkj/kmprs/commit/a9e4963318051aabe422117d96e9221a7734f490)) (from [`@shadowmkj`](https://github.com/shadowmkj))
- Setup ci and cliff.toml - ([2d39c6c](https://github.com/shadowmkj/kmprs/commit/2d39c6c83666b4eda3d2ac67a57c32c336c13b61)) (from [`@shadowmkj`](https://github.com/shadowmkj))
- *(core)* Extract symbol table generation and optimize storage - ([dc20fbe](https://github.com/shadowmkj/kmprs/commit/dc20fbecad8a7e5ac62dfc4a3fdb0abe607fab73)) (from [`@shadowmkj`](https://github.com/shadowmkj))
- Replace constant 256 with ALPHABET_SIZE and clean up unused code - ([7e9c7b1](https://github.com/shadowmkj/kmprs/commit/7e9c7b187e1ef262a23123f6fa630f230df2632d)) (from [`@shadowmkj`](https://github.com/shadowmkj))
- *(shannon)* Use unsigned literals in bit shifts - ([781f137](https://github.com/shadowmkj/kmprs/commit/781f1372e72d2333511300e3611d684f58ffe34f)) (from [`@shadowmkj`](https://github.com/shadowmkj))
- Document core headers and define bit I/O interface - ([9f416af](https://github.com/shadowmkj/kmprs/commit/9f416afc7c1f3a46c0365dee6f1a555d843b0fd6)) (from [`@shadowmkj`](https://github.com/shadowmkj))

### 👥 New Contributors

- [`@shadowmkj`](https://github.com/shadowmkj) made their first contribution

