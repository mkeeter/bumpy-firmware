#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "encoder.h"
#include "serial.h"
#include "leds.h"
#include "mp3.h"
#include "sd.h"

void show_volume()
{
    for (int i=0; i < 8; ++i)
    {
        if (i + 1 <= mp3_volume)    LED_brightness(i, 4);
        else                        LED_brightness(i, 0);
    }
}

int main()
{
    LEDs_init();
    encoder_init();
    serial_init();
    sei();

    printf("Booting up...\n");

    if (sd_init())  { printf("SD card initialized!\n"); }
    if (mp3_init()) { printf("VS1003  initialized!\n"); }

    uint8_t buffer[MP3_BUFFER_SIZE];
    bool buffer_empty = true;

    show_volume();

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

        const int e = encoder;
        const bool s = encoder_switch;
        if (e)
        {
            // Set encoder's recorded value to 0
            cli();
            encoder = 0;
            sei();

            if (e > 0)
            {
                if (s)  sd_next_song();
                else    mp3_volume_up();
            }
            else
            {
                if (s)  sd_prev_song();
                else    mp3_volume_down();
            }

            show_volume();
        }
    }
}
