#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "include/bmp.h"

#include "include/print.h"
#include "include/ascii5x7.h"

uint16_t background_color = TFT_BLACK;
uint16_t last_background_color = TFT_BLACK;
uint16_t color = TFT_WHITE;
uint16_t screen_buffer[Y_SIZE][X_SIZE] = {0};

int x_String = 0; // 15 max
int y_String = 0; // 28 max
int rerun = 0;

int get_bit(unsigned char byte, int n) {
    // Shift the byte to the right by n bits, then AND with 1 to isolate the bit
    return n >= 0 && n < 8 ? (byte >> n) & 1 : 0;
}

uint16_t get_color(int pick) {
    switch(pick) {
        case 1:  return TFT_BLACK;
        case 2:  return TFT_NAVY;
        case 3:  return TFT_DARKGREEN;
        case 4:  return TFT_DARKCYAN;
        case 5:  return TFT_MAROON;
        case 6:  return TFT_PURPLE;
        case 7:  return TFT_OLIVE;
        case 8:  return TFT_LIGHTGREY;
        case 9:  return TFT_DARKGREY;
        case 10: return TFT_BLUE;
        case 11: return TFT_GREEN;
        case 12: return TFT_CYAN;
        case 13: return TFT_RED;
        case 14: return TFT_MAGENTA;
        case 15: return TFT_YELLOW;
        case 16: return TFT_WHITE;
        case 17: return TFT_ORANGE;
        case 18: return TFT_GREENYELLOW;
        case 19: return TFT_PINK;
        case 20: return TFT_SILVER;
        case 21: return TFT_GRAY;
        case 22: return TFT_LIME;
        case 23: return TFT_TEAL;
        case 24: return TFT_FUCHSIA;
        case 25: return TFT_AQUA;
        case 26: return TFT_SKYBLUE;
        case 27: return TFT_VIOLET;
        case 28: return TFT_GOLD;
        case 29: return TFT_SALMON;
        case 30: return TFT_CORAL;
        case 31: return TFT_CRIMSON;
        case 32: return TFT_BROWN;
        case 33: return TFT_INDIGO;
        default: return TFT_BLACK; // Default case for invalid numbers
    }

}

void printArea(int x1, int y1, int x2, int y2, int fill) {
    if(x1 >= X_SIZE || x2 >= X_SIZE || y1 >= Y_SIZE || y2 >= Y_SIZE ||
       x1 < 0 || x2 < 0 || y1 < 0 || y2 < 0) {
        printf("Out of bounds value\n");
        return;
    }

    if(fill) {

        int oX, oY, fX,fY;
        if(x1 <= x2) {
            oX = x1;
            fX = x2;
        }
        else {
            oX = x2;
            fX = x1;
        }
        if(y1 <= y2) {
            oY = y1;
            fY = y2;
        }
        else {
            oY = y2;
            fY = y1;
        }
        // Select area
        writeREG(0x02, oX >> 8);
        writeREG(0x03, oX & 0xFF);
        writeREG(0x04, fX >> 8);
        writeREG(0x05, fX & 0xFF);
        writeREG(0x06, oY >> 8);
        writeREG(0x07, oY & 0xFF);
        writeREG(0x08, fY >> 8);
        writeREG(0x09, fY & 0xFF);
        writeCMD(0x22);

        // Fill the entire area with the specified color
        bcm2835_gpio_write(CS_PIN, LOW);
        bcm2835_spi_transfer(0x72); // Data write command
        for(int i = oY; i <= fY; i++) {
            for(int j = oX; j <= fX; j++) {
                bcm2835_spi_transfer((uint8_t)(color >> 8));   // High byte
                bcm2835_spi_transfer((uint8_t)(color & 0xFF)); // Low byte
                screen_buffer[i][j] = color;
            }
        }
        bcm2835_gpio_write(CS_PIN, HIGH);
    } 
    else {
        printArea(x1, y1, x2, y1, 1); // Up
        printArea(x2, y1, x2, y2, 1); // Right
        printArea(x1, y2, x2, y2, 1); // Down
        printArea(x1, y1, x1, y2, 1); // Left
    }
}

