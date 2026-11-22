# TFT SPI driver

Undergraduate project for DSBM (Design of Systems Based on Microcontrollers) at FIB. A Raspberry Pi drives a 240 × 320 TFT over SPI, with terminal demonstrations for pixels, rectangles, text, background colors and BMP images.

## Design

`config_and_spi.c` handles reset, initialization and SPI transactions. `print.c` provides drawing and a 153,600-byte RGB565 shadow buffer. `bmp.c` decodes images independently of the hardware; `main.c` provides the menu.

The initialization follows the HX8347-D register map. Register writes send one data byte; RGB565 pixels send two, MSB first ([datasheet §5.1.3.1](https://www.displayfuture.com/engineering/specs/controller/HX8347-D.pdf)). Readback is not implemented: `readREG()` returns `-1` with `errno=ENOTSUP`.

## Hardware

Original setup: Raspberry Pi 2 and TFT Proto rev 1.01. Code uses BCM GPIO numbering.

| Signal | BCM GPIO | Header pin |
| --- | --- | --- |
| TFT SDI / MOSI | 10 | 19 |
| SPI clock | 11 | 23 |
| Chip select | 8 | 24 |
| Reset | 17 | 11 |
| MISO (unused) | 9 | 21 |

Use a common ground and check the module's controller, interface straps, supply and logic levels against its schematic. A complete power/backlight wiring diagram is not included. SPI uses mode 0, manual chip select and clock divider 16; the resulting frequency depends on the Pi clock configuration.

## Build and run

Install a C compiler, GNU Make and the [bcm2835 library](https://www.airspayce.com/mikem/bcm2835/) on the Pi, then run from the repository root:

```sh
make
sudo ./tftDriver.exe
```

The original setup uses root for GPIO/SPI access. Choose `q` to quit; EOF or invalid numeric input also exits. Compiler and library paths can be supplied through `CC`, `CPPFLAGS`, `CFLAGS` and `LDFLAGS`.

Images are loaded from `images/`: uncompressed Windows 24-bit BMPs up to 320 × 240, with row padding and either storage orientation supported. Invalid or truncated files are rejected before drawing. The landscape mapping is preserved: image columns advance along display Y, and bottom rows advance along display X. Text uses the bundled 5 × 7 font and wraps across the screen. Background changes swap matching colors in the shadow buffer rather than tracking graphical objects.

## Tests and limitations

Host checks require no bcm2835 installation:

```sh
make test host-build
make analyze                 # GCC -fanalyzer
make clean
make test host-build CC=clang SANITIZE='-fsanitize=address,undefined -fno-omit-frame-pointer'
```

Clean before changing compiler or sanitizer flags. Tests cover BMP validation and bundled images, SPI byte framing, drawing boundaries, text wrapping and image window sizes. `host-build` links the full application against a mock SPI backend.

On the review host, GCC 16.2.1 tests, mock builds and static analysis passed. Clang 23.1.4 AddressSanitizer/UndefinedBehaviorSanitizer tests passed with `ASAN_OPTIONS=detect_leaks=0`; LeakSanitizer could not run under the container's tracing restrictions. Production builds were blocked by missing bcm2835. No Raspberry Pi or display tests ran.

The register-write correction changes physical SPI transactions and needs a logic-analyzer and display check before merging. Reset values and delays are unchanged. Recheck image orientation, text, background changes and the existing demos on the original hardware. Drawing remains synchronous; no timing or performance measurements are provided.

## Demos

**Print Screen**

https://github.com/user-attachments/assets/716b16c6-5ba3-461f-b4a8-a7d27c74bc42

**Print Rectangle**

https://github.com/user-attachments/assets/dfd571b8-4b64-42c6-9c9c-d91a8242fadb

**Print String**

https://github.com/user-attachments/assets/89bc0d72-e08f-4d47-bd1d-789e6242fa19

**Print Image**

https://github.com/user-attachments/assets/cf8e8d88-01b8-451f-9a81-86fa527328eb

These existing recordings predate the fixes. Media in `assets/` and original source credits are retained.
