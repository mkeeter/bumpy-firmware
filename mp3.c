#include <avr/io.h>

#include "mp3.h"
#include "macros.h"

#include "leds.h"

void mp3_init()
{
    // MP3 chip select
    OUTPUT(DDRF, PF4);
    SET(PORTF, PF4);

    // MP3 data select
    OUTPUT(DDRF, PF1);
    SET(PORTF, PF1);

    // MP3 chip reset
    OUTPUT(DDRF, PF5);
    SET(PORTF, PF5);

    // All of the spi stuff is configured by the SD card,
    // so we don't need to do anything here.
}

void mp3_select()
{
    CLEAR(PORTF, PF4);
}

void mp3_deselect()
{
    SET(PORTF, PF4);
}

uint8_t spi_send(const uint8_t b)
{
    SPDR = b;
    while(!(SPSR & (1 << SPIF)));
    CLEAR(SPSR, SPIF);
    return SPDR;
}

uint16_t mp3_read(const uint8_t addr)
{
    LEDs(0);
    // Wait for DREQ to go low
    while(!(PINF & (1 << PF6)));
    LEDs(1);

    // Select the mp3 for a data transmission
    mp3_select();

    spi_send(0b00000011); // opcode for read
    spi_send(addr);

    // Use dummy sends to get out data
    uint16_t out = 0;
    out = spi_send(0);
    out <<= 8;
    out = spi_send(0);

    mp3_deselect();

    return out;
}
