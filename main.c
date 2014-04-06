#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "encoder.h"
#include "serial.h"
#include "leds.h"
#include "mass_storage.h"
#include "mp3.h"
#include "sd.h"
#include "tenths.h"
#include "player.h"

////////////////////////////////////////////////////////////////////////////////
int freeRam (void) {
    extern int __heap_start, *__brkval;
    volatile int v;
    return (int) (&v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval));
}

int main(void)
{
    // If the encoder switch is pressed on startup, jump to DFU bootloader.
    encoder_bootloader_check();

    // Initialize all of the peripherals
    encoder_init();
    LEDs_init();
    serial_init();
    tenths_init();

    mass_storage_init();
    sei();

    LEDs[0] = 5;

    sd_init();
    mp3_init();

    sd_read_test();

    // Initialize player state
    player_init();
    while (1)
    {
        if (usb_task())
        {
            LEDs_usb();
        }
        else
        {
            player_manage_buffer();
            player_update_state();
        }
    }
}
