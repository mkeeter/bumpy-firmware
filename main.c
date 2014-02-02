#include <stdbool.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "encoder.h"
#include "serial.h"
#include "leds.h"
#include "mp3.h"
#include "sd.h"

void show_volume(bool bright)
{
    for (int i=0; i < 8; ++i)
    {
        if (i + 1 <= mp3_volume)    LED_brightness(i, bright ? 4 : 2);
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
    bool playing = false;
    bool scrolled = false;

    show_volume(playing);

    bool switch_status = encoder_switch;

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

        if (mp3_wants_data() && playing)
        {
            mp3_send_data(buffer);
            buffer_empty = true;
        }

        // Handle wheel + button interactions
        const int e = encoder;
        const bool s = encoder_switch;
        bool refresh = false;

        // Start or stop playing based on button press.
        if (s != switch_status) {
            if (s)                  scrolled = false;
            else if (!scrolled)     playing = !playing;
            switch_status = s;
            refresh = true;
        }

        // Adjust volume or go back and forward in track list.
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
            refresh = true;
            scrolled = true;
        }

        if (refresh)    show_volume(playing);
    }
}
