#include <stdio.h>
#include <avr/io.h>
#include <LUFA/Drivers/Peripheral/SPI.h>

#include "mp3.h"

int mp3_volume = 5;

void mp3_write(const uint8_t addr, const uint16_t data);
static void mp3_write_volume(void);


// Initializes and checks mp3 player.  Returns 1 if successful, 0 otherwise.
int mp3_init(void)
{
    // MP3 chip select, data select, chip reset
    DDRF  |= (1 << PF4) | (1 << PF1) | (1 << PF5);
    PORTF |= (1 << PF4) | (1 << PF1) | (1 << PF5);

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
    if (version != 3)
        return 0;

    mp3_write_volume();

    return 1;
}

static inline void mp3_select(void)
{
    PORTF &= ~(1 << PF4);
}

static inline void mp3_deselect(void)
{
    PORTF |= (1 << PF4);
}

static inline void mp3_data_select(void)
{
    PORTF &= ~(1 << PF1);
}

static inline void mp3_data_deselect(void)
{
    PORTF |= (1 << PF1);
}

// Checks DREQ line and returns True if it's high
inline bool mp3_wants_data(void)
{
    return PINF & (1 << PF6);
}


void mp3_send_data(uint8_t* buffer)
{
    // Select the mp3 for a data transmission
    mp3_data_select();

    for (int i=0; i < MP3_BUFFER_SIZE; ++i)
    {
        SPI_SendByte(buffer[i]);
    }

    // Deselect SDI port.
    mp3_data_deselect();
}

static inline void mp3_wait(void)
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

    SPI_SendByte(0b00000011); // opcode for read
    SPI_SendByte(addr);

    // Use dummy sends to get out data
    uint16_t out = 0;
    out = SPI_ReceiveByte();
    out <<= 8;
    out = SPI_ReceiveByte();

    mp3_deselect();

    return out;
}

void mp3_write(const uint8_t addr, const uint16_t data)
{
    mp3_wait();
    mp3_select();

    SPI_SendByte(0b00000010); // opcode for write
    SPI_SendByte(addr);
    SPI_SendByte(data >> 8);
    SPI_SendByte(data & 0xff);

    mp3_deselect();
}


static void mp3_write_volume(void)
{
    uint16_t v = 10*(8 - mp3_volume);
    mp3_write(0xb, (v << 8) | v);
}

void mp3_volume_up(void)
{
    if (mp3_volume < 8)
    {
        mp3_volume++;
        mp3_write_volume();
    }
}


void mp3_volume_down(void)
{
    if (mp3_volume > 1)
    {
        mp3_volume--;
        mp3_write_volume();
    }
}
