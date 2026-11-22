# Source audit and verification

Audit date: 2026-11-22. Starting commit: `5a09ea6`. Scope: all C sources and headers, Makefile, README and BMP assets; existing recordings were retained. This is a source and host-software review, not a hardware certification.

## Verified issues addressed

| Area | Finding and change |
| --- | --- |
| SPI registers | `writeREG` sent two data bytes to an 8-bit register interface. It now sends one; `writeDAT` retains two-byte RGB565 transfers. Reference: HX8347-D datasheet §5.1.3.1, figures 5.27–5.28. |
| Register reads | `readREG` selected a register but returned constant zero. It now reports `ENOTSUP`; reset no longer treats it as a driver ID. Real bidirectional-SDA readback remains unimplemented. |
| BMP safety | Unchecked header/pixel reads, allocation, dimensions and offsets could cause overflow, out-of-bounds access or use of uninitialized bytes. A bounded decoder checks the file before rendering, decodes little-endian fields explicitly, handles padding and both row orientations, and releases allocations on failure. |
| BMP rendering | Inclusive window ends were one pixel too large. Window dimensions now match the exact pixel count. The original landscape mapping remains. |
| Rectangles | Outline rendering drew the right edge twice and omitted the top edge. All four edges are now drawn. |
| Text | Looking ahead at `buffer[n+1]` overread an empty string and dropped the last character without a newline. The loop checks the current character; wrapping avoids the invalid sixteenth text column. |
| Menu/resources | Input conversion and EOF were unchecked, and SPI-start failure leaked bcm2835 state. Numeric conversion is bounded with `strtol`, failed input exits, and initialization failures release resources. |
| Build | Added explicit sources, header dependency files, configurable flags and hardware-independent tests; generated files are ignored. |

## Checks actually run

On this Linux host with GCC 16.2.1 and Clang 23.1.4:

- `make test host-build analyze`: passed with GCC, including `-Wall -Wextra -Wpedantic -Werror` in host targets and `-fanalyzer` compilation of each application source.
- Clean Clang rebuild with `SANITIZE='-fsanitize=address,undefined -fno-omit-frame-pointer'`, then tests: passed with `ASAN_OPTIONS=detect_leaks=0`.
- LeakSanitizer initially failed because the container disallows its tracing operation. Leak detection was disabled only for that rerun; no leak-check result is claimed.
- Tests check short BMPs at every truncation boundary of a padded fixture, invalid headers/dimensions/offsets, both row orientations, RGB565 colors, all three bundled BMPs, register/pixel byte framing, bounds rejection, rectangle edges, empty/single-character strings, wrapping and image window/pixel counts.
- Separate process checks of the mock application passed for EOF, quit, invalid numeric input and an overflowing integer token.
- `git diff --check`: passed.
- Production `make`: blocked by missing `bcm2835.h` on this host. Mock linking is not a production build or hardware test.

## Remaining bench checks

Confirm the actual controller and serial interface straps, reset/power sequence, SPI frequency and 8-bit register transaction timing with a logic analyzer. Re-run all existing demonstrations and compare image rotation, window edges, text wrapping and background swaps. No Raspberry Pi, display, GPIO, timing, visual-output or benchmark test ran during this audit. The original initialization values and delay sequence are preserved. Background replacement still uses color equality, and the driver remains synchronous and write-only.
