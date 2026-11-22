#ifndef BMP_H
#define BMP_H
#include <stdint.h>
#include <stdio.h>
typedef struct {
    int width, height;
    uint16_t *pixels; /* RGB565, bottom row first; caller frees on success. */
} BmpImage;
/* Accept Windows 24-bit BI_RGB BMPs fitting the supplied positive bounds.
 * On failure return -1 and leave output untouched. */
int bmp_read(FILE *file, int max_width, int max_height, BmpImage *output);
#endif
