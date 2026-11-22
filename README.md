# TFT SPI driver

Undergraduate embedded-systems project developed for DSBM (Design of Systems Based on Microcontrollers) at FIB. A Raspberry Pi drives a 240 × 320 TFT over SPI and provides a terminal menu for pixels, rectangles, text, background changes and BMP images.

## Architecture

- `config_and_spi.c`: GPIO reset, controller initialization and SPI transactions.
- `print.c`: drawing, 5 × 7 font rendering and a 240 × 320 RGB565 shadow buffer (153,600 bytes).
- `bmp.c`: hardware-independent, bounded 24-bit BMP decoding.
- `main.c`: interactive demonstrations and resource cleanup.

Register contents use one data byte; RGB565 pixels use two, most significant byte first. The initialization sequence matches the HX8347-D register map. See the [controller datasheet, section 5.1.3.1](https://www.displayfuture.com/engineering/specs/controller/HX8347-D.pdf). Confirm the controller and interface straps on your module before connecting it. Register readback is unsupported by this SPI0 backend: `readREG()` returns `-1` and sets `errno` to `ENOTSUP`, rather than reporting a fabricated ID.

## Hardware

Original setup: Raspberry Pi 2 and TFT Proto rev 1.01. Pin numbers in code are BCM GPIO numbers.

| Signal | BCM GPIO | Physical header pin |
| --- | --- | --- |
| TFT SDI / MOSI | 10 | 19 |
| SPI clock | 11 | 23 |
| TFT chip select | 8 | 24 |
| TFT reset | 17 | 11 |
| MISO (declared, unused) | 9 | 21 |

Use a common ground and check the board's power, logic-level and interface-jumper requirements against its schematic. The repository does not provide a complete power/backlight wiring diagram. SPI uses mode 0, MSB first and manual chip select. Divider 16 assumes the original Pi clock configuration; do not assume the same frequency or compatibility on other Pi models. Bidirectional SDA readback is not implemented.

## Build and run

Install a C compiler, GNU Make and the [bcm2835 library](https://www.airspayce.com/mikem/bcm2835/) on the Raspberry Pi, following that library's installation instructions. Its headers and library must be visible to the compiler and linker.

```sh
make
sudo ./tftDriver.exe
```

Run from the repository root so `images/` resolves correctly. The program needs GPIO/SPI access; the original setup uses root. Choose `q` to exit. EOF or invalid numeric input also ends the demo and releases resources. `CC`, `CFLAGS`, `CPPFLAGS` and `LDFLAGS` can be supplied to Make for local toolchain paths.

Images must be uncompressed Windows 24-bit BMPs, at most 320 pixels wide and 240 pixels high. Bottom-up and top-down storage and row padding are handled. The demo's landscape rotation is preserved: image columns advance along display Y, and bottom rows advance along display X. Oversized or truncated files are rejected before drawing. Text uses the bundled font and wraps across the display.

## Host checks

No GPIO access or bcm2835 installation is needed for these targets:

```sh
make test host-build
make analyze                 # GCC -fanalyzer
make clean
make test host-build CC=clang SANITIZE='-fsanitize=address,undefined -fno-omit-frame-pointer'
```

Tests cover BMP validation, truncation, padding, orientation, bundled images, SPI byte framing, rectangle edges and text boundaries. `host-build` links the complete application against a test backend; it does not test physical SPI. Clean before changing compiler or sanitizer flags. See [AUDIT.md](AUDIT.md) for measured verification status and outstanding checks.

## Limitations

Drawing is synchronous and uses individual byte transfers. The shadow buffer tracks colors, not separate graphical objects; changing the background swaps matching colors throughout it. Image decoding allocates a bounded pixel buffer. No framebuffer integration, readback, performance guarantee or hardware portability claim is made. GPIO timing, power sequencing and display orientation need a bench check after changes.

## Demos

**Print Screen**

https://github.com/user-attachments/assets/716b16c6-5ba3-461f-b4a8-a7d27c74bc42

**Print Rectangle**

https://github.com/user-attachments/assets/dfd571b8-4b64-42c6-9c9c-d91a8242fadb

**Print String**

https://github.com/user-attachments/assets/89bc0d72-e08f-4d47-bd1d-789e6242fa19

**Print Image**

https://github.com/user-attachments/assets/cf8e8d88-01b8-451f-9a81-86fa527328eb

The recordings and media in `assets/` are existing project demonstrations, not evidence that the current changes were tested on hardware. Original source credits, including the font attribution, are retained.
