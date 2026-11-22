#include <assert.h>
#include <errno.h>
#include <string.h>
#include "print.h"
extern unsigned char mock_bytes[];
extern unsigned int mock_count;
extern uint16_t screen_buffer[Y_SIZE][X_SIZE];
int main(void) {
    assert(get_bit(0x80, 7) == 1 && get_bit(0x80, -1) == 0);
    writeREG(0x1f, 0xd0);
    const unsigned char reg[] = {0x70, 0x1f, 0x72, 0xd0};
    assert(mock_count == sizeof reg && !memcmp(mock_bytes, reg, sizeof reg));
    mock_count = 0;
    writeDAT(0x1234);
    const unsigned char pixel[] = {0x72, 0x12, 0x34};
    assert(mock_count == sizeof pixel && !memcmp(mock_bytes, pixel, sizeof pixel));
    mock_count = 0;
    assert(readREG(0) == -1 && errno == ENOTSUP && mock_count == 0);
    color = TFT_RED;
    printArea(1, 2, 4, 5, 0);
    for (int y=2; y<=5; ++y) for (int x=1; x<=4; ++x)
        assert(screen_buffer[y][x] == ((y==2 || y==5 || x==1 || x==4) ? TFT_RED : 0));
    mock_count = 0;
    printArea(-1, 0, 0, 0, 1);
    assert(mock_count == 0);
    char empty[] = "";
    printChar(empty);
    assert(mock_count == 0);
    char one[] = "A";
    printChar(one);
    assert(y_String == 1 && mock_count != 0);
    x_String = 14; y_String = 27;
    printChar(one);
    assert(x_String == 0 && y_String == 0 && rerun == 1);
    mock_count = 0;
    printIMG("vorp.bmp");
    assert(mock_count > 34);
    /* Eight 4-byte register writes, 2-byte GRAM select, then pixel data. */
    assert(mock_bytes[32] == 0x70 && mock_bytes[33] == 0x22 && mock_bytes[34] == 0x72);
    unsigned height = ((unsigned)mock_bytes[11] << 8 | mock_bytes[15]) + 1;
    unsigned width = ((unsigned)mock_bytes[27] << 8 | mock_bytes[31]) + 1;
    assert(mock_count == 35 + 2 * width * height);
    assert(screen_buffer[0][0] == ((unsigned)mock_bytes[35] << 8 | mock_bytes[36]));
    return 0;
}
