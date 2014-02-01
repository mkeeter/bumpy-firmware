#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "encoder.h"
#include "serial.h"
#include "leds.h"
#include "mp3.h"
#include "sd.h"

int main()
{
    LEDs_init();
    encoder_init();
    serial_init();
    mp3_init();
    sei();

    if (sd_init()) { printf("SD card initialized!\n"); }

    const uint16_t status = mp3_read(0x1);
    if ((status & 0xf0) == 0x30) {
        printf("VS1063ds status is good\n");
    } else {
        printf("Invalid VS1063ds status (%x%x)", status, status >> 8);
    }

    while (1)
    {
        LEDs(encoder_switch);
        printf("hello, world\n");
        _delay_ms(1000);
    }
}
