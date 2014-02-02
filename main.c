#include <stdbool.h>

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

    printf("Booting up...\n");

    if (sd_init())  { printf("SD card initialized!\n"); }
    if (mp3_init()) { printf("VS1003  initialized!\n"); }

    uint8_t buffer[MP3_BUFFER_SIZE];
    int buffer_empty = true;
    while (1)
    {
        if (buffer_empty) {
            // If we're at the end of a song, skip to a new one
            // and get a new buffer's worth of data.
            while (!sd_get_data(buffer, MP3_BUFFER_SIZE))
            {
                sd_next_song();
            }
            buffer_empty = false;
        }

        if (mp3_wants_data())
        {
            mp3_send_data(buffer);
            buffer_empty = true;
        }
    }
}
