#include <assert.h>
#include "bcm2835.h"
unsigned char mock_bytes[400000];
unsigned int mock_count;
static int selected;
int bcm2835_init(void) { return 1; }
int bcm2835_close(void) { return 1; }
int bcm2835_spi_begin(void) { return 1; }
void bcm2835_spi_end(void) {}
void bcm2835_spi_setBitOrder(uint8_t v) { (void)v; }
void bcm2835_spi_setDataMode(uint8_t v) { (void)v; }
void bcm2835_spi_setClockDivider(uint16_t v) { (void)v; }
void bcm2835_spi_chipSelect(uint8_t v) { (void)v; }
void bcm2835_gpio_fsel(uint8_t p, uint8_t v) { (void)p; (void)v; }
void bcm2835_gpio_write(uint8_t p, uint8_t v) { if (p == 8) selected = !v; }
void bcm2835_delay(unsigned int ms) { (void)ms; }
uint8_t bcm2835_spi_transfer(uint8_t v) {
    assert(selected);
    assert(mock_count < sizeof mock_bytes);
    mock_bytes[mock_count++] = v;
    return 0;
}
