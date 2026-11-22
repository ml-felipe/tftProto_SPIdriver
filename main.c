#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include "include/config_and_spi.h"
#include "include/print.h"

char choice;
int x_1, y_1, x_2, y_2;
int aux = 0;
char buffer[BUFFER_SIZE];
uint16_t aux_color;

/* Bound numeric input before conversion; malformed input exits cleanly. */
static int read_integer(int *value) {
    char token[64], *end;
    if (scanf("%63s", token) != 1) return 0;
    errno = 0;
    long number = strtol(token, &end, 10);
    if (errno || *end || end == token || number < INT_MIN || number > INT_MAX)
        return 0;
    *value = (int)number;
    return 1;
}

int chooseMode(void) {
    printf("\nSELECT A MODE\n");
    printf("1. Pixel print\n");
    printf("2. Line print\n");
    printf("3. Rectangle print\n");
    printf("4. String print\n");
    printf("5. Screen print\n");
    printf("6. Change color\n");
    printf("7. Change backgorund color\n");
    printf("8. Open image\n");
    printf("q. Quit\n");
    if (scanf(" %c", &choice) != 1) return 1;
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {}
    switch(choice) {
    case '1':
        printf("\nPIXEL PRINT MODE\n");
        printf("Enter (x, y) position\n");
        if (read_integer(&x_1) != 1) return 1;
        if (read_integer(&y_1) != 1) return 1;
        printArea(x_1, y_1, x_1, y_1, 1);
        break;
    case '2':
        printf("\nLINE PRINT MODE\n");
        printf("1. Horizontal\n");
        printf("2. Vertical\n");
        if (read_integer(&aux) != 1) return 1;
        if(aux == 1) {
            printf("Enter x position\n");
            if (read_integer(&x_1) != 1) return 1;
            printf("Enter y_1 position\n");
            if (read_integer(&y_1) != 1) return 1;
            printf("Enter y2 position\n");
            if (read_integer(&y_2) != 1) return 1;
            printArea(x_1, y_1, x_1, y_2, 1);
        }  
        else if(aux == 2) {
            printf("Enter y position\n");
            if (read_integer(&y_1) != 1) return 1;
            printf("Enter x_1 position\n");
            if (read_integer(&x_1) != 1) return 1;
            printf("Enter x_2 position\n");
            if (read_integer(&x_2) != 1) return 1;
            printArea(x_1, y_1, x_2, y_1, 1);
        }
        break;
    case '3':
        printf("\nRECTANGLE PRINT MODE\n");
        printf("1. Fill\n");
        printf("2. Not fill\n");
        if (read_integer(&aux) != 1) return 1;
        if(aux == 2) aux = 0;
        printf("Enter (x_1, y_1) position\n");
        if (read_integer(&x_1) != 1) return 1;
        if (read_integer(&y_1) != 1) return 1;
        printf("Enter (x_2, y2) position\n");
        if (read_integer(&x_2) != 1) return 1;
        if (read_integer(&y_2) != 1) return 1;
        printArea(x_1, y_1, x_2, y_2, aux);
        break;
    case '4':
        printf("\nSTRING PRINT MODE\n");
        printf("(%d, %d) Enter a string: ", x_String, y_String);
        if (!fgets(buffer, sizeof(buffer), stdin)) return 1;
        printChar(buffer);
        break;
    case '5':
        printf("SCREEN PRINT MODE\n");
        printArea(X_ORIGIN, Y_ORIGIN, X_SIZE - 1, Y_SIZE - 1, 1);
        break;
    case '6':
        printf("\nCHANGE COLOR MODE\n");
        printf("1. Black    12. Cyan        23. Teal\n");
        printf("2. Navy     13. Red         24. Fuchsia\n");
        printf("3. D.Green  14. Magenta     25. Aqua\n");
        printf("4. D.Cyan   15. Yellow      26. Skyblue\n");
        printf("5. Maroon   16. White       27. Violet\n");
        printf("6. Purple   17. Orange      28. Gold\n");
        printf("7. Olive    18. Mint        29. Salmon\n");
        printf("8. L.Grey   19. Pink        30. Coral\n");
        printf("9. D.Grey   20. Silver      31. Crimsom\n");
        printf("10. Blue    21. Gray        32. Brown\n");
        printf("11. Green   22. Lime        33. Indigo\n");
        if (read_integer(&aux) != 1) return 1;
        color = get_color(aux);
        break;
    case '7':
        printf("\nCHANGE BACKGORUNDCOLOR MODE\n");
        printf("1. Black    12. Cyan        23. Teal\n");
        printf("2. Navy     13. Red         24. Fuchsia\n");
        printf("3. D.Green  14. Magenta     25. Aqua\n");
        printf("4. D.Cyan   15. Yellow      26. Skyblue\n");
        printf("5. Maroon   16. White       27. Violet\n");
        printf("6. Purple   17. Orange      28. Gold\n");
        printf("7. Olive    18. Mint        29. Salmon\n");
        printf("8. L.Grey   19. Pink        30. Coral\n");
        printf("9. D.Grey   20. Silver      31. Crimsom\n");
        printf("10. Blue    21. Gray        32. Brown\n");
        printf("11. Green   22. Lime        33. Indigo\n");
        if (read_integer(&aux) != 1) return 1;
        last_background_color = background_color;
        background_color = get_color(aux);
        printScreen();
        break;
    case '8':
        printf("\nIMAGE PRINT MODE\n");
        printf("Enter filename: ");
        if (!fgets(buffer, sizeof(buffer), stdin)) return 1;
        buffer[strcspn(buffer, "\n")] = '\0';
        printIMG(buffer);
        break;
    case '9':
        printf("\nSUMMON VORP\n"); // Little easter egg since it was the first image I printed :)
        aux_color = color;
        color = TFT_WHITE;
        printArea(X_ORIGIN, Y_ORIGIN, X_SIZE - 1, Y_SIZE - 1, 1);
        printIMG("vorp.bmp");
        color = aux_color;
        break;
    case 'q':
        return 1;
        break;
    default:
        printf("\nINVALID INPUT!\n");
        break;
    }
    return 0;
}

int main(void) {
    if (!bcm2835_init()) {
        printf("bcm2835_init failed. Are you running as root??\n");
        return 1;
    }
  
    printf("bcm2835 initialized\n");
   
    if (!bcm2835_spi_begin()) {
        printf("bcm2835_spi_begin failed. Are you running as root??\n");
        bcm2835_close();
        return 1;
    }
    printf("spi initialized\n\n");

    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   // The default
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_16);    // 16 = 15.625MHz on Rpi2 (MAX FOR TFT PROTO REV 1.01)
    bcm2835_spi_chipSelect(BCM2835_SPI_CS_NONE);                  // Manual control

    // Pin configuration
    bcm2835_gpio_fsel(CS_PIN, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(RESET_PIN, BCM2835_GPIO_FSEL_OUTP);

    // Initial values
    bcm2835_gpio_write(CS_PIN, HIGH);
    bcm2835_gpio_write(RESET_PIN, HIGH);
    bcm2835_delay(200); // 200 ms

    tftReset();

    printf("Resetting screen\n\n");
    printScreen();

    while(!chooseMode()) { }
    bcm2835_spi_end();  
    bcm2835_close();

    return 0;
}
