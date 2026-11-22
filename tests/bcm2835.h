#ifndef TEST_BCM2835_H
#define TEST_BCM2835_H
#include <stdint.h>
#define LOW 0
#define HIGH 1
#define BCM2835_SPI_BIT_ORDER_MSBFIRST 1
#define BCM2835_SPI_MODE0 0
#define BCM2835_SPI_CLOCK_DIVIDER_16 16
#define BCM2835_SPI_CS_NONE 3
#define BCM2835_GPIO_FSEL_OUTP 1
int bcm2835_init(void);
int bcm2835_close(void);
int bcm2835_spi_begin(void);
void bcm2835_spi_end(void);
void bcm2835_spi_setBitOrder(uint8_t value);
void bcm2835_spi_setDataMode(uint8_t value);
void bcm2835_spi_setClockDivider(uint16_t value);
void bcm2835_spi_chipSelect(uint8_t value);
void bcm2835_gpio_fsel(uint8_t pin, uint8_t mode);
void bcm2835_gpio_write(uint8_t pin, uint8_t value);
void bcm2835_delay(unsigned int ms);
uint8_t bcm2835_spi_transfer(uint8_t value);
#endif
