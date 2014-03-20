#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "encoder.h"
#include "serial.h"
#include "leds.h"
#include "mp3.h"
#include "sd.h"
#include "tenths.h"
#include "player.h"

////////////////////////////////////////////////////////////////////////////////

int main()
{
    // If the encoder switch is pressed on startup, jump to DFU bootloader.
    encoder_bootloader_check();

    // Initialize all of the peripherals
    encoder_init();
    LEDs_init();
    serial_init();
    tenths_init();
    sei();

    printf("Booting up...\n");

    if (sd_init())  { printf("SD card initialized!\n"); }
    if (mp3_init()) { printf("VS1003  initialized!\n"); }

    // Initialize player state
    player_init();
    while (1)
    {
        player_manage_buffer();
        player_update_state();
    }
}
