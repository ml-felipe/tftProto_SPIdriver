#include "include/bmp.h"
#include <limits.h>
#include <stdlib.h>

static uint32_t le32(const unsigned char *p) {
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 |
           (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}
static unsigned le16(const unsigned char *p) {
    return (unsigned)p[0] | (unsigned)p[1] << 8;
}
int bmp_read(FILE *file, int max_width, int max_height, BmpImage *output) {
    unsigned char header[54];
    if (!file || !output || max_width <= 0 || max_height <= 0 ||
        fread(header, 1, sizeof header, file) != sizeof header) return -1;
    uint32_t offset = le32(header + 10), dib = le32(header + 14);
    uint32_t width = le32(header + 18), raw_height = le32(header + 22);
    int top_down = (raw_height & UINT32_C(0x80000000)) != 0;
    uint32_t height = top_down ? ~raw_height + 1u : raw_height;
    if (header[0] != 'B' || header[1] != 'M' || dib < 40 ||
        (uint64_t)offset < (uint64_t)14 + dib ||
        le16(header + 26) != 1 || le16(header + 28) != 24 ||
        le32(header + 30) != 0 || width == 0 || height == 0 ||
        width > (unsigned)max_width || height > (unsigned)max_height) return -1;
    uint64_t stride64 = ((uint64_t)width * 3 + 3) & ~UINT64_C(3);
    uint64_t end = offset + stride64 * height;
    if (end > LONG_MAX || stride64 > SIZE_MAX ||
        (uint64_t)width * height > SIZE_MAX / sizeof(uint16_t)) return -1;
    if (fseek(file, 0, SEEK_END) != 0) return -1;
    long size = ftell(file);
    if (size < 0 || (uint64_t)size < end ||
        fseek(file, (long)offset, SEEK_SET) != 0) return -1;
    size_t stride = (size_t)stride64;
    unsigned char *row = malloc(stride);
    uint16_t *pixels = malloc((size_t)width * height * sizeof *pixels);
    if (!row || !pixels) { free(row); free(pixels); return -1; }
    for (uint32_t y = 0; y < height; ++y) {
        if (fread(row, 1, stride, file) != stride) {
            free(row); free(pixels); return -1;
        }
        uint32_t dest_y = top_down ? height - 1 - y : y;
        for (uint32_t x = 0; x < width; ++x) {
            const unsigned char *bgr = row + 3 * x;
            pixels[(size_t)dest_y * width + x] =
                ((bgr[2] & 0xf8) << 8) | ((bgr[1] & 0xfc) << 3) | (bgr[0] >> 3);
        }
    }
    free(row);
    *output = (BmpImage){(int)width, (int)height, pixels};
    return 0;
}
