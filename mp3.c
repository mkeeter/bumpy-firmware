#include <stdio.h>
#include <avr/io.h>

#include "mp3.h"
#include "macros.h"

int mp3_volume = 8;

void mp3_write(const uint8_t addr, const uint16_t data);

int mp3_init()
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

    // Turn SPI frequency doubling off for this one communication
    SPSR &= ~(1 << SPI2X);

    // Turn up clock multiplier
    mp3_write(0x3, 0x9800);

    // Turn SPI frequency doubling back on, since the VS1003 now
    // has a clock multiplier enabled and can talk fast.
    SPSR |= (1 << SPI2X);

    // Check to make sure that this chip is the right one.
    const uint16_t version = (mp3_read(0x1) & 0xf0) >> 4;
    if (version != 3) {
        printf("Error: Unexpected VS1003ds version (%i)", version);
        return 0;
    }

    return 1;
}

static inline void mp3_select()
{
    CLEAR(PORTF, PF4);
}

static inline void mp3_deselect()
{
    SET(PORTF, PF4);
}

static inline void mp3_data_select()
{
    CLEAR(PORTF, PF1);
}

static inline void mp3_data_deselect()
{
    SET(PORTF, PF1);
}

// Checks DREQ line and returns True if it's high
inline bool mp3_wants_data()
{
    return PINF & (1 << PF6);
}


static inline uint8_t mp3_spi_send(const uint8_t b)
{
    SPDR = b;
    while(!(SPSR & (1 << SPIF)));
    CLEAR(SPSR, SPIF);
    return SPDR;
}

void mp3_send_data(uint8_t* buffer)
{

    // Select the mp3 for a data transmission
    mp3_data_select();

    for (int i=0; i < MP3_BUFFER_SIZE; ++i)
    {
        mp3_spi_send(buffer[i]);
    }

    // Deselect SDI port.
    mp3_data_deselect();

}

static inline void mp3_wait()
{
    // Wait for DREQ to go high (signaling that the mp3 chip
    // can take in an SPI command).
    while(!(PINF & (1 << PF6)));
}


uint16_t mp3_read(const uint8_t addr)
{
    mp3_wait();

    // Select the mp3 for a data transmission
    mp3_select();

    mp3_spi_send(0b00000011); // opcode for read
    mp3_spi_send(addr);

    // Use dummy sends to get out data
    uint16_t out = 0;
    out = mp3_spi_send(0);
    out <<= 8;
    out = mp3_spi_send(0);

    mp3_deselect();

    return out;
}

void mp3_write(const uint8_t addr, const uint16_t data)
{
    mp3_wait();
    mp3_select();

    mp3_spi_send(0b00000010); // opcode for write
    mp3_spi_send(addr);
    mp3_spi_send(data >> 8);
    mp3_spi_send(data & 0xff);

    mp3_deselect();
}


void mp3_volume_up()
{
    const uint16_t vol = mp3_read(0xb) & 0xff;
    if (vol >= 10)
    {
        mp3_write(0xb, ((vol - 10) << 8) | ((vol - 10) & 0xff));
        mp3_volume++;
    }
}


void mp3_volume_down()
{
    const uint8_t vol = mp3_read(0xb) & 0xff;
    if (vol <= 60)
    {
        mp3_write(0xb, ((vol + 10) << 8) | ((vol + 10) & 0xff));
        mp3_volume--;
    }
}