void printChar(char *buffer) {
    if (!buffer) return;
    for(int n = 0; buffer[n] != '\0' && buffer[n] != '\n'; n++) {
        if(buffer[n] < 32 || buffer[n] > 125) buffer[n] = '!'; // Not recognized char
        for(int i = 0; i < 5; i++) {
            for (int j = 7 ; j >= 0; j--) {
                if(get_bit(Font5x7[5 * (buffer[n] - 32) + i], j)) {
                    printArea(232 - j * 2 - x_String * 15, 7 + i * 2 + y_String * 11, 231 - j * 2 - x_String * 15, 8 + i * 2 + y_String * 11, 1);
                }
                else if(rerun) {
                    uint16_t aux_color = color;
                    color = background_color;
                    printArea(232 - j * 2 - x_String * 15, 7 + i * 2 + y_String * 11, 231 - j * 2 - x_String * 15, 8 + i * 2 + y_String * 11, 1);
                    color = aux_color;
                }
            }
        }
        if(++y_String == 28) {
            y_String = 0;
            if(++x_String >= 15) {
                x_String = 0;
                rerun = 1;
            }
        }
    }
}

void printScreen(void) {
    writeREG(0x02, X_ORIGIN >> 8);
    writeREG(0x03, X_ORIGIN & 0xFF);
    writeREG(0x04, (X_SIZE - 1) >> 8);
    writeREG(0x05, (X_SIZE - 1) & 0xFF);
    writeREG(0x06, Y_ORIGIN >> 8);
    writeREG(0x07, Y_ORIGIN & 0xFF);
    writeREG(0x08, (Y_SIZE - 1) >> 8);
    writeREG(0x09, (Y_SIZE - 1) & 0xFF);
    writeCMD(0x22);
    // Fill the entire area with the specified color
    bcm2835_gpio_write(CS_PIN, LOW);
    bcm2835_spi_transfer(0x72); // Data write command
    for (int i = 0; i < Y_SIZE; i++) {
        for(int j =0; j < X_SIZE; j++) {
            if(screen_buffer[i][j] == last_background_color) {
                bcm2835_spi_transfer((uint8_t)(background_color >> 8));   // High byte
                bcm2835_spi_transfer((uint8_t)(background_color & 0xFF)); // Low byte
                screen_buffer[i][j] = background_color;
            }
            else if(screen_buffer[i][j] == background_color) {
                bcm2835_spi_transfer((uint8_t)(last_background_color >> 8));   // High byte
                bcm2835_spi_transfer((uint8_t)(last_background_color & 0xFF)); // Low byte
                screen_buffer[i][j] = last_background_color;
            }
            else {
                bcm2835_spi_transfer((uint8_t)(screen_buffer[i][j] >> 8));   // High byte
                bcm2835_spi_transfer((uint8_t)(screen_buffer[i][j]& 0xFF)); // Low byte
            }
        }
    }
    bcm2835_gpio_write(CS_PIN, HIGH);
}

void printIMG(const char *filename) {
    char filepath[256];
    if (!filename) return;
    int length = snprintf(filepath, sizeof(filepath), "images/%s", filename);
    if (length < 0 || (size_t)length >= sizeof(filepath)) {
        fprintf(stderr, "Image path is too long\n");
        return;
    }
    FILE *file = fopen(filepath, "rb");
    if (!file) { perror("Failed to open image"); return; }
    BmpImage image = {0};
    int result = bmp_read(file, Y_SIZE, X_SIZE, &image);
    fclose(file);
    if (result != 0) {
        fprintf(stderr, "Invalid, truncated, oversized or unsupported BMP\n");
        return;
    }
    /* Preserve the demo's landscape rotation: BMP bottom row maps to x=0. */
    int x_offset = 0, y_offset = 0;
#ifdef TFT_CENTER_IMAGES
    x_offset = (X_SIZE - image.height) / 2;
    y_offset = (Y_SIZE - image.width) / 2;
#endif
    writeREG(0x02, x_offset >> 8);
    writeREG(0x03, x_offset & 0xff);
    writeREG(0x04, (x_offset + image.height - 1) >> 8);
    writeREG(0x05, (x_offset + image.height - 1) & 0xff);
    writeREG(0x06, y_offset >> 8);
    writeREG(0x07, y_offset & 0xff);
    writeREG(0x08, (y_offset + image.width - 1) >> 8);
    writeREG(0x09, (y_offset + image.width - 1) & 0xff);
    writeCMD(0x22);
    bcm2835_gpio_write(CS_PIN, LOW);
    bcm2835_spi_transfer(0x72);
    for (int y = 0; y < image.width; ++y) {
        for (int x = 0; x < image.height; ++x) {
            uint16_t pixel = image.pixels[x * image.width + y];
            bcm2835_spi_transfer((uint8_t)(pixel >> 8));
            bcm2835_spi_transfer((uint8_t)pixel);
            screen_buffer[y + y_offset][x + x_offset] = pixel;
        }
    }
    bcm2835_gpio_write(CS_PIN, HIGH);
    free(image.pixels);
}
